#!/usr/bin/python

import sys
import subprocess

import list_of_test_cases

print("running tests\n");

failed_test_cases = [];
error_messages = [];

for test_case in list_of_test_cases.test_cases:
    successful, error_message = test_case();
    if(successful == False):
       print("FAILED\n");
       failed_test_cases.append(test_case);
       error_messages.append(error_message);
    else:
       print("PASSED\n");

print(str(len(failed_test_cases)) + " out of " + str(len(list_of_test_cases.test_cases)) + " tests failed");
for index, failed_test in enumerate(failed_test_cases):
    print("test \"" + failed_test.__name__ + "\" failed with error : " + str(error_messages[index]));

