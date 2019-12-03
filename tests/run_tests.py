#!/usr/bin/python

import sys
import subprocess

import test_touch
import test_rm

print("running tests");
test_cases = [];
test_cases.append( test_touch.test_touch );
test_cases.append( test_rm.test_rm );

failed_test_cases = [];

for test_case in test_cases:
    successful = test_case();
    if(successful == False):
       failed_test_cases.append(test_case);

print(str(len(failed_test_cases)) + " out of " + str(len(test_cases)) + "failed");
#for failed_test

#with open('temp_output', "w") as outfile:
   #subprocess.call(["ls"], stdout=outfile)
   #subprocess.call(["mkdir", "testdir"], stdout=outfile)
   #subprocess.call(["../bin/MGfilesystem", "-d", "testdir"], stdout=outfile)
#   subprocess.call(["ls","testdir"], stdout=outfile)

# create file
#   subprocess.call(["touch","testdir/new_file_1"], stdout=outfile)
#   subprocess.call(["ls","testdir"], stdout=outfile)


#lines=[];
#input_file = open('temp_output', "r");
#for line in input_file:
#    lines.append( line );
#input_file.close();
#
#for line in lines:
#    print line;
