import test_tools

def test_touch():
    print("running test_touch");
    ls_output = test_tools.ls_output("testdir");
    print ls_output;
    print test_tools.file_exists( "mg_file", ls_output );
    print test_tools.file_exists( "new_file_1", ls_output );
    return False;
