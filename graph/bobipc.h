#ifndef _BOBIPC_H_
#define _BOBIPC_H_

/*
    These are helpers for inter process communication.
*/
#ifndef _BOBUTILS_H_
#include "bobutils.h"
#endif

typedef struct Shared_buffer_s {
    void *data_ptr; 
    Bobuint_t buffer_len;
    Bobuint_t entity_size;
    Bobuint_t total_entities;
    Types_t data_type;
    bool lock_ram;
} Shared_buffer_t;

int shared_buffer_init(Shared_buffer_t *_instance);

#endif