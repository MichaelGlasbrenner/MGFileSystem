#define FUSE_USE_VERSION 26

#define _FILE_OFFSET_BITS  64 // added by me

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <libssh/libssh.h> // FIXME delete

#include "mg_operations.h"

#include <tclap/CmdLine.h>

//extern filesystem_data mg_filesystem_data;
//extern ssh_backend mg_filesystem_data;
extern storage_backend* mg_filesystem_data;


struct mg_fuse_operations : fuse_operations
{
    mg_fuse_operations ()
    {
        getattr     = mg_getattr;
        readdir     = mg_readdir;
        read        = mg_read;
        write       = mg_write;
        chmod       = mg_chmod;
        chown       = mg_chown;
        mkdir       = mg_mkdir;
        rename      = mg_rename;
        truncate    = mg_truncate;
        unlink      = mg_unlink;
        create      = mg_create;
        mknod       = mg_mknod;
        utimens     = mg_utimens;
        readlink    = mg_readlink;
        rmdir       = mg_rmdir;
        symlink     = mg_symlink;
        open        = mg_open;
        flush       = mg_flush;
        release     = mg_release;
        fsync       = mg_fsync;
        setxattr    = mg_setxattr;
        getxattr    = mg_getxattr;
        listxattr   = mg_listxattr;
        removexattr = mg_removexattr;
        opendir     = mg_opendir;
        releasedir  = mg_releasedir;
        fsyncdir    = mg_fsyncdir;
    }
};

static struct mg_fuse_operations mg_oper;


int main( int argc, char *argv[] )
{
    printf("argc : %d \n", argc);
    for(int i=0; i < argc; ++i)
    {
        printf("argv[%d] : %s \n", i, argv[i]);
    }

    try
    {
        char delimiter='=';
        TCLAP::CmdLine cmd("Command description message", delimiter, "0.9");

        std::vector<std::string> allowed_backends;
        allowed_backends.push_back("ram");
        allowed_backends.push_back("ssh");
        TCLAP::ValuesConstraint<std::string> allowedBackends( allowed_backends );    

        std::vector<std::string> allowed_auth_methods;
        allowed_auth_methods.push_back("pw");
        allowed_auth_methods.push_back("key");
        TCLAP::ValuesConstraint<std::string> allowedAuthMethods( allowed_auth_methods );    
 
        TCLAP::ValueArg<std::string> storage_backend_Arg("", "backend", "storage backend", true, "ssh", &allowedBackends);
        TCLAP::ValueArg<std::string> ssh_dir_Arg("", "ssh", "ssh remote directory", false, "", "string");
        TCLAP::ValueArg<std::string> auth_Arg("", "auth", "ssh authentication method", false, "pw", &allowedAuthMethods);
        TCLAP::ValueArg<std::string> mount_Arg("", "mount-point", "mount point", true, "", "string");
        TCLAP::SwitchArg debug_Arg("", "debug", "debug mode", false);
        TCLAP::SwitchArg foreground_Arg("f", "", "run FUSE in foreground", false);
        TCLAP::SwitchArg singleThread_Arg("s", "", "run single-threaded", false);
        TCLAP::MultiArg<std::string> fuse_mount_option_Args("o", "", "FUSE mount option", false, "string");
     
        cmd.add( storage_backend_Arg );
        cmd.add( ssh_dir_Arg );
        cmd.add( auth_Arg );
        cmd.add( mount_Arg );
        cmd.add( debug_Arg );
        cmd.add( foreground_Arg );
        //cmd.add( singleThread_Arg );
        //cmd.add( fuse_mount_option_Args );
     
        cmd.parse(argc, argv);
     
        std::string which_backend = storage_backend_Arg.getValue();
        std::string ssh_dir = ssh_dir_Arg.getValue();
        std::string auth_method = auth_Arg.getValue();
        std::string mount_point = mount_Arg.getValue();
        bool fuse_debug = debug_Arg.getValue();
        bool fuse_foreground_operation = foreground_Arg.getValue();
        bool single_threaded = singleThread_Arg.getValue();
        std::vector<std::string> fuse_mount_options = fuse_mount_option_Args.getValue();

        /////////////////////////////
        printf("which_backend : %s \n", which_backend.c_str());
        printf("ssh_dir : %s \n", ssh_dir.c_str());
        printf("auth_method : %s \n", auth_method.c_str());
        printf("mount_point : %s \n", mount_point.c_str());
        printf("fuse_debug : %s \n", fuse_debug ? "TRUE" : "FALSE");
        printf("fuse_foreground_operation : %s \n", fuse_foreground_operation ? "TRUE" : "FALSE");
        printf("single_threaded : %s \n", single_threaded ? "TRUE" : "FALSE");
        /////////////////////////////

        if(which_backend == "ssh")
        {
            mg_filesystem_data = new ssh_backend();

            if(ssh_dir.size() == 0)
            {
                printf("ssh mount argument missing\n");
                exit(-1);
            }

            // parsing address of form <user>@<ip-addr>:<directory>
            std::string remote_user = ssh_dir.substr(0, ssh_dir.find("@"));
            std::string ip_addr =  ssh_dir.substr(ssh_dir.find("@")+1, ssh_dir.find(":")-ssh_dir.find("@")-1);
            std::string remote_dir =  ssh_dir.substr(ssh_dir.find(":")+1, ssh_dir.size()-ssh_dir.find(":"));

            if( remote_user.size() == 0 || ip_addr.size() == 0 || remote_dir.size() == 0)
            {
                printf("bad ssh mount argument\n");
                exit(-1);
            }

            printf("remote_user : %s \n", remote_user.c_str());
            printf("ip : %s \n", ip_addr.c_str());
            printf("remote_dir : %s \n", remote_dir.c_str());
            ((ssh_backend*)mg_filesystem_data)->init(remote_dir, ip_addr, remote_user, auth_method);
        }
        else if(which_backend == "ram")
        {
            mg_filesystem_data = new filesystem_data();

        }

        // preparing fuse "command line" arguments
        std::vector<std::string> fuse_arg_strings;
        fuse_arg_strings.push_back( std::string(argv[0]) ); // program name
        fuse_arg_strings.push_back( mount_point );
        if(fuse_debug == true) fuse_arg_strings.push_back("-d"); 
        if(fuse_foreground_operation == true) fuse_arg_strings.push_back("-f"); 
        if(single_threaded == true) fuse_arg_strings.push_back("-s"); 

        int fuse_argc = fuse_arg_strings.size();
        char** fuse_argv = new char*[fuse_argc]; 
        for(int i=0; i < fuse_argc; ++i)
        {
            fuse_argv[i] = const_cast<char*>(fuse_arg_strings[i].c_str());
            printf("fuse_argv[%d] : %s \n", i, fuse_argv[i]);
        }
    
        int return_code = fuse_main( fuse_argc, fuse_argv, &mg_oper, NULL );

        delete[] fuse_argv;

        if(which_backend == "ssh")
        {
           ((ssh_backend*)mg_filesystem_data)->clean_up();
        }

        delete mg_filesystem_data;

        return return_code; // FIXME
    }
    catch(TCLAP::ArgException& e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

}

