#include "ssh_backend.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <assert.h>


ssh_backend mg_filesystem_data;


ssh_backend::ssh_backend()
{
   _session = NULL;
   _sftp_session = NULL;
}


bool ssh_backend::init(const std::string& mount_dir, const std::string& ip_addr, const std::string& remote_user,
                       const std::string& authentication_method)
{
   _mount_dir = mount_dir; 

   this->establish_ssh_connection(ip_addr, remote_user, authentication_method);

   return true; // FIXME
}


void ssh_backend::clean_up()
{
    if(_sftp_session != NULL)
    {
        this->close_sftp_session();
    }
 
    if(_session != NULL)
    {
        ssh_disconnect( _session );
        ssh_free(_session);
    }
}


ssh_backend::~ssh_backend()
{
}


void ssh_backend::establish_ssh_connection(const std::string& ip_addr, const std::string& remote_user, 
                                           const std::string& authentication_method)
{
   _session = ssh_new();

   if(_session == NULL)
   {
       exit(-1);
   }

   ssh_options_set( _session, SSH_OPTIONS_HOST, ip_addr.c_str());
   ssh_options_set( _session, SSH_OPTIONS_USER, remote_user.c_str());

   int rc = ssh_connect( _session );
   if(rc != SSH_OK)
   {
       fprintf(stderr, "Error in establishing ssh connection : %s \n", ssh_get_error( _session ));
       exit(-1);
   }

   if(authentication_method == "pw")
   {
       this->password_authentication(_session);
   }
   else if(authentication_method == "key")
   {
       this->public_key_authentication(_session);
   }
   else
   {
       fprintf(stderr, "Unsupported authentication method : %s \n", authentication_method.c_str());
       exit(-1);
   }

   rc = this->init_sftp_session();
   if(rc != SSH_OK)
   {
       fprintf(stderr, "SFTP error: %d \n", sftp_get_error(_sftp_session));
   } 

   //rc = sftp_mkdir( _sftp_session, "new_dir2", S_IRWXU );
   //rc = sftp_mkdir( _sftp_session, "/home/michael/Programmierung/C++/MyFileSystem/tests/testdir/new_dir2", S_IRWXU );
   if(rc != SSH_OK)
   {
       if(sftp_get_error(_sftp_session) != SSH_FX_FILE_ALREADY_EXISTS)
       {
           fprintf(stderr, "Can't create directory: %s \n", ssh_get_error(_session));
       }
   } 

   //this->create_directory();

   //this->remote_command(_session, "ls -ltr");
   //this->remote_command(_session, "cd Programmierung/C++");
   //this->remote_command(_session, "mkdir SOME_FOLDER");
   //this->remote_command(_session, "pwd");
   //this->remote_command(_session, "pwd");
   //this->remote_command(_session, "pwd");



   //fflush(stdout);
   //exit(-2);
}


int ssh_backend::password_authentication(ssh_session session)
{
    char* password;
    int rc;

    password = getpass("Enter your password: ");
    rc = ssh_userauth_password( session, NULL, password);
    if(rc == SSH_AUTH_ERROR)
    {
        fprintf(stderr, "Authentication failed: %s \n", ssh_get_error(_session));
        exit(-1);
    }

    return rc;
}


int ssh_backend::public_key_authentication(ssh_session session)
{
    int rc;

    rc = ssh_userauth_publickey_auto( session, NULL, NULL);
    if(rc == SSH_AUTH_ERROR)
    {
        fprintf(stderr, "Authentication failed: %s \n", ssh_get_error(_session));
        exit(-1);
    }

    return rc;
}


void ssh_backend::remote_command(ssh_session session, const char* command)
{
    int remote_success = this->send_remote_command(_session, command);
    if(remote_success != SSH_OK)
    {
        fprintf(stderr, "Error in send_remote_command : %s \n", ssh_get_error( _session ));
        exit(-1);
    }
}


int ssh_backend::send_remote_command(ssh_session session, const char* command)
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;
    channel = ssh_channel_new(session);
    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }
    rc = ssh_channel_request_exec(channel, command);
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0)
    {
        if (write(1, buffer, nbytes) != (unsigned int) nbytes)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
    if (nbytes < 0)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return SSH_ERROR;
    }
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}


