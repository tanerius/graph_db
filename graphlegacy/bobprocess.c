#include "bobprocess.h"

pid_t fork_child(void (*func)(void* arg), void* state_args) {
    pid_t child_pid;
    switch (child_pid = fork()) {
        case ERROR:
            // could not fork
            return ERROR;
            break;
        case 0:
            // successfully forked this is the child 
            (*func)(state_args); 
            exit(0); 
            break;
        default: 
            // parent continues
            return child_pid;
            break;
    }
}

void wait_for_children(pid_t* children, const unsigned char processes) {
    int status;
    while (ERROR < wait(&status)) {
        if (!WIFEXITED(status)) {
            for (int c = 0; c < processes; c++) {
                kill(children[c], SIGKILL);
            }
            break;
        }
    }
}