#ifndef _FSMANAGER_
#define _FSMANAGER_

#include <vector>
#include <string>
#include <stdio.h>
#include <libssh/libssh.h>

// FIXME : common base class for client_node and storage_node ??

class client_node
{
    public:
       client_node(std::string ip) :
           _ip(ip),
           _reachable(false),
           _responsive(false),
           _fs_mounted(false)
       {

       }

    public: // FIXME
       std::string _ip;
       bool _reachable;
       bool _responsive;
       bool _fs_mounted;
};


class storage_node
{
    public:
        storage_node(std::string ip) :
            _ip(ip),
            _reachable(false),
            _responsive(false),
            _fs_available(false)
        {

        }

    public: // FIXME
       std::string _ip;
       bool _reachable;
       bool _responsive;
       bool _fs_available;
};


class  FSmanager
{
   public:
       FSmanager();
       ~FSmanager();
       void check_storage_nodes();
       void check_client_nodes();

   private:
       bool establish_ssh_connection(ssh_session& session, const std::string& ip_addr, 
                                     const std::string& remote_user, const std::string& authentication_method);
       int  send_remote_command(ssh_session session, const char* command);
       int  password_authentication(ssh_session session);
       int  public_key_authentication(ssh_session session);
       void remote_command(ssh_session session, const char* command);
       void print_status();

   private:
       std::vector<client_node> _client_nodes;
       std::vector<storage_node> _storage_nodes;

       std::vector<ssh_session> _storage_sessions;
       std::vector<ssh_session> _client_sessions;

       FILE* _log_file;
};


#endif
