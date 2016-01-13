/*graph.h*/
#ifndef _GRAPH_H_
#define _GRAPH_H_
 
/* 
    Define useful stuff.
*/
#define TRUE        1
#define FALSE       0
// MAX_PAGE_SIZE defines by how much should the adjacency list size grow once it reaches max size
#define MAX_PAGE_SIZE            20
#define VERSION_MAJOR             1
#define VERSION_MINOR             0
#define THEORETICAL_MAX     1000000 // theoretical maxumum for some stuff

// Main typoes of graphs. THese cannot mix
typedef enum {UNDIRECTED=0, DIRECTED} usr_graph_type;

typedef enum {
    HAS=0,
    IS,
    C1,
    C2,
    C3
} usr_edge_type;

/* all the return types ever */
typedef enum {
    OK=0,                   /* ok */
    BAD_PAYLOAD,            /* illegal payload was provided */
    PAGE_FULL,              /* the adjacency array is full realloc it */
    SRC_GONE,               /* the source element has been deleted or isnt assigned */
    DST_GONE,               /* the destination element has been deleted or isnt assigned */
    MEM_FULL,               /* memory could not be allocated */
    NO_INDEX,               /* invalid index when referencing something with an index */
    NO_NODE,                /* cannot allocate memory for node */
    MAX_NODES               /* maximum elements reached - you're NUTS! */
} usr_ret_type;


// define the apsolute maximum number container
typedef unsigned long big_number,*big_number_p;
 
/* Adjacency list node */
typedef struct Node
{
    /*
        The id of the vertex that this edge is pointing to.
    */
    big_number vertex;
    usr_edge_type type;
    /* 
        WARNING:
        payload MUST be destroyed by caller!!!!
    */
    void *payload;
    /*Pointer to the next node*/
    struct Node *next; 
}Node_t, *Node_p;
 
/* Adjacency list */
typedef struct List
{
    big_number id_hi;                   /* hi order id */
    big_number id_lo;                   /* low order id */
    big_number num_edges;             /* number of edges */
    big_number list_id;                 /* unique id of list - incremental*/
    void *payload;                      /* List payload */
    bool is_deleted;                     /* should be purged on garbage collection */
    Node_t *head;                       /*head of the adjacency linked list is the actual element */
}List_t, *List_p;
 
/* Graph structure. A graph is an array of adjacency lists.
   Size of array will be number of vertices in graph*/
typedef struct Graph
{
    /* last id assigned */
    big_number id_hi;                   /* hi order id - for counting */
    big_number id_lo;                   /* low order id - for counting */

    /* for multiple threads access */
    bool is_safe_for_access;

    /* filename where graph is stored for persistence */
    char *db_filename;

    /* Define the type of graph */
    usr_graph_type type;      
    /* Number of vertices */
    big_number num_vertices;
    /* Total number of deleted elements */
    big_number total_deleted;
    // next free id to allocate with an element
    big_number next_available_id;
    /* Pointer to a deleted element in adjacency list (caching optimization) */
    List_p deleted_element;
    /* Array of adjacency lists */
    List_p arr_list;
    /* Becomes true when graph needs to grow its element page */
    bool needs_page_increase;
}Graph_t, *Graph_p;
 

usr_ret_type addEdge(Graph_t*, const big_number, const big_number, void*);
usr_ret_type addGraphElement(Graph_t*, void*); // payload cant be null
usr_ret_type computeID(Graph_t*, big_number*, big_number*, const bool b);
Graph_p createGraph(const usr_graph_type, const char*);
void *createMemory(const size_t);
Node_p createNode(big_number, void*);
usr_ret_type deleteGraphElement(Graph_t*, const big_number); // only flag as deleted
void destroyEdges(Node_p);
void destroyGraph(Graph_p);

List_p getElementPointerByID(const Graph_p, const big_number, const big_number);
List_p getElementPointerByIndex(const Graph_p, const big_number);


void *resizeMemory(void*, size_t);

#endif