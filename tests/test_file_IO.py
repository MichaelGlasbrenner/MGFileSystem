import test_tools
import subprocess
import os


def test_file_IO():

    print("running test_file_IO");
    success = False;
    error_message = "";
    file_list = test_tools.get_list_of_files("testdir");
	
    if test_tools.file_exists( "new_file", file_list ):
       success = False;
       error_message = "file already existed";
       with open('temp_output', "w") as outfile:
            subprocess.call(["rm","testdir/" + "new_file"], stdout=outfile)
       return success, error_message;

    with open('temp_output', "w") as outfile:
        os.system("echo -n \"this is the content\" > testdir/new_file"); # -n -> do not append \n
	file_list = test_tools.get_list_of_files("testdir");

	if not test_tools.file_exists( "new_file", file_list ):
           error_message = "file was not created";
           return success, error_message;

        file_content = test_tools.get_file_content("testdir/new_file");

        if (len(file_content) == 1 and (file_content[0] == "this is the content")):
           success = True;
        else:
           content_string = "";
           for line in file_content:
               content_string += str(line);
           error_message = ("wrong file content: " + content_string);
           subprocess.call(["rm","testdir/" + "new_file"], stdout=outfile)
           return success, error_message;

        subprocess.call(["rm","testdir/" + "new_file"], stdout=outfile)

    return success, error_message;

