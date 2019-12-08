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

   ssh_options_set( _session, SSH_OPTIONS_HOST, "192.168.2.112");
   ssh_options_set( _session, SSH_OPTIONS_USER, "michael");

   int rc = ssh_connect( _session );
   if(rc != SSH_OK)
   {
      fprintf(stderr, "Error in establishing ssh connection : %s \n", ssh_get_error( _session ));
      exit(-1);
   }

   this->password_authentication(_session);

   int remote_success = this->show_remote_processes(_session);
   if(remote_success != SSH_OK)
   {
      fprintf(stderr, "Error in establishing show_remote_processes : %s \n", ssh_get_error( _session ));
      exit(-1);
   }

   fflush(stdout);
   exit(-2);
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


int ssh_backend::show_remote_processes(ssh_session session)
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
    rc = ssh_channel_request_exec(channel, "ls -ltr");
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


void ssh_backend::list_files_in_dir(const char *path, void *buffer, fuse_fill_dir_t filler)
{

}


char* ssh_backend::read_file_content(const char *path)
{

}


void ssh_backend::get_attributes(const char* path, struct stat* st)
{

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


