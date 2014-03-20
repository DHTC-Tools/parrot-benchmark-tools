#!/bin/bash
ROOTFILE='root://faxbox.usatlas.org//user/sthapa/FDR/user.flegger.MWT2.data12_8TeV.00212172.physics_Muons.merge.NTUP_SMWZ.f479_m1228_p1067_p1141_tid01007411_00/NTUP_SMWZ.01007411._000072.MWT2.root.1'
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
localSetupROOT --skipConfirm
./readDirect-cpuburn  $ROOTFILE physics 10 30
