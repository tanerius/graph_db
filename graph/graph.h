/*graph.h*/
#ifndef _GRAPH_H_
#define _GRAPH_H_
 
/* 
    Define a type to determine a type of graph
    This is the atomic level of the graph.

*/
typedef enum {UNDIRECTED=0, DIRECTED} usr_graph_type;
 
/* Adjacency list node */
typedef struct adjlist_node
{
    int vertex;                /*Index to adjacency list array*/
    struct adjlist_node *next; /*Pointer to the next node*/
}adjlist_node_t, *adjlist_node_p;
 
/* Adjacency list */
typedef struct adjlist
{
    int num_members;           /*number of members in the list (for future use)*/
    adjlist_node_t *head;      /*head of the adjacency linked list*/
}adjlist_t, *adjlist_p;
 
/* Graph structure. A graph is an array of adjacency lists.
   Size of array will be number of vertices in graph*/
typedef struct graph
{
    /* Define the type of graph */
    usr_graph_type type;      
    /* Number of vertices */
    int num_vertices;         
    adjlist_p adjListArr;     /*Adjacency lists' array*/
}graph_t, *graph_p;
 

void addEdge(graph_t *graph, int src, int dest);
graph_p createGraph(int n, usr_graph_type type);
adjlist_node_p createNode(int v);
void destroyGraph(graph_p graph);
void displayGraph(graph_p graph);

/* Exit function to handle a conditional exit due to error or other*/
__inline void cond_exit(const char* title, const char* msg)
{
    printf("[%s]: %s \nTerminating...\n", title, msg);
    exit(1);
}
 
#endif