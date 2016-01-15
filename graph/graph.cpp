/*graph.c*/
#ifndef _PRIMITIVES_H_
#include "primitives.h"
#endif

#include "graph.h"

pthread_mutex_t edge_mutex;     /*!< A mutex for edges */
pthread_mutex_t node_mutex;     /*!< Vertex mutex */
pthread_mutex_t graph_mutex;    /*!< Mutex for ne daj baze */

/* Adds an edge to a graph*/
Gdb_ret_t addEdge(Graph_t *graph_p, const Gdb_N_t src, const Gdb_N_t dest, Gdb_hr_t el_type){
    // check that we can do this
    if(graph_p->arr_list[src].vertex_status != ALLOCATED){
        // use this to our advantage to fill the cache
        graph_p->deleted_element = &(graph_p->arr_list[src]);
        return SRC_GONE;
    }
    if(graph_p->arr_list[dest].vertex_status != ALLOCATED){
        graph_p->deleted_element = &(graph_p->arr_list[dest]);
        return DST_GONE;
    }

    // LOCK
    pthread_mutex_lock(&edge_mutex);

    /* Add an edge from src to dst in the adjacency list*/
    Node_p new_node = createNode(dest,el_type);
    if (!new_node){
        pthread_mutex_unlock(&edge_mutex);
        return NO_NODE;
    }

    //addEdgeInPosition(graph_p->arr_list[dest].head,graph_p->arr_list[src].num_edges,new_node);
    new_node->next = graph_p->arr_list[src].head;
    graph_p->arr_list[src].head = new_node;
    graph_p->arr_list[src].num_edges++;

    if(graph_p->type == UNDIRECTED){
        /* Add an edge from dest to src also*/
        new_node = createNode(src,el_type);
        if (!new_node){
            pthread_mutex_unlock(&edge_mutex);
            return NO_NODE;
        }
        // find the correct place
        //addEdgeInPosition(graph_p->arr_list[dest].head,graph_p->arr_list[dest].num_edges,new_node);
        
        new_node->next =graph_p->arr_list[dest].head;
        graph_p->arr_list[dest].head = new_node;
        graph_p->arr_list[dest].num_edges++;
    }
    pthread_mutex_unlock(&edge_mutex);
    return OK;
}

// Add vertices in ordered position
Gdb_ret_t addEdgeInPosition(Node_t *head_node,Gdb_N_t total_edges,Node_t *new_node){
    Node_p tmp_node = head_node;

    if(total_edges == 0){
        head_node = new_node;
    }
    else{
        Node_p last_checked;
        bool found = false
        while((!found) && tmp_node){
            if(new_node->vertex <= tmp_node->vertex){

            }

            //3 -> 1 2 3 4 5 
        }
    }

    

    if(head_node == last_node){
        // single edge here add the new one
        if(new_node->vertex <= head_node->vertex){
            new_node->next = head_node;
            head_node = new_node;
        }
        else{
            head_node->next = new_node;
        }
    }
    else{
        Gdb_N_t current = total_edges / 2 ;
        bool found = false;
        while(!found){
            // split in half
            tmp_node = first_node + current;
            if(new_node->vertex < tmp_node->vertex){
                last_node = tmp_node;
                // check first == last
                if (first_node == last_node){
                    if(head_node == first_node){
                        // this means new node should be first
                        new_node->next = first_node;
                        head_node = new_node;
                        found = true;
                    }
                }
            }
        }
    }

}

Gdb_ret_t addGraphElement(Graph_t *graph, Gdb_hr_t el_type){
    // This will hold the index of the referenced allocation space
    Gdb_N_t i;
    // check next available id
    pthread_mutex_lock(&node_mutex);
    if(graph->next_available_id < graph->num_vertices){
        // We are ok ..allocate here
        i = graph->next_available_id;
        graph->next_available_id = graph->next_available_id + 1;
        // first check to make sure element isnt deleted
        if(graph->arr_list[i].vertex_status == DELETED){
            // this is a deleted vertex - reclaim its space
            graph->arr_list[i].vertex_status = RECLAIMED;
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
    graph->arr_list[i].vertex_status = ALLOCATED;
    graph->arr_list[i].type = el_type; // assign it e generic type for indexing
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
        graph->arr_list[i].vertex_status = DELETED;
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
        releaseMemory(graph);
        pthread_mutex_unlock(&graph_mutex);
        return NULL;
    }
    graph->type = type;
    graph->deleted_element = NULL;
    graph->total_deleted = 0;
    graph->next_available_id = 0;
    graph->needs_page_increase = false;
    graph->id_hi = 0;
    graph->id_lo = 0;
    graph->edhe_type = BST;

    pthread_mutex_lock(&node_mutex);
    for(int i = 0; i < MAX_PAGE_SIZE; i++){
        // initialize the list
        graph->arr_list[i].num_edges = 0;
        graph->arr_list[i].list_id = i;           // give it an id
        graph->arr_list[i].vertex_status = NEW;   // active cell
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
    graph->num_vertices += MAX_PAGE_SIZE; // inclrease vertex count without needing to count - keep it O(1)
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
Node_p createNode(Gdb_N_t vertex_id, Gdb_hr_t el_type){
    // newNode is a pointer
    Node_p new_node = (Node_p)createMemory(sizeof(Node_t));
    if(!new_node){
        return NULL;
    }
    // goes to which vertex (index)?
    new_node->vertex = vertex_id;
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->type = el_type; // generic type

    return new_node;
}

/* Leletes elements from the list */
Gdb_ret_t deleteGraphElement(Graph_t *graph, const Gdb_N_t element_index){
    if(element_index >= graph->num_vertices)
        return NO_INDEX;
    pthread_mutex_lock(&node_mutex);
    graph->arr_list[element_index].vertex_status = DELETED;
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
        releaseMemory(tmp);
    }
}

/*Destroys the graph*/
Gdb_Nothing_t destroyGraph(Graph_p graph){
    if(graph){
        if(graph->arr_list){
            Gdb_N_t v;
            /*Free up the nodes*/
            for (v = 0; v < graph->num_vertices; v++){
                destroyEdges(graph->arr_list[v].head);
            }
            /*Free the adjacency list array*/
            releaseMemory(graph->arr_list);
        }
        /*Free the graph*/
        releaseMemory(graph);
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

/* free memory */
void releaseMemory(void *ptr){
    free(ptr);
}

/* function used to realloc memory - in case we decide to reqrite that one day */
void *resizeMemory(void *ptr, size_t size){
    return realloc(ptr, size);
}