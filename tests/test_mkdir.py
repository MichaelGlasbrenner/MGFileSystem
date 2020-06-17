import test_tools
import subprocess


def test_mkdir_new_dir():

    success = False;
    error_message = "";
    dir_list = test_tools.get_list_of_directories("testdir");

    if test_tools.directory_exists( "new_dir", dir_list ):
       success = False;
       error_message = "directory already existed";
       with open('temp_output', "w") as outfile:
            subprocess.call(["rmdir","testdir/" + "new_dir"], stdout=outfile)
       return success, error_message;

    with open('temp_output', "w") as outfile:
        subprocess.call(["mkdir","testdir/" + "new_dir"], stdout=outfile)

        dir_list = test_tools.get_list_of_directories("testdir");

        if test_tools.directory_exists( "new_dir", dir_list ):
           success = True;
        else:
           error_message = "directory was not created";
           return success, error_message;

        subprocess.call(["rmdir","testdir/" + "new_dir"], stdout=outfile)

    return success, error_message;

