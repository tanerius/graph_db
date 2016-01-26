#include "gdbserver.h"

int main(){
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(1);
    }
    /* If we got a good PID, then
    we can exit the parent process. */
    if (pid > 0) {
        printf("Closing out standard descriptors...\n");
        exit(0);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    printf("Closing out standard descriptors...\n");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Daemon-specific initialization goes here */
    bool continue_cycle=true;
    /* The Big Loop */
    while (continue_cycle) {
        /* Do some task here ... */
        
        continue_cycle = false;
        sleep(30); /* wait 30 seconds */
    }
    printf("Quitting main...\n");
    
    return 0;
}