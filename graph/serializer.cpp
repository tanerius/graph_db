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

CSerializer::CSerializer(){
    the_graph = NULL;
    p_db_file = NULL;
    is_initialized = false;
}

CSerializer::CSerializer(Graph_p g){
    the_graph = g;
    p_db_file = NULL;
    is_initialized = false;
}

const Gdb_ret_t CSerializer::create_db(){
    // create and open a file for writing binary stream
    p_db_file = fopen(the_graph->db_filename, "wb");
    // calculate sizes here and send a bytestream to file
    if (!p_db_file) return FILE_I_ERROR; // exit file operations something went wrong
    // write the current graph structure first - start by writing the total bytes required
    // We do this so we can later check if we read the file correctly
    Gdb_N_t total_bytes = 6 * sizeof(Gdb_N_t); /* for all natural numbers in graph struct + this */
    total_bytes = 6 * sizeof(Gdb_N_t); /* for all natural numbers in graph struct + this */
    total_bytes += sizeof(Gdb_graph_t); /* for the graph type */
    fwrite(&total_bytes, sizeof(Gdb_N_t), 1, p_db_file); 
    fwrite(&(the_graph->id_hi), sizeof(Gdb_N_t), 1, p_db_file); 
    fwrite(&(the_graph->id_lo), sizeof(Gdb_N_t), 1, p_db_file); 
    fwrite(&(the_graph->num_vertices), sizeof(Gdb_N_t), 1, p_db_file); 
    fwrite(&(the_graph->total_deleted), sizeof(Gdb_N_t), 1, p_db_file); 
    fwrite(&(the_graph->next_available_id), sizeof(Gdb_N_t), 1, p_db_file); 
    fwrite(&(the_graph->type), sizeof(Gdb_graph_t), 1, p_db_file); 
    // Done writing the initial structure of the graph 
    // now lets write the adjecency list (without the edges)
    // always start with the size
    total_bytes = the_graph->num_vertices * ((5 * sizeof(Gdb_N_t)) + sizeof(bool)); // include this variable too
    fwrite(&total_bytes, sizeof(Gdb_N_t), 1, p_db_file); // write the size even if its 0 so we can wnow it
    // write all vertexes now
    for(int x=0; x < the_graph->num_vertices; x++){
        List_p one_node =  &(the_graph->arr_list[x]);
        fwrite(&(one_node->id_hi), sizeof(Gdb_N_t), 1, p_db_file);
        fwrite(&(one_node->id_lo), sizeof(Gdb_N_t), 1, p_db_file);
        fwrite(&(one_node->num_edges), sizeof(Gdb_N_t), 1, p_db_file);
        fwrite(&(one_node->list_id), sizeof(Gdb_N_t), 1, p_db_file);
        fwrite(&(one_node->is_deleted), sizeof(bool), 1, p_db_file);
    }


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