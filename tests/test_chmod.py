import test_tools
import subprocess
import os


def test_chmod():

    success = False;
    error_message = "";
        
    os.system("touch testdir/new_file"); 
    
    file_list = test_tools.get_list_of_files("testdir");

    if not test_tools.file_exists( "new_file", file_list ):
       error_message = "file was not created";
       return success, error_message;

    file_modes = [];
    expected_permission_strings = [];

    file_modes.append( "456" );      expected_permission_strings.append( "-r--r-xrw-" );
    file_modes.append( "700" );      expected_permission_strings.append( "-rwx------" );
    file_modes.append( "666" );      expected_permission_strings.append( "-rw-rw-rw-" );

    for i, mode in enumerate(file_modes):
       os.system("chmod " + str(mode) + " testdir/new_file"); 
       ls_output = test_tools.ls_output("testdir/");
       permission_string = test_tools.get_file_property( "permissions", "new_file", ls_output);
       
       if (permission_string == expected_permission_strings[i]):
           success = True;
       else:
           success = False;
           error_message = ("wrong file permissions: " + str(permission_string));
           os.system("chmod 777 testdir/new_file"); # make sure file can be deleted
           os.system("rm -rf testdir/new_file"); 
           return success, error_message;

    os.system("chmod 777 testdir/new_file"); # make sure file can be deleted
    os.system("rm testdir/new_file"); 

    return success, error_message;

