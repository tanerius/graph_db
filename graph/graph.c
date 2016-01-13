/*graph.c*/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <endian.h>
#include "graph.h"


/* Adds an edge to a graph*/
usr_ret_type addEdge(Graph_t *graph, big_number src, big_number dest, void *payload){
    // check that we can do this
    if(graph->arr_list[src].is_deleted){
        // use this to our advantage to fill the cache
        graph->deleted_element = &(graph->arr_list[src]);
        return SRC_GONE;
    }
    if(graph->arr_list[src].payload == NULL)
        return SRC_GONE;
    if(graph->arr_list[dest].is_deleted){
        graph->deleted_element = &(graph->arr_list[dest]);
        return DST_GONE;
    }
    if(graph->arr_list[dest].payload == NULL)
        return DST_GONE;

    /* Add an edge from src to dst in the adjacency list*/
    Node_p newNode = createNode(dest,payload);
    if (!newNode){
        return NO_NODE;
    }
    newNode->next = graph->arr_list[src].head;
    graph->arr_list[src].head = newNode;
    graph->arr_list[src].num_edges++;

    if(graph->type == UNDIRECTED){
        /* Add an edge from dest to src also*/
        newNode = createNode(src,payload);
        if (!newNode){
            return NO_NODE;
        }
        newNode->next = graph->arr_list[dest].head;
        graph->arr_list[dest].head = newNode;
        graph->arr_list[dest].num_edges++;
    }
    return OK;
}

usr_ret_type addGraphElement(Graph_t *graph, void *payload){
    // here payload must be valid
    if(payload == NULL){
        return BAD_PAYLOAD;
    }
    // This will hold the index of the referenced allocation space
    big_number i;
    // check next available id
    if(graph->next_available_id < graph->num_vertices){
        // are are ok ..allocate here
        i = graph->next_available_id;
        graph->next_available_id = graph->next_available_id + 1;
        // first check to make sure element isnt deleted
        if(graph->arr_list[i].is_deleted){
            // this is a deleted vertex - reclaim its space
            graph->arr_list[i].is_deleted = false;
            graph->total_deleted = graph->total_deleted - 1;
            if(&(graph->arr_list[i]) == graph->deleted_element){
                // if this is the referenced deleted element then clear the reference
                // since we are reallocating it. 
                // TODO: Maybe try to find a new deleted if there is one!
                graph->deleted_element = NULL;
            }
        }
    }
    else{
        // no space left. Check for deleted or set graph->needs_page_increase = true
        if(graph->total_deleted == 0){
            // no deleted elements either set flag for page increase
            graph->needs_page_increase = true;
            return PAGE_FULL;
        }

        // ok we have deleted. lets see if we get lucky with the cache
        if(graph->deleted_element != NULL){
            // we got lucky
            graph->total_deleted = graph->total_deleted - 1;
            i = graph->deleted_element->list_id;
            graph->deleted_element = NULL;
        }
        else{
            // dont search since search space could be huge and take a long time
            // just flag 
            graph->needs_page_increase = true;
            return PAGE_FULL;
        }
    }

    // DO ACTUAL ALLOCATION
    graph->arr_list[i].payload = payload;
    // clean memory for number of edges
    if(graph->arr_list[i].num_edges > 0){
        for(big_number j = 0; j < graph->arr_list[i].num_edges; j++){
            destroyEdges(graph->arr_list[i].head);
        }
    }
    graph->arr_list[i].num_edges = 0;
    graph->arr_list[i].head = NULL;
    return OK;
}

/* Function to create a graph with n vertices; Creates both directed and undirected graphs*/
Graph_p createGraph(usr_graph_type type){
    // graph will create a pointer
    Graph_p graph = (Graph_p)createMemory(sizeof(Graph_t));
    if(!graph)
        return NULL;
    // allocate memory for initial page
    graph->num_vertices = MAX_PAGE_SIZE;
    graph->type = type;
    graph->arr_list = (List_p)createMemory(MAX_PAGE_SIZE * sizeof(List_t));
    if(!graph->arr_list){
        free(graph);
        return NULL;
    }
    graph->deleted_element = NULL;
    graph->total_deleted = 0;
    graph->next_available_id = 0;
    graph->needs_page_increase = false;

    for(int i = 0; i < MAX_PAGE_SIZE; i++){
        // initialize the list
        graph->arr_list[i].num_edges = 0;
        graph->arr_list[i].payload = NULL;        // We check this to see if we can allocate this element
        graph->arr_list[i].list_id = i;           // give it an id
        graph->arr_list[i].is_deleted = false;    // active cell
        graph->arr_list[i].head = NULL;           // This means no edges
    }

    return graph;
}

/* Function used to create a new graph page for adjecent lists */
usr_ret_type createGraphPage(Graph_t *graph){
    // this will double memory usage while it does the copying!!!
    List_p new_block = (List_p)resizeMemory(graph->arr_list, (graph->num_vertices + MAX_PAGE_SIZE)*sizeof(List_t));
    if(!new_block)
        return MEM_FULL;
    graph->arr_list = new_block;
    return OK;
}

void *createMemory(size_t size){
    // Just in case we will one day override malloc
    return malloc(size);
}

/* Function to create an adjacency list node*/
Node_p createNode(big_number vertex_id, void *p){
    // newNode is a pointer
    Node_p new_node = (Node_p)createMemory(sizeof(Node_t));
    if(!new_node){
        return NULL;
    }
    // goes to which vertex (index)?
    new_node->vertex = vertex_id;
    new_node->next = NULL;
    new_node->payload = p;
    new_node->type = C1; // generic type

    return new_node;
}

/* Leletes elements from the list */
usr_ret_type deleteGraphElement(Graph_t *graph, big_number element_index){
    if(element_index >= graph->num_vertices)
        return NO_INDEX;
    graph->arr_list[element_index].is_deleted = true;
    if(graph->arr_list[element_index].num_edges > 0){
        for(big_number j = 0; j < graph->arr_list[element_index].num_edges; j++){
            destroyEdges(graph->arr_list[element_index].head);
        }
    }
    return OK;
}

/* Destroys edges */
void destroyEdges(Node_p n){
    Node_p node_ptr = n;
    while (node_ptr){
        Node_p tmp = node_ptr;
        node_ptr = node_ptr->next;
        // just to be safe - payload MUST be taken care of by caller!!!
        tmp->payload = NULL;
        free(tmp);
    }
}

/*Destroys the graph*/
void destroyGraph(Graph_p graph){
    if(graph){
        if(graph->arr_list){
            int v;
            /*Free up the nodes*/
            for (v = 0; v < graph->num_vertices; v++){
                destroyEdges(graph->arr_list[v].head);
            }
            /*Free the adjacency list array*/
            free(graph->arr_list);
        }
        /*Free the graph*/
        free(graph);
    }
}

/* function used to realloc memory - in case we decide to reqrite that one day */
void *resizeMemory(void *ptr, size_t size){
    return realloc(ptr, size);
}