int ssh_backend::init_sftp_session()
{
  int rc;
  _sftp_session = sftp_new(_session);
  if (_sftp_session == NULL)
  {
    fprintf(stderr, "Error allocating SFTP session: %s\n",
            ssh_get_error(_session));
    return SSH_ERROR;
  }

  rc = sftp_init(_sftp_session);
  if (rc != SSH_OK)
  {
    fprintf(stderr, "Error initializing SFTP session: code %d.\n",
            sftp_get_error(_sftp_session));
    sftp_free(_sftp_session);
    return rc;
  }
  
  return SSH_OK;
}


void ssh_backend::close_sftp_session()
{
  sftp_free(_sftp_session);
}


void ssh_backend::list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler)
{
    sftp_dir dir;
    sftp_attributes file;
    printf("requested path: %s \n", this->get_full_path(path).c_str() );

    /* the connection is made */
    /* opening a directory */
    dir = sftp_opendir(_sftp_session, this->get_full_path(path).c_str() );
    if (!dir) 
    {
        fprintf(stderr, "Directory not opened(%s)\n", ssh_get_error(_session));
        return;
    }

    /* reading the whole directory, file by file */
    while ((file = sftp_readdir( _sftp_session, dir))) 
    {
        /*fprintf(stderr, "%30s(%.8o) : %s(%.5d) %s(%.5d) : %.10llu bytes\n",
                file->name,
                file->permissions,
                file->owner,
                file->uid,
                file->group,
                file->gid,
                (long long unsigned int) file->size);*/
        filler(buffer, file->name, NULL, 0 );
        sftp_attributes_free(file);
    }

    /* when file = NULL, an error has occured OR the directory listing is end of
     * file */
    if (!sftp_dir_eof(dir)) 
    {
        fprintf(stderr, "Error: %s\n", ssh_get_error(_session));
        return;
    }

    if (sftp_closedir(dir)) 
    {
        fprintf(stderr, "Error: %s\n", ssh_get_error(_session));
        return;
    }

}


char* ssh_backend::read_file_content(const char *path, size_t offset, size_t size)
{
    sftp_file the_file = sftp_open (_sftp_session, this->get_full_path(path).c_str(), O_RDONLY, 0);
    if (the_file == NULL) 
    {
        fprintf(stderr, "Error in ssh_backend::read_file_content: %s\n", ssh_get_error(_session));
    }

    int rc = sftp_seek (the_file, offset);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error in sftp_seek: %d.\n", sftp_get_error(_sftp_session));
    }

    char* buffer = new char[size];
    ssize_t read_size = sftp_read (the_file, buffer, size);
    if(read_size < 0)
    {
        fprintf(stderr, "Error in sftp_read: %s\n", ssh_get_error(_session));
    }

    char* output_array = new char[read_size];
    memcpy( output_array, buffer, read_size );
    delete[] buffer;

    sftp_close(the_file);
    return output_array;
}


void ssh_backend::get_attributes(const char* path, struct stat* st)
{
   sftp_attributes attributes_of_file = sftp_stat(_sftp_session, this->get_full_path(path).c_str() );

   if(attributes_of_file == NULL)
   {
    fprintf(stderr, "SSH error: code %s.\n", ssh_get_error(_session));
    fprintf(stderr, "SFTP error: code %d.\n", sftp_get_error(_sftp_session));
    return;
   }

   st->st_mode = attributes_of_file->permissions;
   st->st_nlink = 1; // FIXME
   st->st_size = attributes_of_file->size;
   
   st->st_uid = attributes_of_file->uid; 
   st->st_gid = attributes_of_file->gid; 
   st->st_atime = attributes_of_file->atime;
   st->st_mtime = attributes_of_file->mtime;
}


bool ssh_backend::file_exists(const char* path)
{
   sftp_attributes attributes_of_file = sftp_stat(_sftp_session, this->get_full_path(path).c_str() );
   printf("determining if file with path %s exists **************************\n", this->get_full_path(path).c_str());
   printf("answer : %s ************************\n", (attributes_of_file == NULL) ? "false" : "true" );

   if(attributes_of_file == NULL)
   {
    fprintf(stderr, "SSH error: code %s.\n", ssh_get_error(_session));
    fprintf(stderr, "SFTP error: code %d.\n", sftp_get_error(_sftp_session));
    return false;
   }

   return true;
}


