#ifndef _BOBIPC_H_
#define _BOBIPC_H_

/*
    These are helpers for inter process communication.
*/
#ifndef _BOBUTILS_H_
#include "bobutils.h"
#endif

#ifndef SH_MSG_SIZE
    #define SH_MSG_SIZE 45
#endif
    
#define ERROR -1

typedef struct Shared_msg_s {
    char msg[SH_MSG_SIZE];
    int count;
} Shared_msg_t;

Shared_msg_t* create_mmap(Bobuint_t map_size);
void delete_mmap(void* addr);
#endif