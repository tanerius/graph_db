#ifndef _BOBIPC_H_
#define _BOBIPC_H_

/*
    These are helpers fir inter process communication.
*/
#ifndef _BOBUTILS_H_
#include "bobutils.h"
#endif

typedef struct Shared_buffer_s {
    void *data_ptr;
    Bobuint_t len;
    Bobuint_t total_entities;
    bool lock_ram;
    Types_t data_type;
} Shared_buffer_t;

int shared_buffer_init(Shared_buffer_t *_instance);


#endif