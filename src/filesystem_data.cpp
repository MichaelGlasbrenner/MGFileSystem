#include "filesystem_data.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

filesystem_data::filesystem_data()
{
   simple_file mg_file_1;

   mg_file_1._path = "/mg_file_1";
   mg_file_1._name = "mg_file_1";
   mg_file_1._content = "this is the content";

   _the_files.push_back(mg_file_1);
}


void filesystem_data::list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler)
{
   for(int i=0; i < _the_files.size(); ++i)
   {
      filler(buffer, _the_files[i]._name.c_str(), NULL, 0 );
   }
}
