import test_tools
import subprocess


def test_mkdir_new_dir(dir_name):

    print("running test_mkdir");
    success = False;
    error_message = "";
    dir_list = test_tools.get_list_of_directories("testdir");
	
    if test_tools.directory_exists( dir_name, dir_list ):
       success = False;
       error_message = "directory already existed";
       with open('temp_output', "w") as outfile:
            subprocess.call(["rmdir","testdir/" + dir_name], stdout=outfile)
       return success, error_message;

    with open('temp_output', "w") as outfile:
        subprocess.call(["mkdir","testdir/" + dir_name], stdout=outfile)
	dir_list = test_tools.get_list_of_directories("testdir");

	if test_tools.directory_exists( dir_name, dir_list ):
           success = True;
        else:
           error_message = "directory was not created";
           return success, error_message;

        subprocess.call(["rmdir","testdir/" + dir_name], stdout=outfile)

    return success, error_message;

