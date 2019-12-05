import test_tools
import subprocess


def test_rm():

    print("running test_rm");
    success = False;
    error_message = "";
    
    with open('temp_output', "w") as outfile:
        subprocess.call(["touch","testdir/new_file"], stdout=outfile)
    ls_output = test_tools.ls_output("testdir");
	
    if not test_tools.file_exists( "new_file", ls_output ):
       success = False;
       error_message = "file creation failed";
       return success, error_message;

    with open('temp_output', "w") as outfile:
        subprocess.call(["rm","testdir/new_file"], stdout=outfile)
        ls_output = test_tools.ls_output("testdir");
	if test_tools.file_exists( "new_file", ls_output ):
           success = False;
           error_message = "file was not deleted";
        else:
           success = True;

    return success, error_message;
