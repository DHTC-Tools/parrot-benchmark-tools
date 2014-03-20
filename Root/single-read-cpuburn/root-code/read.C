#include <stdlib.h>
#include <math.h>

#include "Riostream.h"
#include "TROOT.h"
#include "TFile.h"
#include "TNetFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TTreeCache.h"
#include "TTreePerfStats.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TTreeCacheUnzip.h"
#include "TEnv.h"

#include <iostream>
#include <fstream>
#include <sstream>


struct squareMatrix {
    double *elements;
    int size;
};

inline long getIndex(int row, int column, int size) {
    return size*row+column;
}

double dotProduct(squareMatrix *matrix1, int row, squareMatrix *matrix2, int column) {
    double product = 0;
    for (long i = getIndex(row, 0, matrix1->size); 
         i < (getIndex(row, 0, matrix1->size) + matrix1->size); 
         i++) {
        for (long j = getIndex(0, column, matrix1->size);
             j < (matrix2->size * matrix2->size); 
             j += matrix2->size)  {
            product +=  matrix1->elements[i] * matrix2->elements[j];
        }
    }
    return product;
}

void scaleRow(squareMatrix *matrix, int row, double scaling_factor) {
    long row_index = getIndex(row, 0, matrix->size);
    for (int i = 0; i < matrix->size; i++) {
        matrix->elements[row_index + i] *= scaling_factor;
    }
}

void swapRow(squareMatrix *matrix, int row1, int row2 ) {
    long row1_index = getIndex(row1, 0, matrix->size);
    long row2_index = getIndex(row2, 0, matrix->size);
    double temp = 0.0;
    for (int i = 0; i < matrix->size; i++) {
        temp = matrix->elements[row1_index + i]; 
        matrix->elements[row1_index + i] = matrix->elements[row2_index + i];
        matrix->elements[row2_index + i] = temp;
    }
}

void mac(squareMatrix *matrix, int src, int dest, double scaling_factor) {
    // Multiply and accumulate (e.g. dest' = src*scaling_factor+dest
    long src_index = getIndex(src, 0, matrix->size);
    long dest_index = getIndex(dest, 0, matrix->size);
    for (int i = 0; i < matrix->size; i++) {
      matrix->elements[dest_index + i] = matrix->elements[src_index + i] * scaling_factor + 
                               matrix->elements[dest_index + i];
    }
}

void initializeMatrix(squareMatrix *matrix, int seed = 42) {
    srand48(seed);
    long max_index = matrix->size * matrix->size;
    for (long i = 0; i < max_index; i++) {
	matrix->elements[i] = rand() % 100;
	if (matrix->elements[i] == 0) {
	    // don't want any chance of a row of zeros
	    // this lets us skip rearranging the matrix 
	    // when getting it's inverse
	    matrix->elements[i] += 1;
	}
    }
}

void initializeIdentityMatrix(squareMatrix *matrix ) {
    for (long i = 0; i < (matrix->size * matrix->size); i++) {
	matrix->elements[i] = 0;
    }
    for (int i = 0; i < matrix->size; i++) {
	matrix->elements[getIndex(i, i, matrix->size)]  = 1;
    }
}

void printMatrix(squareMatrix *matrix) {
  std::cout << "Matrix contents:" << std::endl;
  for (int i = 0; i < matrix->size; i++) {
      for (int j = 0; j < matrix->size; j++) {
          std::cout << matrix->elements[getIndex(i, j, matrix->size)] << " ";
      }
      std::cout << std::endl;
  }
}

