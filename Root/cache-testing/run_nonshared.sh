#!/bin/bash
cur_dir=`pwd`
if [ -e nonshared/$1 ]; 
then
  echo "Log directory presenting, exiting"
  exit 1
fi
mkdir nonshared/$1
cd nonshared/$1
if [ -e /tmp/root-nonshared ]
then
  echo "Test already running, exiting..."
  exit 1
fi

for (( i = 1; i < 17; i++)) 
do
 echo "Starting process $i"
 ../multiple_read_nonshared.py > run_nonshared.out.$i 2>&1 &
done
wait
rm -fr /tmp/root-nonshared
