#!/bin/bash
/bin/hostname
export TMP_DIR="$OSG_WN_TMP"
work_dir=`mktemp -d -t root.XXXXXXXXX`
if [ ! -d $work_dir ]; then
  echo "Can't create work dir"
  exit 1
fi
start_dir=`pwd`
cd $work_dir
start_time=`date +"%s.%N"`
wget http://stash.osgconnect.net/+sthapa/cvmfs-root.tar.gz
download_time=`date +"%s.%N"`
export PATH=$PATH:$work_dir/cvmfs-root/
tar xvzf cvmfs-root.tar.gz
cd cvmfs-root
exec_time=`date +"%s.%N"`
ls
time $work_dir/cvmfs-root/run-read.sh 
end_time=`date +"%s.%N"`
rm -fr $work_dir
