#include "ssh_backend.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>



ssh_backend mg_filesystem_data;

ssh_backend::ssh_backend()
{
   this->establish_ssh_connection();

}


ssh_backend::~ssh_backend()
{
    this->close_sftp_session();

    ssh_disconnect( _session );
    ssh_free(_session);

}


void ssh_backend::establish_ssh_connection()
{
   _session = ssh_new();

   if(_session == NULL)
   {
       exit(-1);
   }

   //ssh_options_set( _session, SSH_OPTIONS_HOST, "192.168.2.112");
   //ssh_options_set( _session, SSH_OPTIONS_USER, "michael");
   ssh_options_set( _session, SSH_OPTIONS_HOST, "192.168.2.100");
   ssh_options_set( _session, SSH_OPTIONS_USER, "michael");
   //ssh_options_set( _session, SSH_OPTIONS_HOST, "10.0.101.1");
   //ssh_options_set( _session, SSH_OPTIONS_USER, "mglasbre");

   int rc = ssh_connect( _session );
   if(rc != SSH_OK)
   {
      fprintf(stderr, "Error in establishing ssh connection : %s \n", ssh_get_error( _session ));
      exit(-1);
   }

   this->password_authentication(_session);
   //this->public_key_authentication(_session);

   rc = this->init_sftp_session();
   if(rc != SSH_OK)
   {
       fprintf(stderr, "SFTP error: %s \n", sftp_get_error(_sftp_session));
   } 
    
   rc = sftp_mkdir( _sftp_session, "new_dir2", S_IRWXU );
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
       return SSH_AUTH_ERROR;
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
       return SSH_AUTH_ERROR;
    }

    return rc;
}


void ssh_backend::remote_command(ssh_session session, const char* command)
{
   int remote_success = this->send_remote_command(_session, command);
   if(remote_success != SSH_OK)
   {
      fprintf(stderr, "Error in establishing send_remote_command : %s \n", ssh_get_error( _session ));
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

}


char* ssh_backend::read_file_content(const char *path)
{

}


void ssh_backend::get_attributes(const char* path, struct stat* st)
{
   //sftp_attributes attributes_of_file = sftp_stat(_sftp_session, path);
   sftp_attributes attributes_of_file = sftp_stat(_sftp_session, "new_dir");

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

}


bool ssh_backend::directory_exists(const char* path)
{

}



void ssh_backend::set_file_mode(const char* path, mode_t new_mode)
{

}


void ssh_backend::write_file_content(const char* path, const char* data, size_t size, off_t offset)
{

}


void ssh_backend::rename_file(const char* path, const char* new_path)
{

}


void ssh_backend::create_directory(const char* path, mode_t new_mode)
{
    printf("calling ssh_backend::create_directory \n");
    int rc = sftp_mkdir( _sftp_session, "new_dir", S_IRWXU );
    if(rc != SSH_OK)
    {
       if(sftp_get_error(_sftp_session) != SSH_FX_FILE_ALREADY_EXISTS)
       {
          fprintf(stderr, "Can't create directory: %s \n", ssh_get_error(_session));
       }
    } 
}


void ssh_backend::remove_file(const char* path)
{

}


void ssh_backend::create_file(const char* path, mode_t new_mode)
{

}


void ssh_backend::set_access_and_modification_times(const char* path, const struct timespec tv[2])
{

}


void ssh_backend::truncate_file(const char* path, off_t new_length)
{

}


void ssh_backend::change_ownership(const char* path, uid_t new_user, gid_t new_group)
{

}



void ssh_backend::debug()
{

}


