import test_tools
import subprocess

def test_touch():
    print("running test_touch");
    ls_output = test_tools.ls_output("testdir");
    print ls_output;
    print test_tools.file_exists( "new_file_2", ls_output );
    with open('temp_output', "w") as outfile:
        subprocess.call(["touch","testdir/new_file_2"], stdout=outfile)
    ls_output = test_tools.ls_output("testdir");
    print ls_output;
    print test_tools.file_exists( "new_file_2", ls_output );
    return False;
