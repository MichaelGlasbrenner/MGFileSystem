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


static int mg_getattr( const char *path, struct stat *st )
{
    printf("\n\n");
    printf( "[getattr] Called\n" );
    printf( "\tAttributes of %s requested\n", path );

    // GNU's definitions of the attributes (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
    // 		st_uid: 	The user ID of the file’s owner.
    //		st_gid: 	The group ID of the file.
    //		st_atime: 	This is the last access time for the file.
    //		st_mtime: 	This is the time of the last modification to the contents of the file.
    //		st_mode: 	Specifies the mode of the file. This includes file type information (see Testing File Type) and the file permission bits (see Permission Bits).
    //		st_nlink: 	The number of hard links to the file. This count keeps track of how many directories have entries for this file. If the count is ever decremented to zero, then the file itself is discarded as soon 
    //						as no process still holds it open. Symbolic links are not counted in the total.
    //		st_size:	This specifies the size of a regular file in bytes. For files that are really devices this field isn’t usually meaningful. For symbolic links this specifies the length of the file name the link refers to.

    st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
    st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
    st->st_atime = time( NULL ); // The last access of the file/directory is right now
    st->st_mtime = time( NULL ); // The last modification of the file/directory is right now

    if ( strcmp( path, "/" ) == 0 )
    {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2; // Why two hardlinks instead of one ? The answer is here: http://unix.stackexchange.com/a/101536

        return 0;
    }
    else if ( mg_filesystem_data.file_exists(path) == true)
    {
        mg_filesystem_data.get_attributes(path, st);

        return 0; // file exists
    }
    else if ( mg_filesystem_data.directory_exists(path) == true)
    {
        mg_filesystem_data.get_attributes(path, st);

        return 0; // directory exists
    }
    else
    {
        printf("returning -ENOENT \n");
        return -ENOENT; // meaning "no such file or directory"
    }

}

static int mg_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
    printf("\n\n");
    printf( "--> Getting The List of Files of %s ***********\n", path );

    filler( buffer, ".", NULL, 0 ); // Current Directory
    filler( buffer, "..", NULL, 0 ); // Parent Directory

    if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
    {
        mg_filesystem_data.list_files_in_dir(path, buffer, filler);
    }

    return 0;
}


static int mg_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    printf("\n\n");
    printf( "--> Trying to read %s, %zu, %zu\n", path, offset, size );

    char* selectedText;

    if ( mg_filesystem_data.file_exists(path) == true)
    {
        selectedText = mg_filesystem_data.read_file_content(path);
    }
    else
    {
        printf("file does not exist ! ************************\n");
        return -ENOENT;
    }

    memcpy( buffer, selectedText + offset, size );

    return strlen( selectedText ) - offset;
}


