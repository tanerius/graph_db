#include "bobipc.h"

/*
    Routine to initialize a buffer which will be shared across forks
*/
int shared_buffer_init(Shared_buffer_t *_instance) {
    _instance->buffer_len = _instance->total_entities * _instance->entity_size;
    _instance->data_ptr = (Byte_t*)mmap(NULL,_instance->buffer_len, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1 ,0);
    
    if (_instance->data_ptr==MAP_FAILED) {
        _instance->buffer_len = 0;
        _instance->total_entities = 0;
        return errno;
    }
    
    if (_instance->lock_ram == TRUE) {
        if (mlock(_instance->data_ptr, _instance->buffer_len) == -1) {
            return errno;
        }
    }
    
    return 0;
}