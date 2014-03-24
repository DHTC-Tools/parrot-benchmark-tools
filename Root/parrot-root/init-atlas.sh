#!/bin/bash
start_time=`date +"%s"`
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
localSetupROOT --skipConfirm
end_time=`date +"%s"`
elapsed=$(( $end_time - $start_time ))
echo "Time elapsed: $elapsed"
