
import test_touch
import test_rm
import test_mkdir

test_cases = [];
test_cases.append( test_touch.test_touch_new_file );
test_cases.append( test_rm.test_rm );
test_cases.append( test_mkdir.test_mkdir_new_dir() );
