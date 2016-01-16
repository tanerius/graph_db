#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

/**
 * Universal stuff used almost everywhere.
 * All explicit graphdb variables will be prefixed with Gdb_
 */
#define TRUE        1                   /* Custom true */
#define FALSE       0                   /* Custom false */
#define MAX_PAGE_SIZE            20     /* MAX_PAGE_SIZE defines how much should the adjacency list size grow/page */
#define VERSION_MAJOR             1     /* Major version */
#define VERSION_MINOR             0     /* Minor version */
#define THEORETICAL_MAX     1000000     /* Maximums used in generating IDs for nodes */
#define GDB_ENGINE_MEMORY         0     /* Memory engine type */
 


/*! 
    Formerly usr_graph_type
    This enum type is used to determine the type of the graph
*/
typedef enum {
    UNDIRECTED=0, 
    DIRECTED
} Gdb_graph_t;

/*! 
    Type definition for the engine type
*/
typedef int Gdb_engine_t;

/*! 
    This enum type is used to determine the status of a node
*/
typedef enum {
    NEW=0, 
    ALLOCATED,
    DELETED,
    RECLAIMED
} Gdb_node_status_t;

/*! 
    This enum type is used to define the default edge type
*/
typedef enum {
    ULL=0,                  /* Unsorted linked list - fastest insertion slow read */
    SLL,                    /* Sorted linked list - probably useless but ok to have*/
    BST                     /* Balanced search tree - default fastest read O(log n) insertion */
} Gdb_edge_sort_t;

/*! 
    Formerly usr_edge_type
    This type is used to determine an index for the type of entity.
    Int will then be mapped to human readable stuff.
*/
typedef int Gdb_hr_t;

/*! 
    Formerly usr_ret_type
    This enum type is used to define all the possible return types for anything.
    At some point we should map these to meaningful human readable messages.
*/
typedef enum {
    OK=0,                   /* ok */
    BAD_PAYLOAD,            /* illegal payload was provided */
    PAGE_FULL,              /* the adjacency array is full realloc it */
    SRC_GONE,               /* the source element has been deleted or isnt assigned */
    DST_GONE,               /* the destination element has been deleted or isnt assigned */
    MEM_FULL,               /* memory could not be allocated */
    NO_INDEX,               /* invalid index when referencing something with an index */
    NO_NODE,                /* cannot allocate memory for node */
    MAX_NODES,              /* maximum elements reached - you're NUTS! */
    INVALID_SERIAL,         /* The serializer is not in proper state */
    FILE_I_ERROR,           /* There was an error trying to write the file  */
    FILE_O_ERROR,           /* There was an error trying to read the file  */
    FILE_ERROR,             /* There was an error trying to open the file  */
    LOCK_ERR,               /* Mutexes failed!!! VERY VERY BAD  */
    PROC_IDLE,              /* Thread/process control. Process is idle.  */
    PROC_OK,                /* Thread/process control. Process finished ok.  */
    PROC_RUNNING,           /* Thread/process control. Process running.  */
    TH_C_ERROR,             /* Cannot create a thread.  */
    EDGE_DUPLICATE          /* Did not add edge since it exists.  */
} Gdb_ret_t;

/*! 
    Generic representation of a pointer to payload
*/
typedef void Gdb_Nothing_t, *Gdb_Payload_p;

/*! 
    Formerly big_number and *big_number_p
    Natural (N) numbers representation
*/
typedef unsigned long Gdb_N_t,*Gdb_n_p;

/*! 
    Full (Z) numbers representation
*/
typedef long Gdb_Z_t,*Gdb_z_p;

/*! 
    1 Byte representation. Cross your fingers and hope its 8bits or wont work.
    TODO: check out different representations too.
*/
typedef unsigned char Gdb_byte_t;

#endif