bool matrixInvert(int size = 600, int seed = 42) {
    bool success = true;
    squareMatrix matrix ;
    matrix.size = size;
    matrix.elements = new double[size * size];

    squareMatrix identity_matrix;
    identity_matrix.size = size;
    identity_matrix.elements =  new double[size*size];
    initializeMatrix(&matrix, seed);
    initializeIdentityMatrix(&identity_matrix);
    // invert matrix using Gauss-Jordan elimination 
    // this won't work for singular matrices but almost all random
    // matrices are non-singular
    // Any operation on original matrix is also done on identity matrix
    // first step is to move rows that are all zeros to bottom of matrix
    // due to the way the matrix was initialized, this is not a concern 
    int pivot_row = 0;
    double pivot_value = 0;
    double scaling_factor = 0;
    for (int i = 0; i < size; i++) {
	// for each row starting from the top, use row operations to
	// make create an upper triangular matrix
	// each row operation is applied to both the original matrix
	// and the identity matrix instead of using an augmented matrix
        
        
        // First find the pivot row and pivot
        // pivot row is the row k such that k >= i and row[k][i] is 
        // max(row[j][i] s.t. i <= j < size)
        pivot_row = i;
        pivot_value = fabs(matrix.elements[getIndex(i, i, size)]);
        for (int j = i+1; j < size; j++) {
            if (fabs(matrix.elements[getIndex(j, i, size)]) > pivot_value) {
                pivot_row = j;
                pivot_value = matrix.elements[getIndex(j, i, size)];
            }
        }
        if (pivot_row != i) {
            // pivot_row is not i so we'll swap the ith row with it
            swapRow(&matrix, i, pivot_row);
            // do the same to identity matrix
            swapRow(&identity_matrix, i, pivot_row);
        }
        // scale and add row i to rows i+i -> size to get 0s for rows below row i 
        // in this column
        for (int j = i+1; j < size; j++) {
            scaling_factor = matrix.elements[getIndex(j, i, size)] / pivot_value * -1;
            mac(&matrix, i, j, scaling_factor);
            mac(&identity_matrix, i, j, scaling_factor);
        }
    }

    // now have upper triangular matrix, work from bottom row up and 
    // scale/add rows to get a diagonal matrix
    for (int i = size - 1; i >= 0; i--) {
        pivot_value = matrix.elements[getIndex(i, i, size)];
        for (int j = i - 1; j >= 0; j--) {
            scaling_factor = matrix.elements[getIndex(j, i, size)] / 
                            pivot_value * -1;
            mac(&matrix, i, j, scaling_factor);
            mac(&identity_matrix, i, j, scaling_factor);
        }
    }

    // finally, scale each row to convert diagonal matrix to identity matrix
    for (int i = 0; i < matrix.size; i++) {
        scaling_factor = 1 / matrix.elements[getIndex(i, i, size)];
        scaleRow(&matrix, i, scaling_factor);
        scaleRow(&identity_matrix, i, scaling_factor);
    }

    // identiy_matrix now has the inverse matrix 
    //
    // Now let's multiple the two matrices and verify that the product is the identity matrix
    double element = 0;
    for (int i = 0; i < matrix.size; i++) {
        for (int j = 0; j < matrix.size; j++) {
//          std::cout << "dotproduct i=" << i << " j=" << j << std::endl;
          element = dotProduct(&matrix, i, &identity_matrix, j);
          if (i == j) {
              element -= 1;
          }
          if (fabs(element) > 1e10) {
            std::cout << element << " not zero" << std::endl;
            success = false;
          }
        }
    }

    delete matrix.elements;
    delete identity_matrix.elements;
    return success;
}


void diskSt(long long * diskStat, string fn){

    // first finding base directory for the file
    int pos=fn.find('/',2);
    string dis;
    dis=fn.substr(0, pos);
    cout<<"\nmount point from fn: "<<dis<<endl;

    // finding which device has this mount
    string na, nb, nc, nd;
    int i1;    
    ifstream mounts ("/proc/mounts", ios::in);
    if (mounts.is_open()){
        while(!mounts.eof()){
            mounts>>na>>nb>>nc>>nd>>i1>>i1;
            if (nb.compare(dis)==0)  {
                cout<<na<<"\t"<<nb<<"\t";
                if (na.substr(0,5).compare("/dev/")==0)
                    na=na.substr(5);
                cout<<na<<endl;    
                break;
            }
        }
    }

    ifstream file ("/proc/diskstats", ios::in);
    if (file.is_open()){
        while(!file.eof()){
            string name="";
            file>>i1>>i1>>name;
            // cout<<name<<"\t";
            for (int c=0;c<11;c++) {
                file>>diskStat[c]; 
                // cout<<diskStat[c]<<"\t";
            }
            // cout<<endl;    
            if (!name.compare(na)) {
                // for (int c=0;c<11;c++) cout<<diskStat[c]<<"\t"; cout<<endl;
                break;
            };
        } 
    }
    file.close();
}

void getMemoryInfo(){
    ifstream file ("/proc/self/status", ios::in);
    int i;
    if (file.is_open()){
        while(!file.eof()){
            string name="";
            file>>name;
            // cout<<name<<"\t";
            if (!name.compare("VmPeak:")) {file>>i;cout<<"VMEM="<<i<<endl;}
            if (!name.compare("VmRSS:")) {file>>i;cout<<"RSS="<<i<<endl;file.close();return;}        
        } 
    }
    file.close();
}

void getCPUusage(){
    FILE *pf;
    char data[48];
    pf=popen("uptime |awk -F'average:' '{ print $2}'  | sed s/,//g | awk '{ print $3}' ","r");
    if(!pf){
        fprintf(stderr, "Could not open pipe for output.\n");
        return;
    }
    fgets(data, 48 , pf);
    cout<<"CPUSTATUS="<<data<<endl;
}

void netSt(long long *netStat){
    system("netstat -in > nstat.txt");
    ifstream file("nstat.txt", ios::in);
    int i;netStat[0]=0;netStat[1]=0;
    if (file.is_open()){
        while(!file.eof()){
            string name="";
            file>>name;
            if (!name.compare("eth0")) {
                file>>i>>i>>netStat[0]>>i>>i>>i>>netStat[1]; 
                break;
            }
        }
    }

    system("rm nstat.txt");
    file.close();
}

