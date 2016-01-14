/*graph.h*/
#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif
#ifndef _GRAPH_H_
#include "graph.h"
#endif
#include <stdio.h>
#include <thread>
#include <vector>
#include <mutex>

#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

 /*! 
    Serializer class is responsible for reading and writing 
*/
class CSerializer {
    public:
        std::mutex mutex; /*!< Mutex to make sure that only one thread at a time can have access to this */

        CSerializer();
        CSerializer(Graph_p g);
        const Gdb_ret_t create_db();
        const Gdb_ret_t initialize();
        const Gdb_ret_t read_db();
    private:
        Graph_p the_graph;      /*!< The graph object to be serialized */
        bool is_initialized;    /*!< False when no files opened */
        FILE* p_db_file;        /*!< File contailing the graph */

        bool is_valid();

};

#endif