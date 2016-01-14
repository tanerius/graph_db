/*serializer.cpp*/
#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif
#ifndef _GRAPH_H_
#include "graph.h"
#endif

#include <pthread.h>
#include <vector>
#include <unistd.h> /* This is for file access() */
#include "serializer.h"

CSerializer::CSerializer(){
    the_graph = NULL;
    p_db_file = NULL;
    is_initialized = false;
    is_mutex_ok = true;
    create_status = PROC_IDLE;
}

CSerializer::CSerializer(Graph_p g){
    the_graph = g;
    p_db_file = NULL;
    is_initialized = false;
    is_mutex_ok = true;
    create_status = PROC_IDLE;
}

CSerializer::~CSerializer(){
    pthread_mutex_destroy(&mutex_db);
    pthread_mutex_destroy(&mutex_edge);
}

void* CSerializer::createDb(){
    if(create_status != PROC_IDLE) return NULL;
    // paranoid
    pthread_mutex_lock(&mutex_db);
    create_status = PROC_RUNNING;
    // create and open a file for writing binary stream
    p_db_file = fopen(the_graph->db_filename, "wb");
    // calculate sizes here and send a bytestream to file
    if (!p_db_file) {
        create_status = FILE_ERROR;
        pthread_mutex_unlock(&mutex_db);
        return NULL; // exit file operations something went wrong
    }
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
    create_status = PROC_OK;
    pthread_mutex_unlock(&mutex_db);
    return NULL;
}

Gdb_ret_t CSerializer::initialize(){
    int s;
    if(!is_valid()) return INVALID_SERIAL;
    Gdb_ret_t ret = initializeMutexes();
    if(!(ret == OK)) return ret;

    // go to paraniod mode with the mutex
    if( access( the_graph->db_filename, F_OK ) != -1 ) {
        // file exists - means we should read it
        return OK;
    } 
    else {
        // file doesn't exist - means we should create it
        // start the creation threads
        s = pthread_create(&db_writer_t, 0, CSerializer::createDbThread, this);
        if (s != 0)
            return TH_C_ERROR;
        pthread_join(db_writer_t, NULL);
        return OK;
    } 
    
}

Gdb_ret_t CSerializer::initializeMutexes(){
    if (pthread_mutex_init(&mutex_db, NULL) != 0)
    {
        return LOCK_ERR;
    }
    if (pthread_mutex_init(&mutex_edge, NULL) != 0)
    {
        return LOCK_ERR;
    }
    
    return OK;
}

Gdb_ret_t CSerializer::readDb(){
    return FILE_O_ERROR;
}

inline bool CSerializer::is_valid(){
    return ((the_graph != NULL) && is_mutex_ok) ;
}