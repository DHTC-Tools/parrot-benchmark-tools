#!/bin/bash
start_time=`date +"%s"`
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
asetup AtlasProduction,17.2.11.14,slc5,64
end_time=`date +"%s"`
elapsed=$(( $end_time - $start_time ))
echo "Time elapsed: $elapsed"
