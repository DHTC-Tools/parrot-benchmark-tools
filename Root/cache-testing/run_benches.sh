#!/bin/sh
for (( i = 1; i < 9; i++)) 
do
  ./run_shared.sh run$i
  ./run_nonshared.sh run$i
done
