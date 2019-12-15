#ifndef _SSH_BACKEND_
#define _SSH_BACKEND_

#include <fuse.h>
#include "storage_backend.h"
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <string>

/*
 *  ssh-based remote file-system
 */


class ssh_backend : public storage_backend
{
    public:
       ssh_backend();
       ~ssh_backend();

       bool init(const std::string& mount_dir, const std::string& ip_addr, const std::string& remote_user,
                 const std::string& authentication_method);
       void clean_up();
       void list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler);
       char* read_file_content(const char *path, size_t offset, size_t read_size);
       void get_attributes(const char* path, struct stat* st);
       bool file_exists(const char* path);
       bool directory_exists(const char* path);

       void set_file_mode(const char* path, mode_t new_mode);
       void write_file_content(const char* path, const char* data, size_t size, off_t offset) ;
       void rename_file(const char* path, const char* new_path);
       void create_directory(const char* path, mode_t new_mode);
       void remove_directory(const char* path);
       void remove_file(const char* path);
       void create_file(const char* path, mode_t new_mode);
       void set_access_and_modification_times(const char* path, const struct timespec tv[2]);
       void truncate_file(const char* path, off_t new_length);
       void change_ownership(const char* path, uid_t new_user, gid_t new_group);

       void debug();

    private:
       ssh_session _session;
       sftp_session _sftp_session;
       std::string _mount_dir;
       
    private:
       void establish_ssh_connection(const std::string& ip_addr, const std::string& remote_user, 
                                     const std::string& authentication_method);
       int password_authentication(ssh_session session);
       int public_key_authentication(ssh_session session);
       void remote_command(ssh_session session, const char* command);
       int send_remote_command(ssh_session session, const char* command);
       int init_sftp_session();
       void close_sftp_session();
       std::string get_full_path(const char* path);
};


#endif
