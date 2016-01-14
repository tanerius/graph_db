/*serializer.cpp*/
#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif
#ifndef _GRAPH_H_
#include "graph.h"
#endif

#include <thread>
#include <vector>
#include <unistd.h> /* This is for file access() */
#include "serializer.h"


CSerializer::CSerializer(Graph_p g = NULL){
    the_graph = g;
    p_db_file = NULL;
    is_initialized = false;
}

const Gdb_ret_t CSerializer::create_db(){
    p_db_file = fopen(the_graph->db_filename, "wb");
    // calculate sizes here and send a bytestream to file
    if (!p_db_file) return FILE_I_ERROR; // exit file operations something went wrong


    fclose(p_db_file);
    return OK;
}

const Gdb_ret_t CSerializer::initialize(){
    if(!is_valid()) return INVALID_SERIAL;
    // go to paraniod mode with the mutex
    std::lock_guard<std::mutex> guard(mutex);
    if( access( the_graph->db_filename, F_OK ) != -1 ) {
        // file exists - means we should read it
        return read_db();
    } else {
        // file doesn't exist - means we should create it
        return create_db();
    } 
    
}

const Gdb_ret_t CSerializer::read_db(){
    return FILE_O_ERROR;
}

inline bool CSerializer::is_valid(){
    return (the_graph != NULL);
}