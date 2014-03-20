#include <stdlib.h>
#include <math.h>

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


int main(int argc, char **argv){
    if (!matrixInvert(400)) {
      std::cout << "error with inversion" << std::endl;
    }
    return 0;
};

