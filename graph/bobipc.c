#include "bobipc.h"


int shared_buffer_init(Shared_buffer_t *_instance) {
    Bobuint_t element_size = _instance->total_entities * sizeof(_instance->entity_type);
    
}