#ifndef _BOBPROCESS_H_
#define _BOBPROCESS_H_

/*
    These are helpers for process control.
*/
#ifndef _BOBUTILS_H_
    #include "bobutils.h"
#endif

#ifndef _BOBIPC_H_
    #include "bobipc.h"
#endif
#include <signal.h>



/*
    a function which helps to fork children processes by providind a handler function as an argument and an IPC variable
*/
pid_t fork_child(void (*func)(void*), void*); 
void wait_for_children(pid_t*, const unsigned char); // function to instruct parent to wait for its children

#endif