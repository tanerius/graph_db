/*graph.h*/
#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif
#ifndef _GRAPH_H_
#include "graph.h"
#endif
#include <stdio.h>
#include <pthread.h>
#include <vector>


#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

/*! 
    Serializer class is responsible for reading and writing 
*/
class CSerializer {
    public:
        

        CSerializer();
        CSerializer(Graph_p g);
        ~CSerializer();
        
        static void* createDbThread(void *arg) { return static_cast<CSerializer*>(arg)->createDb(); }

        Gdb_ret_t initialize();
        
    private:
        pthread_t db_writer_t;      /*!< Thread used for writing gdb */
        pthread_t edge_writer_t;    /*!< Thread used for writing edb */

        pthread_mutex_t mutex_db;       /*!< Mutex - only one thread at a time can write to graph file */
        pthread_mutex_t mutex_edge;     /*!< Mutex - only one thread at a time can write to edge file */
        Graph_p the_graph;      /*!< The graph object to be serialized */
        bool is_initialized;    /*!< False when no files opened */
        FILE* p_db_file;        /*!< File contailing the graph and vertexes */
        FILE* p_edges_file;     /*!< File contailing the edges */
        bool is_mutex_ok;
        bool is_valid();
        Gdb_ret_t create_status;

        void* createDb();
        Gdb_ret_t createEdges();
        Gdb_ret_t initializeMutexes();
        Gdb_ret_t readDb();
        Gdb_ret_t readEdges();
};

#endif