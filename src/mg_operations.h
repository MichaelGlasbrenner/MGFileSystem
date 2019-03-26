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
     printf("\n\ncalling mg_chmod ***********************************\n");

     mg_filesystem_data.set_file_mode(path, new_mode);

     return 0;
}


static int mg_write( const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
  printf( "\n\ncalling mg_write ********************************\n" );

  mg_filesystem_data.write_file_content(path, buffer, size, offset);

  return size; // FIXME
}


static int mg_mkdir (const char* path, mode_t new_mode)
{
    printf("\n\ncalling mg_mkdir *******************************\n");

    mg_filesystem_data.create_directory(path, new_mode);

    return 0;
}


static int mg_rename(const char* path, const char* new_name)
{
   mg_filesystem_data.debug();
   
   printf("\n\ncalling mg_rename *******************************\n");
   mg_filesystem_data.rename_file(path, new_name);
   
   mg_filesystem_data.debug();

   return 0;
}


static int mg_chown (const char* path, uid_t, gid_t)
{
    printf("\n\ncalling mg_chown *******************************\n");

    return 0;
}


static int mg_truncate (const char* path, off_t)
{
    printf("\n\ncalling mg_truncate *******************************\n");

    return 0;
}


static int mg_unlink (const char* path)
{
    printf("\n\ncalling mg_unlink *******************************\n");
   
    mg_filesystem_data.remove_file(path);

    return 0;
}


static int mg_create (const char* path, mode_t)
{
    printf("\n\ncalling mg_create *******************************\n");
   
    mg_filesystem_data.create_file(path);

    return 0;
}
