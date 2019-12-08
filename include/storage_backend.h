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

       virtual void list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler) = 0;
       virtual char* read_file_content(const char *path) = 0;
       virtual void get_attributes(const char* path, struct stat* st) = 0;
       virtual bool file_exists(const char* path) = 0;
       virtual bool directory_exists(const char* path) = 0;

       virtual void set_file_mode(const char* path, mode_t new_mode) = 0;
       virtual void write_file_content(const char* path, const char* data, size_t size, off_t offset) = 0;
       virtual void rename_file(const char* path, const char* new_path) = 0;
       virtual void create_directory(const char* path, mode_t new_mode) = 0;
       virtual void remove_file(const char* path) = 0;
       virtual void create_file(const char* path, mode_t new_mode) = 0;
       virtual void set_access_and_modification_times(const char* path, const struct timespec tv[2]) = 0;
       virtual void truncate_file(const char* path, off_t new_length) = 0;
       virtual void change_ownership(const char* path, uid_t new_user, gid_t new_group) = 0;

       virtual void debug() = 0;
};


#endif
