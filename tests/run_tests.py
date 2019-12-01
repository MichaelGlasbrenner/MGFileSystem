#!/usr/bin/python

import sys
import subprocess

import test_touch

print("running tests");
test_touch.test_touch();

with open('temp_output', "w") as outfile:
   #subprocess.call(["ls"], stdout=outfile)
   #subprocess.call(["mkdir", "testdir"], stdout=outfile)
   #subprocess.call(["../bin/MGfilesystem", "-d", "testdir"], stdout=outfile)
   subprocess.call(["ls","testdir"], stdout=outfile)

# create file
   subprocess.call(["touch","testdir/new_file_1"], stdout=outfile)
   subprocess.call(["ls","testdir"], stdout=outfile)


lines=[];
input_file = open('temp_output', "r");
for line in input_file:
    lines.append( line );
input_file.close();

for line in lines:
    print line;
