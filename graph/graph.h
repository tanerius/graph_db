/*graph.h*/
#ifndef _GRAPH_H_
#define _GRAPH_H_

#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


/*! 
    Adjacency list node representation
*/
typedef struct Node
{
    /*
        The id of the vertex that this edge is pointing to.
    */
    Gdb_N_t vertex;         /*!<  index of the vertex being pointed at */
    Gdb_edge_t type;        /*!< Type of edge */
    Gdb_Payload_p payload;  /*!< Generic payload */
    struct Node *next;      /*!< Pointer to the next node */
}Node_t, *Node_p;
 
/*! 
    Adjacency list - This is actually a representation of a vertex
*/
typedef struct List
{
    Gdb_N_t id_hi;                   /* hi order id */
    Gdb_N_t id_lo;                   /* low order id */
    Gdb_N_t num_edges;             /* number of edges */
    Gdb_N_t list_id;                 /* unique id of list - incremental*/
    Gdb_Payload_p payload;           /* List payload */
    bool is_deleted;                     /* should be purged on garbage collection */
    Node_t *head;                       /*head of the adjacency linked list is the actual element */
}List_t, *List_p;
 
/*! 
    This is the main graph structure
*/
typedef struct Graph
{
    /* last id assigned */
    Gdb_N_t id_hi;                   /* hi order id - for counting */
    Gdb_N_t id_lo;                   /* low order id - for counting */

    /* for multiple threads access */
    bool is_safe_for_access;
    bool is_all_locked;


    /* filename where graph is stored for persistence */
    char *db_filename;

    /* Define the type of graph */
    Gdb_graph_t type;      
    /* Number of vertices */
    Gdb_N_t num_vertices;
    /* Total number of deleted elements */
    Gdb_N_t total_deleted;
    // next free id to allocate with an element
    Gdb_N_t next_available_id;
    /* Pointer to a deleted element in adjacency list (caching optimization) */
    List_p deleted_element;
    /* Array of adjacency lists */
    List_p arr_list;
    /* Becomes true when graph needs to grow its element page */
    bool needs_page_increase;
}Graph_t, *Graph_p;
 

pthread_mutex_t edge_mutex;     /*!< A mutex for edges */
pthread_mutex_t node_mutex;     /*!< Vertex mutex */
pthread_mutex_t graph_mutex;    /*!< Mutex for ne daj baze */


Gdb_ret_t addEdge(Graph_t*, const Gdb_N_t, const Gdb_N_t, Gdb_Payload_p);
Gdb_ret_t addGraphElement(Graph_t*, Gdb_Payload_p); // payload cant be null
Gdb_ret_t allMutexesLock();
Gdb_ret_t allMutexesUnLock();
Gdb_ret_t computeID(Graph_t*, Gdb_N_t*, Gdb_N_t*, const bool b);
Graph_p createGraph(const Gdb_graph_t, const char*);
void *createMemory(const size_t);
Node_p createNode(Gdb_N_t, void*);
Gdb_ret_t deleteGraphElement(Graph_t*, const Gdb_N_t); // only flag as deleted
Gdb_Nothing_t destroyEdges(Node_p);
Gdb_Nothing_t destroyGraph(Graph_p);

List_p getElementPointerByID(const Graph_p, const Gdb_N_t, const Gdb_N_t);
List_p getElementPointerByIndex(const Graph_p, const Gdb_N_t);

Gdb_ret_t initMutexes();

void *resizeMemory(void*, size_t);

#endif