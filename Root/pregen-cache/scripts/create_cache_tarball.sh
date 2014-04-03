#!/bin/bash
if  [ -e parrot_cvmfs_cache.tar.gz ]; 
then
  echo "parrot_cmvfs_cache.tar.gz exists, exiting."
  exit 1
fi
parrot_dir=`./init_atlas.py | grep 'Parrot_dir' | cut -f 2 -d= `
present_dir=$PWD
cd $parrot_dir/parrot_cache
tar czf parrot_cvmfs_cache.tar.gz cvmfs
cp parrot_cvmfs_cache.tar.gz $present_dir
cd $present_dir


