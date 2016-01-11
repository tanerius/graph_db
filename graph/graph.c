/*graph.c*/
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

/* Function to create an adjacency list node*/
Node_p createNode(int v){
    // newNode is a pointer
    Node_p newNode = (Node_p)malloc(sizeof(Node_t));
    if(!newNode)
        cond_exit("Error", "Unable to allocate memory for new node");

    newNode->vertex = v;
    newNode->next = NULL;

    return newNode;
}

/* Function to create a graph with n vertices; Creates both directed and undirected graphs*/
Graph_p createGraph(int n, usr_graph_type type){
    int i;
    // graph will create a pointer
    Graph_p graph = (Graph_p)malloc(sizeof(Graph_t));
    if(!graph)
        cond_exit("Error", "Unable to allocate memory for graph");
    graph->num_vertices = n;
    graph->type = type;

    /* Create an array of adjacency lists*/
    graph->arr_list = (List_p)malloc(n * sizeof(List_t));
    if(!graph->arr_list)
        cond_exit("Error", "Unable to allocate memory for adjacency list array");

    for(i = 0; i < n; i++){
        graph->arr_list[i].head = NULL;
        graph->arr_list[i].num_members = 0;
    }

    return graph;
}

/*Destroys the graph*/
void destroyGraph(Graph_p graph){
    if(graph){
        if(graph->arr_list){
            int v;
            /*Free up the nodes*/
            for (v = 0; v < graph->num_vertices; v++){
                Node_p node_ptr = graph->arr_list[v].head;
                while (node_ptr){
                    Node_p tmp = node_ptr;
                    node_ptr = node_ptr->next;
                    free(tmp);
                }
            }
            /*Free the adjacency list array*/
            free(graph->arr_list);
        }
        /*Free the graph*/
        free(graph);
    }
}

/* Adds an edge to a graph*/
void addEdge(Graph_t *graph, int src, int dest){
    /* Add an edge from src to dst in the adjacency list*/
    Node_p newNode = createNode(dest);
    newNode->next = graph->arr_list[src].head;
    graph->arr_list[src].head = newNode;
    graph->arr_list[src].num_members++;

    if(graph->type == UNDIRECTED){
        /* Add an edge from dest to src also*/
        newNode = createNode(src);
        newNode->next = graph->arr_list[dest].head;
        graph->arr_list[dest].head = newNode;
        graph->arr_list[dest].num_members++;
    }
}
