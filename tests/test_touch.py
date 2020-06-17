import test_tools
import subprocess


def test_touch_new_file():

    success = False;
    error_message = "";
    file_list = test_tools.get_list_of_files("testdir");

    if test_tools.file_exists( "new_file", file_list ):
       success = False;
       error_message = "file already existed";
       subprocess.call(["rm","testdir/new_file"])
       return success, error_message;

    with open('temp_output', "w") as outfile:
        subprocess.call(["touch","testdir/new_file"])

        file_list = test_tools.get_list_of_files("testdir");
        ls_output = test_tools.ls_output("testdir");

        if test_tools.file_exists( "new_file", file_list ):
           success = True;
        else:
           error_message = "file was not created";
           return success, error_message;

        if not (test_tools.get_file_property("permissions", "new_file", ls_output) == "-rw-r--r--"):
           success = False;
           error_message = "wrong file permissions";
           subprocess.call(["rm","testdir/new_file"])
           return success, error_message;

        test_tools.check_creation_time( "new_file", ls_output);
        subprocess.call(["rm","testdir/new_file"])

    return success, error_message;

