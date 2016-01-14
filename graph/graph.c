/*graph.c*/

#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif

#include "graph.h"



/* Adds an edge to a graph*/
Gdb_ret_t addEdge(Graph_t *graph, const Gdb_N_t src, const Gdb_N_t dest, Gdb_Payload_p payload){
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

    // LOCK
    pthread_mutex_lock(&edge_mutex);

    /* Add an edge from src to dst in the adjacency list*/
    Node_p newNode = createNode(dest,payload);
    if (!newNode){
        pthread_mutex_unlock(&edge_mutex);
        return NO_NODE;
    }
    newNode->next = graph->arr_list[src].head;
    graph->arr_list[src].head = newNode;
    graph->arr_list[src].num_edges++;

    if(graph->type == UNDIRECTED){
        /* Add an edge from dest to src also*/
        newNode = createNode(src,payload);
        if (!newNode){
            pthread_mutex_unlock(&edge_mutex);
            return NO_NODE;
        }
        newNode->next = graph->arr_list[dest].head;
        graph->arr_list[dest].head = newNode;
        graph->arr_list[dest].num_edges++;
    }
    pthread_mutex_unlock(&edge_mutex);
    return OK;
}

Gdb_ret_t addGraphElement(Graph_t *graph, Gdb_Payload_p payload){
    // here payload must be valid
    if(payload == NULL){
        return BAD_PAYLOAD;
    }
    // This will hold the index of the referenced allocation space
    Gdb_N_t i;
    // check next available id
    pthread_mutex_lock(&node_mutex);
    if(graph->next_available_id < graph->num_vertices){
        // We are ok ..allocate here
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
            pthread_mutex_unlock(&node_mutex);
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
            pthread_mutex_unlock(&node_mutex);
            return PAGE_FULL;
        }
    }

    // DO ACTUAL ALLOCATION
    graph->arr_list[i].payload = payload;
    // clean memory for number of edges
    if(graph->arr_list[i].num_edges > 0){
        pthread_mutex_lock(&edge_mutex);
        for(Gdb_N_t j = 0; j < graph->arr_list[i].num_edges; j++){
            destroyEdges(graph->arr_list[i].head);
        }
        pthread_mutex_unlock(&edge_mutex);
    }
    graph->arr_list[i].num_edges = 0;
    graph->arr_list[i].head = NULL;
    
    Gdb_N_t id_hi;
    Gdb_N_t id_lo;
    Gdb_ret_t get_ids = computeID(graph, &id_hi, &id_lo, true);

    if(get_ids == OK){
        graph->arr_list[i].id_hi = id_hi;
        graph->arr_list[i].id_lo = id_lo;
    }
    else{
        graph->arr_list[i].is_deleted = true;
        graph->total_deleted = graph->total_deleted + 1;
        graph->deleted_element = &(graph->arr_list[i]);
    }
    pthread_mutex_unlock(&node_mutex);
    return get_ids;
}

Gdb_ret_t allMutexesLock(Graph_t *graph){
    /*
        Here we must also handle case where locking fails and in this case we must kill the entire application!

        EINVAL
            the mutex has not been properly initialized. 
        EDEADLK
            the mutex is already locked by the calling thread (â€˜â€˜error checkingâ€™â€™ mutexes only). 
        EOWNERDEAD
            the robust mutex is now locked by the calling thread after the previous owner terminated without unlocking it.
        ENOTRECOVERABLE
            the robust mutex is not locked and is no longer usable after the previous owner unlocked it without calling pthread_mutex_consistent.

            The pthread_mutex_trylock function returns the following error codes on error: 

        EBUSY
            the mutex could not be acquired because it was currently locked. 
        EINVAL
            the mutex has not been properly initialized. 
        EOWNERDEAD
            the robust mutex is now locked by the calling thread after the previous owner terminated without unlocking it.
        ENOTRECOVERABLE
            the robust mutex is not locked and is no longer usable after the previous owner unlocked it without calling pthread_mutex_consistent.
    */

    int r = pthread_mutex_lock(&edge_mutex);
    if(r != 0) return LOCK_ERR;
    r = pthread_mutex_lock(&node_mutex);
    if(r != 0){
        pthread_mutex_unlock(&edge_mutex);
        return LOCK_ERR;
    }
    r = pthread_mutex_lock(&graph_mutex);
    if(r != 0){
        pthread_mutex_unlock(&edge_mutex);
        pthread_mutex_unlock(&node_mutex);
        return LOCK_ERR;
    }
    graph->is_all_locked = true;
    // If this doesnt return OK then quit everything to prevent corruption
    return OK;
}

Gdb_ret_t allMutexesUnLock(Graph_t *graph){
    pthread_mutex_unlock(&edge_mutex);
    pthread_mutex_unlock(&node_mutex);
    pthread_mutex_unlock(&graph_mutex);
    graph->is_all_locked = false;
    return OK;
}

/* write the next ID of the node */
Gdb_ret_t computeID(Graph_t *graph, Gdb_N_t *hi, Gdb_N_t *lo, const bool do_update){
    // Make sure that this part is extra paranoid!
    Gdb_N_t tmp_hi = graph->id_hi;
    Gdb_N_t tmp_lo = graph->id_lo;
    if(tmp_lo < THEORETICAL_MAX){
        *(hi) = tmp_hi;
        *(lo) = tmp_lo + 1;
    }
    else{
        if(tmp_hi < THEORETICAL_MAX){
            *(hi) = tmp_hi+1;
            *(lo) = 0;
        }
        else{
            return MAX_NODES;
        }
    }
    if(do_update){
        // graph changes here... protect with a mutex
        pthread_mutex_lock(&graph_mutex);
        graph->id_hi = *(hi);
        graph->id_lo = *(lo);
        pthread_mutex_unlock(&graph_mutex);
    }
    return OK;
}

