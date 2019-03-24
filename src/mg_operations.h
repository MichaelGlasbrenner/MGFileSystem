#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "filesystem_data.h"

extern filesystem_data mg_filesystem_data;


static int mg_chmod(const char* path, mode_t new_mode)
{
     printf("calling mg_chmod");

     mg_filesystem_data.set_file_mode(path, new_mode);

     return 0;
}


static int mg_write( const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
  printf( "--> Trying to write %s, %zu, %zu\n", path, offset, size );

  mg_filesystem_data.write_file_content(path, buffer, size, offset);

  return size; // FIXME
}

