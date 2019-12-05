#!/usr/bin/python

import sys
import subprocess

import test_touch
import test_rm

print("running tests\n");
test_cases = [];
test_cases.append( test_touch.test_touch );
test_cases.append( test_rm.test_rm );

failed_test_cases = [];
error_messages = [];

for test_case in test_cases:
    successful, error_message = test_case();
    if(successful == False):
       print("FAILED\n");
       failed_test_cases.append(test_case);
       error_messages.append(error_message);
    else:
       print("PASSED\n");

print(str(len(failed_test_cases)) + " out of " + str(len(test_cases)) + " tests failed");
for index, failed_test in enumerate(failed_test_cases):
    print("test " + str(failed_test) + " failed with error : " + str(error_messages[index]));

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
