#!/bin/bash
present_dir=$PWD
cd multiple-read/root-code
make
cp readDirect-multiple ../../root-parrot
cp readDirect-multiple ../../root-cvmfs
make clean
cd $present_dir
cd single-read/root-code
make
cp readDirect-single ../../root-parrot
cp readDirect-single ../../root-cvmfs
make clean
cd $present_dir
cd single-read-cpuburn
cp readDirect-cpuburn ../../root-parrot
cp readDirect-cpuburn ../../root-cvmfs
make clean
cd $present_dir
tar cvzf parrot-root.tar.gz parrot-root
tar cvzf parrot-cvmfs.tar.gz parrot-cvmfs