/* Function to create a graph with n vertices; Creates both directed and undirected graphs*/
Graph_p createGraph(const Gdb_graph_t type, const char *fn){
    // graph will create a pointer
    pthread_mutex_lock(&graph_mutex);
    Graph_p graph = (Graph_p)createMemory(sizeof(Graph_t));
    if(!graph)
        return NULL;
    // allocate memory for initial page
    graph->num_vertices = MAX_PAGE_SIZE;
    graph->type = type;
    graph->arr_list = (List_p)createMemory(MAX_PAGE_SIZE * sizeof(List_t));
    if(!graph->arr_list){
        free(graph);
        pthread_mutex_unlock(&graph_mutex);
        return NULL;
    }
    graph->type = type;
    graph->deleted_element = NULL;
    graph->total_deleted = 0;
    graph->next_available_id = 0;
    graph->needs_page_increase = false;
    graph->id_hi = 0;
    graph->id_hi = 0;

    pthread_mutex_lock(&node_mutex);
    for(int i = 0; i < MAX_PAGE_SIZE; i++){
        // initialize the list
        graph->arr_list[i].num_edges = 0;
        graph->arr_list[i].payload = NULL;        // We check this to see if we can allocate this element
        graph->arr_list[i].list_id = i;           // give it an id
        graph->arr_list[i].is_deleted = false;    // active cell
        graph->arr_list[i].head = NULL;           // This means no edges
    }
    pthread_mutex_unlock(&node_mutex);

    // Becomes true when all mutexes lock
    graph->is_all_locked = false;

    // allocate a filename 
    graph->db_filename = (char*)createMemory(strlen(fn) + 1);
    // add the terminator
    memset(graph->db_filename, '\0', strlen(fn));
    strcpy(graph->db_filename,fn);
    pthread_mutex_unlock(&graph_mutex);

    return graph;
}

/* Function used to create a new graph page for adjecent lists */
Gdb_ret_t createGraphPage(Graph_t *graph){
    // this will double memory usage while it does the copying!!!
    pthread_mutex_lock(&graph_mutex);
    pthread_mutex_lock(&node_mutex);
    List_p new_block = (List_p)resizeMemory(graph->arr_list, (graph->num_vertices + MAX_PAGE_SIZE)*sizeof(List_t));
    if(!new_block){
        pthread_mutex_unlock(&graph_mutex);
        pthread_mutex_unlock(&node_mutex);
        return MEM_FULL;
    }
    graph->arr_list = new_block;
    pthread_mutex_unlock(&graph_mutex);
    pthread_mutex_unlock(&node_mutex);
    return OK;
}

void *createMemory(const size_t size){
    // Just in case we will one day override malloc
    return malloc(size);
}

/* Function to create an adjacency list node*/
// NEVER CALL DIRECTLY OUTSIDE THIS FILE - IT IS NOT THREAD-SAFE ON ITS OWN!!!
Node_p createNode(Gdb_N_t vertex_id, void *p){
    // newNode is a pointer
    Node_p new_node = (Node_p)createMemory(sizeof(Node_t));
    if(!new_node){
        return NULL;
    }
    // goes to which vertex (index)?
    new_node->vertex = vertex_id;
    new_node->next = NULL;
    new_node->payload = p;
    new_node->type = 0; // generic type

    return new_node;
}

/* Leletes elements from the list */
Gdb_ret_t deleteGraphElement(Graph_t *graph, const Gdb_N_t element_index){
    if(element_index >= graph->num_vertices)
        return NO_INDEX;
    pthread_mutex_lock(&node_mutex);
    graph->arr_list[element_index].is_deleted = true;
    if(graph->arr_list[element_index].num_edges > 0){
        pthread_mutex_lock(&edge_mutex);
        for(Gdb_N_t j = 0; j < graph->arr_list[element_index].num_edges; j++){
            destroyEdges(graph->arr_list[element_index].head);
        }
        pthread_mutex_unlock(&edge_mutex);
    }
    pthread_mutex_unlock(&node_mutex);
    return OK;
}

/* Destroys edges */
// NEVER CALL DIRECTLY OUTSIDE THIS FILE- IT IS NOT THREAD-SAFE ON ITS OWN!!!
Gdb_Nothing_t destroyEdges(Node_p n){
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
Gdb_Nothing_t destroyGraph(Graph_p graph){
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

/* get an element/node of the graph given its id offset and position */
List_p getElementPointerByID(const Graph_p graph, const Gdb_N_t hi, const Gdb_N_t lo){
    return NULL;
}

/* Return a pointer to an element/node of the graph */
List_p getElementPointerByIndex(const Graph_p graph, const Gdb_N_t index){
    if(index < graph->num_vertices){
        return &(graph->arr_list[index]);
    }
    else{
        return NULL;
    }
    
}

Gdb_ret_t initMutexes(){
    if (pthread_mutex_init(&edge_mutex, NULL) != 0)
    {
        return LOCK_ERR;
    }
    if (pthread_mutex_init(&node_mutex, NULL) != 0)
    {
        return LOCK_ERR;
    }
    if (pthread_mutex_init(&graph_mutex, NULL) != 0)
    {
        return LOCK_ERR;
    }
    return OK;
}

/* function used to realloc memory - in case we decide to reqrite that one day */
void *resizeMemory(void *ptr, size_t size){
    return realloc(ptr, size);
}