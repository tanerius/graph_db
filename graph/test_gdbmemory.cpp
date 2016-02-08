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
    Gdb_N_t value_to_store=0;
    for (Gdb_N_t xx=1;xx <= 1000000000;xx++){
        value_to_store = xx*2;
        fwrite(&value_to_store, sizeof(Gdb_N_t), 1, p_db_file); 
    }
    fclose(p_db_file);
}


int main(int argc , char *argv[])
{
    //GdbMemory graph(true);
    fd = open("foo", O_RDONLY);
pagesize = getpagesize();
    return 0;
    
} 