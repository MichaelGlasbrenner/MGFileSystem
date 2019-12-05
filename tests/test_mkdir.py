import test_tools
import subprocess


def test_mkdir_new_dir():

    print("running test_mkdir");
    success = False;
    error_message = "";
    ls_output = test_tools.ls_output("testdir");
	
    if not test_tools.file_exists( "new_file", ls_output ):
       success = False;
       error_message = "file already existed";
       with open('temp_output', "w") as outfile:
            subprocess.call(["rm","testdir/new_file"], stdout=outfile)
       return success, error_message;

    with open('temp_output', "w") as outfile:
        subprocess.call(["touch","testdir/new_file"], stdout=outfile)
        ls_output = test_tools.ls_output("testdir");
	if test_tools.file_exists( "new_file", ls_output ):
           success = True;
        else:
           error_message = "file was not created";
           return success, error_message;

        if not (test_tools.get_file_property("permissions", "new_file", ls_output) == "-rw-r--r--"):
           success = False;
           error_message = "wrong file permissions";
	   return success, error_message;

        if not (test_tools.get_file_property("permissions", "new_file", ls_output) == "-rw-r--r--"):
           success = False;
           error_message = "wrong file permissions";
	   return success, error_message;

        test_tools.check_creation_time( "testdir/new_file", ls_output);
        subprocess.call(["rm","testdir/new_file"], stdout=outfile)

    return success, error_message;

