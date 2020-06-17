#!/usr/bin/python

import sys
import subprocess

import list_of_test_cases

storage_backends=["ssh"];
#storage_backends=["none", "ram", "ssh"];

for backend in storage_backends:

   print("running tests with backend = " + str(backend));
   
   #if backend=="ram":
   #   subprocess.call(["fusermount", "-u", "./testdir"]);
   #   fs_process = subprocess.Popen(["../bin/MGfilesystem", "--mount-point=./testdir", "--backend=ram"]);
   #elif backend=="ssh":
   #   fs_process = subprocess.Popen(["../bin/MGfilesystem", "--mount-point=./testdir", "--backend=ssh", "--auth=pw", "--ssh=root@192.168.122.252:/root/testdir"]);

   failed_test_cases = [];
   error_messages = [];
   for test_case in list_of_test_cases.test_cases:
       successful, error_message = test_case();
       if(successful == False):
          print(test_case.__name__ + ": FAILED");
          failed_test_cases.append(test_case);
          error_messages.append(error_message);
       else:
          print(test_case.__name__ + ": PASSED");

   #if not (backend== "none"):
   #   subprocess.call(["fusermount", "-u", "./testdir"]);
   #   fs_process.terminate();

   print(str(len(failed_test_cases)) + " out of " + str(len(list_of_test_cases.test_cases)) + " tests failed");
   for index, failed_test in enumerate(failed_test_cases):
       print("test \"" + failed_test.__name__ + "\" failed with error : " + str(error_messages[index]));

