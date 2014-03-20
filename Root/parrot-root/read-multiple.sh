#!/bin/bash
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
localSetupROOT --skipConfirm
filelist=''
for filename in `./parrot-root/get_file_list.py ./parrot-root/filelist 5`;
do
  filelist="$filelist root://faxbox.usatlas.org//user/sthapa/$filename"
done
./parrot-root/readDirect-multiple $filelist physics 10 30
