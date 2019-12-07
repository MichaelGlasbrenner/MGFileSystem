
import test_touch
import test_rm
import test_mkdir
import test_rmdir
import test_file_IO

test_cases = [];
test_cases.append( test_touch.test_touch_new_file );
test_cases.append( test_rm.test_rm );
test_cases.append( test_mkdir.test_mkdir_new_dir );
test_cases.append( test_rmdir.test_rmdir_existing_dir );
test_cases.append( test_file_IO.test_file_IO );
