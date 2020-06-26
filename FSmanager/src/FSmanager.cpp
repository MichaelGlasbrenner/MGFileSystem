#include "FSmanager.h"
#include <syslog.h>


FSmanager::FSmanager()
{
   _log_file = fopen ("/home/michael/Programmierung/C++/MyFileSystem/FSmanager/fsmanager.log", "w");
   if(_log_file == NULL)
   {
      syslog (LOG_NOTICE, "Failed to open log-file");
      exit(-1);
   }
   syslog (LOG_NOTICE, " opened log-file");

   fprintf(_log_file, "starting FSmanager\n\n");
   fflush(_log_file);

   // FIXME
   // FIXME
   // FIXME
   // 192.168.122.189
   // 192.168.122.252
   _storage_nodes.push_back(storage_node("192.168.122.189"));
   _storage_nodes.push_back(storage_node("192.168.122.252"));

   for(size_t i=0; i < _storage_nodes.size(); ++i)
   {
       auto& node = _storage_nodes[i];

       ssh_session new_session = ssh_new();
       _storage_sessions.push_back(new_session);
       //bool connect_worked = true; // FIXME
       bool connect_worked = this->establish_ssh_connection( _storage_sessions[i], node._ip, "root", "key" );
 
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
   fflush(_log_file);
}


FSmanager::~FSmanager()
{
    for(ssh_session sess : _storage_sessions)
    {
        if(sess != NULL)
        {
            ssh_disconnect( sess );
            ssh_free(sess);
        }
    }

    for(ssh_session sess : _client_sessions)
    {
        if(sess != NULL)
        {
            ssh_disconnect( sess );
            ssh_free(sess);
        }
    }

    fclose(_log_file);
}



void FSmanager::check_storage_nodes()
{
     std::string fs_path = "/root/local_filesystem"; // FIXME
    
         fprintf(_log_file, "been here1 \n");
         fflush(_log_file);
     for(size_t i=0; i < _storage_nodes.size(); ++i)
     {
          auto& node = _storage_nodes[i];
          ssh_session session = _storage_sessions[i]; 

         //std::string command = "ls -ltr " + fs_path;
         std::string command = "if [ -d \"" + fs_path + "\" ]; then echo \"0\"; else echo \"1\"; fi ";
         ssh_channel channel;
         int rc;
         char buffer[256];
         int nbytes;
         fprintf(_log_file, "been here2 \n");
         fflush(_log_file);

         channel = ssh_channel_new(session);
         if (channel == NULL)
         {
             node._reachable = false;
             node._responsive = false;
             continue;
         }
         fprintf(_log_file, "been here3 \n");
         fflush(_log_file);

         rc = ssh_channel_open_session(channel);
         if (rc != SSH_OK)
         {
             ssh_channel_free(channel);
             node._reachable = false;
             node._responsive = false;
             continue;
         }
         fprintf(_log_file, "been here4 \n");
         fflush(_log_file);

         rc = ssh_channel_request_exec(channel, command.c_str());
         if (rc != SSH_OK)
         {
             ssh_channel_close(channel);
             ssh_channel_free(channel);

             node._reachable = false;
             node._responsive = false;
             continue;
         }
         fprintf(_log_file, "been here5 \n");
         fflush(_log_file);

         nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);

         while (nbytes > 0)
         {
             if (write(1, buffer, nbytes) != (unsigned int) nbytes)
             {
                 ssh_channel_close(channel);
                 ssh_channel_free(channel);

                 node._reachable = false;
                 node._responsive = false;
                 continue;
             }
             nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
             fprintf(_log_file, "been here6 \n");
             fflush(_log_file);
         }

         if (nbytes < 0)
         {
             ssh_channel_close(channel);
             ssh_channel_free(channel);

             node._reachable = false;
             node._responsive = false;
             continue;
         }

         fprintf(_log_file, "been here7 \n");
         fflush(_log_file);

         fprintf(_log_file, "buffer : %d \n", (int)buffer[0]);

         if(buffer[0] == '0') // -> directory exists on the node
         {
             node._reachable = true;
             node._responsive = true;
         }
         else if(buffer[0] == '1') // -> directory does not exists, but node responds
         {
             node._reachable = false;
             node._responsive = false;
         }

         fflush(_log_file);

         ssh_channel_send_eof(channel);
         ssh_channel_close(channel);
         ssh_channel_free(channel);

         // FIXME : check also if unison is running
         // with a command like this: (return 0 or 1)
         // systemctl is-active --quiet sshd
     }


     this->print_status();
}


void FSmanager::check_client_nodes()
{

}


void FSmanager::print_status()
{
    fprintf(_log_file, "\nfilesystem status: \n\n");

    for(auto& node : _storage_nodes)
    {
        fprintf(_log_file, "%s %s %s \n", node._ip.c_str(), (node._reachable ? "true" : "false"), (node._responsive ? "true" : "false"));
    }
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
       fprintf(_log_file, "Error in establishing ssh connection : %s \n", ssh_get_error( session ));
       return false;
   }

   if(authentication_method == "pw")
   {
       rc = this->password_authentication(session);
   }
   else if(authentication_method == "key")
   {
       rc = this->public_key_authentication(session);
   }
   else
   {
       fprintf(_log_file, "Unsupported authentication method : %s \n", authentication_method.c_str());
       return false;
   }
   
   if(rc != SSH_OK)
   {
       fprintf(_log_file, "authentication error : %s \n", ssh_get_error( session ));
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
        fprintf(_log_file, "Authentication failed: %s \n", ssh_get_error(session));
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
        fprintf(_log_file, "Authentication failed: %s \n", ssh_get_error(session));
        exit(-1);
    }

    return rc;
}


void FSmanager::remote_command(ssh_session session, const char* command)
{
    int remote_success = this->send_remote_command(session, command);
    if(remote_success != SSH_OK)
    {
        fprintf(_log_file, "Error in send_remote_command : %s \n", ssh_get_error( session ));
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
    {
        return SSH_ERROR;
    }

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
