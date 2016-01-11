/*graph.h*/
#ifndef _GRAPH_H_
#define _GRAPH_H_
 
/* 
    Define a type to determine a type of graph
    This is the atomic level of the graph.

*/
typedef enum {UNDIRECTED=0, DIRECTED} usr_graph_type;
 
/* Adjacency list node */
typedef struct Node
{
    int vertex;                /*Index to adjacency list array*/
    struct Node *next; /*Pointer to the next node*/
}Node_t, *Node_p;
 
/* Adjacency list */
typedef struct List
{
    int num_members;            /*number of members in the list (for future use)*/
    Node_t *head;               /*head of the adjacency linked list*/
}List_t, *List_p;
 
/* Graph structure. A graph is an array of adjacency lists.
   Size of array will be number of vertices in graph*/
typedef struct Graph
{
    /* Define the type of graph */
    usr_graph_type type;      
    /* Number of vertices */
    int num_vertices;
    /* Array of adjacency lists */
    List_p arr_list;      
}Graph_t, *Graph_p;
 

void addEdge(Graph_t*, int, int);
Graph_p createGraph(int, usr_graph_type);
Node_p createNode(int);
void destroyGraph(Graph_p);
void displayGraph(Graph_p);

/* Exit function to handle a conditional exit due to error or other*/
__inline void cond_exit(const char* title, const char* msg)
{
    printf("[%s]: %s \nTerminating...\n", title, msg);
    exit(1);
}
 
#endif