static int mg_chmod(const char* path, mode_t new_mode)
{
    printf("\n\n");
    printf("\n\ncalling mg_chmod ***********************************\n");

    mg_filesystem_data.set_file_mode(path, new_mode);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_write( const char* path, const char* buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
    printf("\n\n");
    printf( "\n\ncalling mg_write ********************************\n" );
    printf("size, offset : %zu %zu \n", size, offset);
    printf("the_buffer : %s \n", buffer);

    mg_filesystem_data.write_file_content(path, buffer, size, offset);

    printf("op-count : %d \n", counter); counter++; 
    return size; // FIXME
}


static int mg_mkdir (const char* path, mode_t new_mode)
{
    printf("\n\n");
    printf("\n\ncalling mg_mkdir *******************************\n");

    mg_filesystem_data.create_directory(path, new_mode);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_rename(const char* path, const char* new_name)
{
    printf("\n\n");
    mg_filesystem_data.debug();

    printf("\n\ncalling mg_rename *******************************\n");
    mg_filesystem_data.rename_file(path, new_name);

    mg_filesystem_data.debug();

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_chown (const char* path, uid_t new_user, gid_t new_group)
{
    printf("\n\n");
    printf("\n\ncalling mg_chown *******************************\n");

    mg_filesystem_data.change_ownership(path, new_user, new_group);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_truncate (const char* path, off_t new_length)
{
    printf("\n\n");
    printf("\n\ncalling mg_truncate *******************************\n");

    mg_filesystem_data.truncate_file( path, new_length);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_unlink (const char* path)
{
    printf("\n\n");
    printf("\n\ncalling mg_unlink *******************************\n");

    mg_filesystem_data.remove_file(path);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_create (const char* path, mode_t new_mode, struct fuse_file_info *fi)
{
    printf("\n\n");
    printf("\n\ncalling mg_create *******************************\n");

    mg_filesystem_data.create_file(path, new_mode);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_mknod (const char* path, mode_t new_mode, dev_t dev)
{
    printf("\n\n");
    printf("\n\ncalling mg_mknod *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


static int mg_utimens (const char * path, const struct timespec tv[2])
{
    printf("\n\n");
    printf("\n\ncalling mg_utimens *******************************\n");

    mg_filesystem_data.set_access_and_modification_times(path, tv);

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_readlink (const char *, char *, size_t)
{
    printf("\n\n");
    printf("\n\ncalling mg_readlink *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_rmdir (const char *)
{
    printf("\n\n");
    printf("\n\ncalling mg_rmdir *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_symlink (const char *, const char *)
{
    printf("\n\n");
    printf("\n\ncalling mg_symlink *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_link (const char *, const char *)
{
    printf("\n\n");
    printf("\n\ncalling mg_link *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_open (const char *, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_open *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_statfs (const char *, struct statvfs *)
{
    printf("\n\n");
    printf("\n\ncalling mg_statfs *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_flush (const char *, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_flush *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_release (const char *, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_release *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_fsync (const char *, int, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_fsync *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_setxattr (const char *, const char *, const char *, size_t, int)
{
    printf("\n\n");
    printf("\n\ncalling mg_setxattr *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_getxattr (const char *, const char *, char *, size_t)
{
    printf("\n\n");
    printf("\n\ncalling mg_getxattr *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_listxattr (const char *, char *, size_t)
{
    printf("\n\n");
    printf("\n\ncalling mg_listxattr *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_removexattr (const char *, const char *)
{
    printf("\n\n");
    printf("\n\ncalling mg_removexattr *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_opendir (const char *, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_opendir *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_releasedir (const char *, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_releasedir *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_fsyncdir (const char *, int, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_fsyncdir *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


// void *(*init) (struct fuse_conn_info *conn, struct fuse_config *cfg);
// void (*destroy) (void *private_data);

int mg_access (const char *, int)
{
    printf("\n\n");
    printf("\n\ncalling mg_access *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_lock (const char *, struct fuse_file_info *, int cmd, struct flock *)
{
    printf("\n\n");
    printf("\n\ncalling mg_lock *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_bmap (const char *, size_t blocksize, uint64_t *idx)
{
    printf("\n\n");
    printf("\n\ncalling mg_bmap *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_ioctl (const char *, unsigned int cmd, void *arg, struct fuse_file_info *, unsigned int flags, void *data)
{
    printf("\n\n");
    printf("\n\ncalling mg_ioctl *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_poll (const char *, struct fuse_file_info *, struct fuse_pollhandle *ph, unsigned *reventsp)
{
    printf("\n\n");
    printf("\n\ncalling mg_poll *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_write_buf (const char *, struct fuse_bufvec *buf, off_t off, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_write_buf *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_read_buf (const char *, struct fuse_bufvec **bufp, size_t size, off_t off, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_read_buf *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_flock (const char *, struct fuse_file_info *, int op)
{
    printf("\n\n");
    printf("\n\ncalling mg_flock *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


int mg_fallocate (const char *, int, off_t, off_t, struct fuse_file_info *)
{
    printf("\n\n");
    printf("\n\ncalling mg_fallocate *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}


ssize_t mg_copy_file_range (const char *path_in, struct fuse_file_info *fi_in,
        off_t offset_in, const char *path_out, struct fuse_file_info *fi_out, off_t offset_out, size_t size, int flags)
{
    printf("\n\n");
    printf("\n\ncalling mg_copy_file_range *******************************\n");

    printf("op-count : %d \n", counter); counter++; 
    return 0;
}



