#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include "FSmanager.h"

using namespace std; // FIXME

#define DAEMON_NAME "FSmanager"

// #define daemonize // FIXME


void process(){

    syslog (LOG_NOTICE, "Writing to my Syslog");
}   

int main(int argc, char *argv[]) {

    //Set our Logging Mask and open the Log
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

    syslog(LOG_INFO, "Entering Daemon");

#ifdef daemonize
    pid_t pid, sid;

   //Fork the Parent Process
    pid = fork();

    if (pid < 0) { exit(EXIT_FAILURE); }

    //We got a good pid, Close the Parent Process
    if (pid > 0) { exit(EXIT_SUCCESS); }

    //Change File Mask
    umask(0);

    //Create a new Signature Id for our child
    sid = setsid();
    if (sid < 0) { exit(EXIT_FAILURE); }

    //Change Directory
    //If we cant find the directory we exit with failure.
    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

    //Close Standard File Descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
#endif

    // create FSmanager object
    FSmanager fsmanager;

    //----------------
    //Main Process
    //----------------
    while(true){
        process();    //Run our Process

        fsmanager.check_storage_nodes();
        fsmanager.check_client_nodes();

        sleep(10);    //Sleep for 60 seconds
    }

    //Close the log
    closelog ();
}
