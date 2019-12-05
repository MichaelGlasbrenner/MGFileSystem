import test_tools
import subprocess


def test_touch():

    print("running test_touch");
    success = False;
    error_message = "";
    ls_output = test_tools.ls_output("testdir");
	
    if test_tools.file_exists( "new_file", ls_output ):
       success = False;
       error_message = "file already existed";
       with open('temp_output', "w") as outfile:
            subprocess.call(["rm","testdir/new_file"], stdout=outfile)
       return success, error_message;

    with open('temp_output', "w") as outfile:
        subprocess.call(["touch","testdir/new_file"], stdout=outfile)
        ls_output = test_tools.ls_output("testdir");
	if test_tools.file_exists( "new_file", ls_output ):
           print(test_tools.get_file_property( "permissions", "new_file", ls_output));
           print(test_tools.get_file_property( "hard_link_number", "new_file", ls_output));
           print(test_tools.get_file_property( "owner", "new_file", ls_output));
           print(test_tools.get_file_property( "group", "new_file", ls_output));
           print(test_tools.get_file_property( "size", "new_file", ls_output));
           print(test_tools.get_file_property( "timestamp", "new_file", ls_output));
           success = True;
        else:
           error_message = "file was not created";
           return success, error_message;

        if not (test_tools.get_file_property("permissions", "new_file", ls_output) == "-rw-r--r--"):
           success = False;
           error_message = "wrong file permissions";
	   return success, error_message;

        subprocess.call(["rm","testdir/new_file"], stdout=outfile)

    return success, error_message;
