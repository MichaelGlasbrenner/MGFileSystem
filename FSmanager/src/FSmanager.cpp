#include "FSmanager.h"


FSmanager::FSmanager()
{
   _log_file = fopen ("fsmanager.log", "w");

   // FIXME
   // FIXME
   // FIXME
   // 192.168.122.189
   // 192.168.122.252
   _storage_nodes.push_back(storage_node("192.168.122.189"));
   _storage_nodes.push_back(storage_node("192.168.122.252"));

   for(auto& node : _storage_nodes)
   {
       _storage_sessions.push_back(nullptr);
       bool connect_worked = this->establish_ssh_connection( *(_storage_sessions.back()), node._ip, "root", "pw" );
 
       if(connect_worked)
       {
          fprintf(_log_file, "connection to storage node %s established \n", node._ip.c_str());
          node._reachable = true;
          node._responsive = true;
       }
       else
       {
          fprintf(_log_file, "could not connect to storage node %s \n", node._ip.c_str());
       }
   }
}


FSmanager::~FSmanager()
{
    for(ssh_session* sessptr : _storage_sessions)
    {
        if(sessptr != NULL)
        {
            ssh_disconnect( *sessptr );
            ssh_free(*sessptr);
        }
    }

    for(ssh_session* sessptr : _client_sessions)
    {
        if(sessptr != NULL)
        {
            ssh_disconnect( *sessptr );
            ssh_free(*sessptr);
        }
    }

    fclose(_log_file);
}



void FSmanager::check_storage_nodes()
{

}


void FSmanager::check_client_nodes()
{

}



bool FSmanager::establish_ssh_connection(ssh_session& session, const std::string& ip_addr, const std::string& remote_user, 
                                         const std::string& authentication_method)
{
   session = ssh_new();

   if(session == NULL)
   {
       return false;
   }

   ssh_options_set( session, SSH_OPTIONS_HOST, ip_addr.c_str());
   ssh_options_set( session, SSH_OPTIONS_USER, remote_user.c_str());

   int rc = ssh_connect( session );
   if(rc != SSH_OK)
   {
       fprintf(stderr, "Error in establishing ssh connection : %s \n", ssh_get_error( session ));
       return false;
   }

   if(authentication_method == "pw")
   {
       this->password_authentication(session);
   }
   else if(authentication_method == "key")
   {
       this->public_key_authentication(session);
   }
   else
   {
       fprintf(stderr, "Unsupported authentication method : %s \n", authentication_method.c_str());
       return false;
   }

   return true;
}


int FSmanager::password_authentication(ssh_session session)
{
    char* password;
    int rc;

    password = getpass("Enter your password: ");
    rc = ssh_userauth_password( session, NULL, password);
    if(rc == SSH_AUTH_ERROR)
    {
        fprintf(stderr, "Authentication failed: %s \n", ssh_get_error(session));
        exit(-1);
    }

    return rc;
}


int FSmanager::public_key_authentication(ssh_session session)
{
    int rc;

    rc = ssh_userauth_publickey_auto( session, NULL, NULL);
    if(rc == SSH_AUTH_ERROR)
    {
        fprintf(stderr, "Authentication failed: %s \n", ssh_get_error(session));
        exit(-1);
    }

    return rc;
}


void FSmanager::remote_command(ssh_session session, const char* command)
{
    int remote_success = this->send_remote_command(session, command);
    if(remote_success != SSH_OK)
    {
        fprintf(stderr, "Error in send_remote_command : %s \n", ssh_get_error( session ));
        exit(-1);
    }
}


int FSmanager::send_remote_command(ssh_session session, const char* command)
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