bool ssh_backend::directory_exists(const char* path)
{
   sftp_attributes attributes_of_dir = sftp_stat(_sftp_session, this->get_full_path(path).c_str());
   printf("determining if directory with path %s exists **************************\n", this->get_full_path(path).c_str());
   printf("answer : %s ************************\n", (attributes_of_dir == NULL) ? "false" : "true" );

   if(attributes_of_dir == NULL)
   {
    fprintf(stderr, "SSH error: code %s.\n", ssh_get_error(_session));
    fprintf(stderr, "SFTP error: code %d.\n", sftp_get_error(_sftp_session));
    return false;
   }

   return true;
}



void ssh_backend::set_file_mode(const char* path, mode_t new_mode)
{
    int rc = sftp_chmod( _sftp_session, this->get_full_path(path).c_str(), new_mode);
    if(rc != SSH_OK)
    {
        fprintf(stderr, "sfpt_chmod failed: %s \n", ssh_get_error(_session));
    } 
}


void ssh_backend::write_file_content(const char* path, const char* data, size_t size, off_t offset)
{
    sftp_file the_file = sftp_open (_sftp_session, this->get_full_path(path).c_str(), O_WRONLY, 0);
    if (the_file == NULL) 
    {
        fprintf(stderr, "Error in ssh_backend::write_file_content: %s\n", ssh_get_error(_session));
    }

    int rc = sftp_seek (the_file, offset);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error in sftp_seek: %d.\n", sftp_get_error(_sftp_session));
    }

    ssize_t number_of_bytes_written = sftp_write(the_file, data, size);
    if(number_of_bytes_written < 0)
    {
        fprintf(stderr, "Error in sftp_read: %s\n", ssh_get_error(_session));
    }

    sftp_close(the_file);
}


void ssh_backend::rename_file(const char* path, const char* new_path)
{
   int rc = sftp_rename(_sftp_session, this->get_full_path(path).c_str(), this->get_full_path(new_path).c_str());

   if(rc != SSH_OK)
   {
       fprintf(stderr, "Error in ssh_backend::rename_file: %s \n", ssh_get_error(_session));
   } 
}


void ssh_backend::create_directory(const char* path, mode_t new_mode)
{
    printf("calling ssh_backend::create_directory \n");
    int rc = sftp_mkdir( _sftp_session, this->get_full_path(path).c_str(), new_mode );
    if(rc != SSH_OK)
    {
       if(sftp_get_error(_sftp_session) != SSH_FX_FILE_ALREADY_EXISTS)
       {
          fprintf(stderr, "Can't create directory: %s \n", ssh_get_error(_session));
       }
    } 
}


void ssh_backend::remove_directory(const char* path)
{
    printf("calling ssh_backend::remove_directory \n");
    int rc = sftp_rmdir( _sftp_session, this->get_full_path(path).c_str());
    if(rc != SSH_OK)
    {
        fprintf(stderr, "Removing directory failed: %s \n", ssh_get_error(_session));
    } 
}


void ssh_backend::remove_file(const char* path)
{
    int rc = sftp_unlink (_sftp_session, this->get_full_path(path).c_str());
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error in ssh_backend::remove_file: code %d.\n", sftp_get_error(_sftp_session));
    }

}


void ssh_backend::create_file(const char* path, mode_t new_mode)
{
   sftp_file new_file = sftp_open ( _sftp_session, this->get_full_path(path).c_str(), O_CREAT, new_mode);
   if (new_file == NULL) 
   {
        fprintf(stderr, "Error in ssh_backend::create_file: %s\n", ssh_get_error(_session));
   }
   sftp_close(new_file);
}


void ssh_backend::set_access_and_modification_times(const char* path, const struct timespec ts[2])
{
    struct timeval tv[2];
    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;

    int rc = sftp_utimes (_sftp_session, this->get_full_path(path).c_str(), tv);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error in ssh_backend::set_access_and_modification_times: code %d.\n", sftp_get_error(_sftp_session));
    }
}


void ssh_backend::truncate_file(const char* path, off_t new_length)
{
    // FIXME
    printf("truncate not implemented \n");
}


void ssh_backend::change_ownership(const char* path, uid_t new_user, gid_t new_group)
{
    int rc = sftp_chown( _sftp_session, this->get_full_path(path).c_str(), new_user, new_group);
    if(rc != SSH_OK)
    {
        fprintf(stderr, "sfpt_chown failed: %s \n", ssh_get_error(_session));
    } 
}



void ssh_backend::debug()
{

}


std::string ssh_backend::get_full_path(const char* path)
{
    std::string full_path = _mount_dir + std::string("/") + std::string(path);
    return full_path;
}