int main(int argc, char **argv){
    if (argc<4) {
        cout<<"usage: read <filename> <treename> <percentage of events to read> <TTreeCache size> [file containing branches to be read]"<<endl;
        return 0;
    }
    string fn               = argv[1];
    string trname           = argv[2];
    float percentage        = atof(argv[3]);
    float TTC               = atof(argv[4]);
    string branchesToBeRead;
    if (argc==6) branchesToBeRead = argv[5]; 

    cout<<"ROOTVERSION="<<gROOT->GetSvnRevision()<<endl;
    cout<<"ROOTBRANCH='"<<gROOT->GetSvnBranch()<<"'"<<endl;

    //gEnv->SetValue("TFile.AsyncPrefetching", 1);

    TStopwatch timer;
    Double_t nb = 0; 
    bool checkHDD=true;
    long long diskS1[11];
    long long diskS2[11];
    long long netSt1[2];
    long long netSt2[2];

    string str2("dcap://");
    size_t found;
    found=fn.find(str2);
    if (found!=string::npos){
        cout << "dcap - no disk accesses available." << endl;
        checkHDD=false;
        cout<<"FILESYSTEM='dcap'"<<endl;
    }
    str2="root://";
    found=fn.find(str2);
    if (found!=string::npos){
        cout << "xrootd - no disk accesses available." << endl;
        checkHDD=false;
        cout<<"FILESYSTEM='root'"<<endl;
    }

    if (checkHDD){
        char buf[2048];
        int count = readlink(fn.c_str(), buf, sizeof(buf));
        if (count >= 0) {
            buf[count] = '\0';
            printf("%s -> %s\n", fn.c_str(), buf);
            fn = string(buf);
        }

        diskSt(diskS1, fn);
    }

    netSt(netSt1);

    TFile *f = TFile::Open(fn.c_str());
    if (!f) {
        cout<<"Problem opening the file."<<endl;
        return 1;
    }
    TTree *tree = (TTree*)f->Get(trname.c_str());


    Int_t nentries = (Int_t)tree->GetEntries();
    cout << endl;

    int* randoms = new int[nentries];

    tree->SetCacheSize((int)(TTC*1024*1024));
    
    if (branchesToBeRead.length()>0) {
        tree->SetBranchStatus("*",0);
        ifstream file (branchesToBeRead.c_str(), ios::in);
        if (file.is_open()){
            while(!file.eof()){
                string name="";
                file>>name;
                tree->SetBranchStatus(name.c_str(),1);
                tree->AddBranchToCache(name.c_str());
                    // cout<<name<<"\n";   
            } 
        }
        file.close();
    }
    else {
        tree->AddBranchToCache("*",kTRUE);
    }

    tree->StopCacheLearningPhase();
        //TTreeCache::SetLearnEntries(1);
    TTreePerfStats *ps= new TTreePerfStats("ioperf",tree);

    for (int i=0;i<nentries;i++) {
        if (gRandom->Rndm(1)<(percentage/100)) {
            randoms[i]=1;
        } else randoms[i]=0;
        if (i == 20) {
            for (int j=0; j<20; j++) {
                cout << "Inversion #" << j << endl;
                if (!matrixInvert(400)) {
                    std::cout << "error with inversion" << std::endl;
                }
            }
        }
    }


    timer.Start();
    for (Int_t ev = 0; ev < nentries; ev++) {
        if (randoms[ev]==0) continue;
        nb += tree->GetEntry(ev);
    }
    timer.Stop();
    
    nb/=1024.0/1024.0;

    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();

    tree->PrintCacheStats();
    //ps->SaveAs("perf.root");
    ps->Print();

    cout<<"TOTALSIZE="<<tree->GetTotBytes()<<endl;
    cout<<"ZIPSIZE="<<tree->GetZipBytes()<<endl;
    cout<<"FILENAME='"<<fn<<"'"<<endl;
    cout<<"EVENTS="<<nentries<<endl;
    cout<<"WALLTIME="<<rtime<<endl;
    cout<<"CPUTIME="<<ctime<<endl;
    cout<<"CACHESIZE="<<TTC*1024*1024<<endl;
    cout<<"Read from tree: " <<nb<<endl;
    cout<<"ROOTBYTESREAD=" <<f->GetBytesRead()<<endl;
    cout<<"ROOTREADS="<<f->GetReadCalls()<<endl;
    cout<<"EVENTS="<<nentries<<endl;
    getMemoryInfo();    
    f->Close();
    // delete f;
    getCPUusage();
    // cout << nentries<< " events"<< endl;
    // cout << "Compression level: "<< f->GetCompressionLevel()<<endl;
//printf("file compression factor = %f\n",f->GetCompressionFactor());
    // 
    if (checkHDD){
        diskSt(diskS2, fn);
        cout<<"HDDREADS="<<diskS2[0]-diskS1[0]<<endl;
        cout<<"reads merged:  "<<diskS2[1]-diskS1[1]<<endl;
        cout<<"reads sectors: "<<diskS2[2]-diskS1[2]<<"   in Mb: "<<(diskS2[2]-diskS1[2])/2048<<endl;
        cout<<"HDDTIME="<<diskS2[3]-diskS1[3]<<endl;
    }

    
    netSt(netSt2);
    cout<<"ETHERNETIN="<<netSt2[0]-netSt1[0]<<endl;
    cout<<"ETHERNETOUT="<<netSt2[1]-netSt1[1]<<endl;

    return 0;
};

