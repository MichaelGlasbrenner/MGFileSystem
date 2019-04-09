#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "filesystem_data.h"

extern filesystem_data mg_filesystem_data;

int counter = 0;

static int mg_chmod(const char* path, mode_t new_mode)
{
    printf("\n\ncalling mg_chmod ***********************************\n");

    mg_filesystem_data.set_file_mode(path, new_mode);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_write( const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    printf( "\n\ncalling mg_write ********************************\n" );

    mg_filesystem_data.write_file_content(path, buffer, size, offset);

    printf("op-count : %d \n", counter); counter++; 
    return size; // FIXME
}


static int mg_mkdir (const char* path, mode_t new_mode)
{
    printf("\n\ncalling mg_mkdir *******************************\n");

    mg_filesystem_data.create_directory(path, new_mode);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_rename(const char* path, const char* new_name)
{
    mg_filesystem_data.debug();

    printf("\n\ncalling mg_rename *******************************\n");
    mg_filesystem_data.rename_file(path, new_name);

    mg_filesystem_data.debug();

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_chown (const char* path, uid_t, gid_t)
{
    printf("\n\ncalling mg_chown *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_truncate (const char* path, off_t)
{
    printf("\n\ncalling mg_truncate *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_unlink (const char* path)
{
    printf("\n\ncalling mg_unlink *******************************\n");

    mg_filesystem_data.remove_file(path);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_create (const char* path, mode_t new_mode, struct fuse_file_info *fi)
{
    printf("\n\ncalling mg_create *******************************\n");

    mg_filesystem_data.create_file(path, new_mode);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_mknod (const char* path, mode_t new_mode, dev_t dev)
{
    printf("\n\ncalling mg_mknod *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_utimens (const char *, const struct timespec tv[2])
{
    printf("\n\ncalling mg_utimens *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_readlink (const char *, char *, size_t)
{
    printf("\n\ncalling mg_readlink *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_rmdir (const char *)
{
    printf("\n\ncalling mg_rmdir *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_symlink (const char *, const char *)
{
    printf("\n\ncalling mg_symlink *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_link (const char *, const char *)
{
    printf("\n\ncalling mg_link *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_open (const char *, struct fuse_file_info *)
{
    printf("\n\ncalling mg_open *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_statfs (const char *, struct statvfs *)
{
    printf("\n\ncalling mg_statfs *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_flush (const char *, struct fuse_file_info *)
{
    printf("\n\ncalling mg_flush *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_release (const char *, struct fuse_file_info *)
{
    printf("\n\ncalling mg_release *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_fsync (const char *, int, struct fuse_file_info *)
{
    printf("\n\ncalling mg_fsync *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_setxattr (const char *, const char *, const char *, size_t, int)
{
    printf("\n\ncalling mg_setxattr *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_getxattr (const char *, const char *, char *, size_t)
{
    printf("\n\ncalling mg_getxattr *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_listxattr (const char *, char *, size_t)
{
    printf("\n\ncalling mg_listxattr *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_removexattr (const char *, const char *)
{
    printf("\n\ncalling mg_removexattr *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_opendir (const char *, struct fuse_file_info *)
{
    printf("\n\ncalling mg_opendir *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_releasedir (const char *, struct fuse_file_info *)
{
    printf("\n\ncalling mg_releasedir *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_fsyncdir (const char *, int, struct fuse_file_info *)
{
    printf("\n\ncalling mg_fsyncdir *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


// void *(*init) (struct fuse_conn_info *conn, struct fuse_config *cfg);
// void (*destroy) (void *private_data);

int mg_access (const char *, int)
{
    printf("\n\ncalling mg_access *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_lock (const char *, struct fuse_file_info *, int cmd, struct flock *)
{
    printf("\n\ncalling mg_lock *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_bmap (const char *, size_t blocksize, uint64_t *idx)
{
    printf("\n\ncalling mg_bmap *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_ioctl (const char *, unsigned int cmd, void *arg, struct fuse_file_info *, unsigned int flags, void *data)
{
    printf("\n\ncalling mg_ioctl *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_poll (const char *, struct fuse_file_info *, struct fuse_pollhandle *ph, unsigned *reventsp)
{
    printf("\n\ncalling mg_poll *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_write_buf (const char *, struct fuse_bufvec *buf, off_t off, struct fuse_file_info *)
{
    printf("\n\ncalling mg_write_buf *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_read_buf (const char *, struct fuse_bufvec **bufp, size_t size, off_t off, struct fuse_file_info *)
{
    printf("\n\ncalling mg_read_buf *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_flock (const char *, struct fuse_file_info *, int op)
{
    printf("\n\ncalling mg_flock *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_fallocate (const char *, int, off_t, off_t, struct fuse_file_info *)
{
    printf("\n\ncalling mg_fallocate *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


ssize_t mg_copy_file_range (const char *path_in, struct fuse_file_info *fi_in,
        off_t offset_in, const char *path_out, struct fuse_file_info *fi_out, off_t offset_out, size_t size, int flags)
{
    printf("\n\ncalling mg_copy_file_range *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}



