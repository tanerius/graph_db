#include "gdbserver.h"

int main(int argc, char *argv[]){
    // Check to make sure args were not requested
    if(argc > 1){
        if((strcmp(argv[1],"-v") == 0)||(strcmp(argv[1],"--version") == 0)){
            printf("GraphDB Server v%d.%d.%d\nCopyright 2016 Taner Selim <tanerius@gmail.com>\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REV);
            printf("Version %d.%d Revision %d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REV);
            //printf("Defaults:\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REV);
        }
    }

    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();

    if (pid < 0) {
        perror("Cannot fork()");
        exit(1);
    }

    /* If we got a good PID, then
    we can exit the parent process. */
    if (pid > 0) {
        exit(0);
    }

    // Here the child process continues. Now daemonize!
    createPID(false); // create the PID file or assert if a daemon is already running

    /* Change the file mode mask */
    umask(0);

    /* Opening threadsafe logs */
    GdbLoggerMain::Instance()->Log("[OK] Starting gdbd session...");

    /* Create a session ID for the process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        GdbLoggerMain::Instance()->Log("[ERROR] Cannot setsid(). Exitting. ");
        exit(1);
    }

    if ((chdir("/")) < 0) {
        GdbLoggerMain::Instance()->Log("[ERROR] Changing working dir.");
        exit(1);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Daemon-specific initialization goes here */
    bool continue_cycle=true;
    /* The Big Loop */
    while (continue_cycle) {
        /* Do some task here ... */
        GdbLoggerMain::Instance()->Log("[OK] In main loop.");
        
        continue_cycle = false;
        sleep(30); /* wait 30 seconds */
    }
    GdbLoggerMain::Instance()->Log("[OK] Shutting down gdbd.");
    removePID();
    return 0;
}