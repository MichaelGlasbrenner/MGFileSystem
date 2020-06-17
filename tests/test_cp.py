import test_tools
import subprocess
import os


def test_cp():

    success = False;
    error_message = "";
        
    os.system("echo -n \"this is the content\" > testdir/new_file"); # -n -> do not append \n
    os.system("cp testdir/new_file testdir/new_file2"); 
    
    file_list = test_tools.get_list_of_files("testdir");

    if not test_tools.file_exists( "new_file2", file_list ):
       error_message = "file was not created";
       return success, error_message;

    with open('temp_output', "w") as outfile:
        file_content = test_tools.get_file_content("testdir/new_file2");

        os.system("rm testdir/new_file"); 
        os.system("rm testdir/new_file2"); 

        if (len(file_content) == 1 and (file_content[0] == "this is the content")):
           success = True;
        else:
           content_string = "";
           for line in file_content:
               content_string += str(line);
           error_message = ("wrong file content: " + content_string);
           return success, error_message;

    return success, error_message;

