#!/bin/bash
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
localSetupROOT --skipConfirm
inspector root://uct3-xrd.mwt2.org///atlas/uct3/data/users/lincolnb/ZZincllNp0.ntuple.root | head -n 10
