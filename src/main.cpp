#define FUSE_USE_VERSION 26

#define _FILE_OFFSET_BITS  64 // added by me

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "mg_operations.h"

extern filesystem_data mg_filesystem_data;


struct mg_fuse_operations : fuse_operations
{
    mg_fuse_operations ()
    {
        getattr     = mg_getattr;
        readdir     = mg_readdir;
        read        = mg_read;
        write       = mg_write;
        chmod       = mg_chmod;
        chown       = mg_chown;
        mkdir       = mg_mkdir;
        rename      = mg_rename;
        truncate    = mg_truncate;
        unlink      = mg_unlink;
        create      = mg_create;
        mknod       = mg_mknod;
        utimens     = mg_utimens;
        readlink    = mg_readlink;
        rmdir       = mg_rmdir;
        symlink     = mg_symlink;
        open        = mg_open;
        flush       = mg_flush;
        release     = mg_release;
        fsync       = mg_fsync;
        setxattr    = mg_setxattr;
        getxattr    = mg_getxattr;
        listxattr   = mg_listxattr;
        removexattr = mg_removexattr;
        opendir     = mg_opendir;
        releasedir  = mg_releasedir;
        fsyncdir    = mg_fsyncdir;
    }
};

static struct mg_fuse_operations mg_oper;


int main( int argc, char *argv[] )
{
	return fuse_main( argc, argv, &mg_oper, NULL );
}

