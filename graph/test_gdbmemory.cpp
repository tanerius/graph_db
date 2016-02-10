#include "graph.h"

// Function that stores a billion numbers in a binary 
void create_huge_file(const char* filename){
    FILE* p_db_file;
    p_db_file = fopen(filename, "wb");
    // calculate sizes here and send a bytestream to file
    if (!p_db_file) {
        printf("Error \n");
        exit(1);
    }
    // write the current graph structure first - start by writing the total bytes required
    // We do this so we can later check if we read the file correctly
    printf("Storing 1000000000 * %lu bytes...\n",sizeof(Gdb_N_t));
    for (Gdb_N_t xx=0;xx < 1000000000;xx++){
        fwrite(&xx, sizeof(Gdb_N_t), 1, p_db_file); 
    }
    fclose(p_db_file);
}

void map_read_huge_file(const char* filename){
    int fd = open(filename, O_RDONLY);
    int offset=3;
    int pagesize = getpagesize();
    printf("The current pagesize is %d \n",pagesize);
    Gdb_N_t *data;
    data = (Gdb_N_t*)mmap((caddr_t)0, pagesize, PROT_READ, MAP_SHARED, fd, pagesize);
    if(data == MAP_FAILED){
        printf("Mapping failed because: %d",errno);
        exit(1);
    }

    printf("Read file starting from byte: %d\n",pagesize);

    printf("Read file starting from byte: %d\n",pagesize);
    printf("An object is %lu bytes wide\n",sizeof(Gdb_N_t));
    printf("Advance pointer by %d objects or %lu bytes\n",offset, offset*sizeof(Gdb_N_t));
    printf("Print next 5 objects: \n");
    for (int i=0;i<5;i++){
        printf("data[%d]=%lu\n",offset+i ,data[offset+i]);
    }

    // unmap
    munmap(data, pagesize);
    close(fd);
}



/*



*/


int main(int argc , char *argv[])
{
    //GdbMemory graph(true);

    
    return 0;
    
} 