#!/bin/bash
cur_dir=`pwd`
if [ -e shared/$1 ]; 
then
  echo "Log directory presenting, exiting"
  exit 1
fi
mkdir shared/$1
cd shared/$1
if [ -e /tmp/root-shared ]
then
  echo "Test already running, exiting..."
  exit 1
fi

for (( i = 1; i < 17; i++)) 
do
 echo "Starting process $i"
 ../multiple_read_shared.py > run_shared.out.$i 2>&1 &
done
wait
rm -fr /tmp/root-shared
