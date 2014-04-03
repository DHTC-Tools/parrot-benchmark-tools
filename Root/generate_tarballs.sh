#!/bin/bash
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
localSetupROOT --skipConfirm
present_dir=$PWD
echo "Compiling multiple-read code"
cd multiple-read/root-code
make
cp readDirect-multiple ../../parrot-root
cp readDirect-multiple ../../cvmfs-root
make clean
echo "Compiling singe-read code"
cd $present_dir
cd single-read/root-code
make
cp readDirect-single ../../parrot-root
cp readDirect-single ../../cvmfs-root
make clean
cd $present_dir
echo "Compiling cpuburn code"
cd single-read-cpuburn/root-code
make
cp readDirect-cpuburn ../../parrot-root
cp readDirect-cpuburn ../../cvmfs-root
make clean
cd $present_dir
tar cvzf parrot-root.tar.gz parrot-root
tar cvzf cvmfs-root.tar.gz cvmfs-root
rm parrot-root/readDirect*
rm cvmfs-root/readDirect*
