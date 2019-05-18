#ifndef _STORAGE_BACKEND_
#define _STORAGE_BACKEND_

#include <fuse.h>

/*
 *  Abstract base class for different storage backends
 *  which can be used for the filesystem
 */


class storage_backend
{
    public:
       storage_backend();

       void list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler) = 0;
       char* read_file_content(const char *path) = 0;
       void get_attributes(const char* path, struct stat* st) = 0;
       bool file_exists(const char* path) = 0;

       void set_file_mode(const char* path, mode_t new_mode) = 0;
       void write_file_content(const char* path, const char* data, size_t size, off_t offset) = 0;
       void rename_file(const char* path, const char* new_path) = 0;
       void create_directory(const char* path, mode_t new_mode) = 0;
       void remove_file(const char* path) = 0;
       void create_file(const char* path, mode_t new_mode) = 0;
       void set_access_and_modification_times(const char* path, const struct timespec tv[2]) = 0;
       void truncate_file(const char* path, off_t new_length) = 0;
       void change_ownership(const char* path, uid_t new_user, gid_t new_group) = 0;

       void debug() = 0;
};


#endif
