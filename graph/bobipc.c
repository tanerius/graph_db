#include "bobipc.h"



/*
    Routine to create a shared mem buffer which will survive a fork
*/
Shared_msg_t* create_mmap(Bobuint_t map_size) {
    void* saddr = 0; // starting address of the mapping. Let kernel decide
    int map_security = PROT_READ|PROT_WRITE; //can read and write to map
    int flags = MAP_SHARED|MAP_ANONYMOUS; // just share mem no file mapping
    int fd = -1; // used if mapping to a file which in this case no
    int offset = 0;

    Shared_msg_t* ret = mmap(saddr, map_size, map_security, flags, fd, offset);

    if ((void*) ERROR == ret) {
        return NULL;
    }

    return ret
}

/*
    Delete a created map
*/
void delete_mmap(void* addr) {
    munmap(arrd, sizeof(Shared_msg_t));
}