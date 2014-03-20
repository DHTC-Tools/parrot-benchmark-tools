#!/usr/bin/env python
import sys, random

if len(sys.argv) != 3:
    sys.stderr.write("Must provide file with list of filenames and number of files to pick\n")
    sys.exit(1)
file_list = open(sys.argv[1])
file_array = []
for filepath in file_list:
    file_array.append(filepath.strip())
try:
   choices = int(sys.argv[2])
except:
    sys.stderr.write("Can't get the number of files to pick\n")
    sys.exit(1)
for i in range(choices):
    sys.stdout.write("%s\n" % random.choice(file_array))
