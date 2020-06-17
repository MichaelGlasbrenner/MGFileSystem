import test_tools
import subprocess


def test_rmdir_existing_dir():

    success = False;
    error_message = "";

    with open('temp_output', "w") as outfile:
        subprocess.call(["mkdir","testdir/" + "new_dir"], stdout=outfile)

    dir_list = test_tools.get_list_of_directories("testdir");

    if not test_tools.directory_exists( "new_dir", dir_list ):
       success = False;
       error_message = "directory creation failed";
       return success, error_message;

    with open('temp_output', "w") as outfile:
        subprocess.call(["rmdir","testdir/" + "new_dir"], stdout=outfile)

        dir_list = test_tools.get_list_of_directories("testdir");

        if test_tools.directory_exists( "new_dir", dir_list ):
           error_message = "directory deletion failed";
           return success, error_message;
        else:
           success = True;

    return success, error_message;

