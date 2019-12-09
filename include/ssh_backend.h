#ifndef _SSH_BACKEND_
#define _SSH_BACKEND_

#include <fuse.h>
#include "storage_backend.h"
#include <libssh/libssh.h>
#include <libssh/sftp.h>

/*
 *  ssh-based remote file-system
 */


class ssh_backend : public storage_backend
{
    public:
       ssh_backend();
       ~ssh_backend();

       void list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler);
       char* read_file_content(const char *path);
       void get_attributes(const char* path, struct stat* st);
       bool file_exists(const char* path);
       bool directory_exists(const char* path);

       void set_file_mode(const char* path, mode_t new_mode);
       void write_file_content(const char* path, const char* data, size_t size, off_t offset) ;
       void rename_file(const char* path, const char* new_path);
       void create_directory(const char* path, mode_t new_mode);
       void remove_file(const char* path);
       void create_file(const char* path, mode_t new_mode);
       void set_access_and_modification_times(const char* path, const struct timespec tv[2]);
       void truncate_file(const char* path, off_t new_length);
       void change_ownership(const char* path, uid_t new_user, gid_t new_group);

       void debug();

    private:
       ssh_session _session;
       sftp_session _sftp_session;
       
    private:
       void establish_ssh_connection();
       int password_authentication(ssh_session session);
       int public_key_authentication(ssh_session session);
       void remote_command(ssh_session session, const char* command);
       int send_remote_command(ssh_session session, const char* command);
       int init_sftp_session();
       void close_sftp_session();
};


#endif
