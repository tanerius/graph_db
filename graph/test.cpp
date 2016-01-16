#include <assert.h>
#include "graph.h"
#include "graphfiles.h"

/* Function to print the adjacency list of graph*/
void displayGraph(Graph_p graph)
{
    Gdb_N_t i;
    for (i = 0; i < graph->num_vertices; i++)
    {
        Node_p node_ptr = graph->arr_list[i].head;
        printf("\nV(%d) [", (int)i);
        while (node_ptr)
        {
            printf("%d<%d>", (int)node_ptr->vertex,(int)node_ptr->type); // bad idea to cast but only for testing to prevent warnings
            node_ptr = node_ptr->next;
            if(node_ptr) printf(", ");
        }
        printf("]");

    }
}

void displayEdgeInfo(Node_p node_ptr){
    printf("\nNode(): ");
    printf("\n   To Vertex: %d",(int)node_ptr->vertex);
    printf("\n   Type: %d\n",(int)node_ptr->type);
}

typedef struct custom_payload {
        int x;
        bool b;
        char c;
    } Payload, *Payload_p;
    

void test_creatememory(){
    printf("\nTesting createMemory(): ");
    unsigned int s = sizeof(char);
    char*x = (char*)createMemory(5 * s);
    strcpy(x,"test");
    printf("\nExpected length:%d",(5*s));
    printf("\nReceived length:%d\n",(int)(strlen(x)+1));
    assert((strlen(x)+1) == (5*s));
    printf("PASS\n");

    printf("\nTesting releaseMemory(): ");
    releaseMemory(x);
    printf("PASS");
}

Graph_p test_createGraph(Gdb_graph_t t){
    Graph_p p;
    if(t==UNDIRECTED){
        printf("\nTesting createGraph(UNDIRECTED): ");
        p = createGraph(t,"./default_u_db.gdb");
    }
    else{
        printf("\nTesting createGraph(DIRECTED): ");
        p = createGraph(t,"./default_d_db.gdb");
    }
    assert(p != NULL);
    printf("PASS");
    return p;
}

void test_addGraphElement(Graph_t *graph, Gdb_hr_t el_type, int how_many){
    Gdb_ret_t result;
    printf("\nTesting addGraphElement() x %d: ",how_many);
    for(int i =0; i<how_many; i++){
        result = addGraphElement(graph, el_type);
        assert((result == OK) || (result==PAGE_FULL));
        if(result == OK){
            printf(".");
        }
        else{
            printf("x");
        }
    }
    printf(": PASS");
}

void test_addEdge(Graph_t *graph_p, const Gdb_N_t src, const Gdb_N_t dest, Gdb_hr_t el_type){
    printf("\nTesting addEdge(from=%d,to=%d,type=%d): ",(int)src, (int)dest,el_type);
    Gdb_ret_t result;
    result = addEdge(graph_p, src, dest, el_type);
    assert(result == OK);
    printf("PASS");
}

// test for creating new pages
void test_createGraphPage(Graph_t *graph){
    printf("\nTesting createGraphPage(): ");
    Gdb_ret_t result; 
    result = createGraphPage(graph);
    assert(result == OK);
    printf("PASS");
}


int main()
{
    printf("Starting tests...\n");
    test_creatememory();
    Graph_p undir_graph = test_createGraph(UNDIRECTED);
    test_addGraphElement(undir_graph, 0, 26); // 20 should pass 6 should require memory reallocation
    test_addEdge(undir_graph, 0, 1, 1);
    test_addEdge(undir_graph, 0, 4, 2);
    test_addEdge(undir_graph, 1, 2, 3);
    test_addEdge(undir_graph, 1, 3, 4);
    test_addEdge(undir_graph, 1, 4, 5);
    test_addEdge(undir_graph, 2, 3, 6);
    test_addEdge(undir_graph, 3, 4, 7);
    test_createGraphPage(undir_graph);
    test_addGraphElement(undir_graph, 1, 6); // Add 6 more - should pass now
    test_addEdge(undir_graph, 4, 0, 8);
    test_addEdge(undir_graph, 13, 0, 9);
    test_addEdge(undir_graph, 13, 0, 10);
    test_addEdge(undir_graph, 5, 0, 11);
    test_addEdge(undir_graph, 6, 0, 12);
    test_addEdge(undir_graph, 7, 0, 13);
    test_addEdge(undir_graph, 8, 0, 14);
    test_addEdge(undir_graph, 9, 0, 15);
    test_addEdge(undir_graph, 13, 0, 9); // duplicate
    test_addEdge(undir_graph, 10, 0, 16);
    test_addEdge(undir_graph, 11, 0, 17);
    test_addEdge(undir_graph, 13, 0, 9); // duplicate
    test_addEdge(undir_graph, 12, 0, 18);

    //get edges of Node(0)
    

    
    
/*
    Graph_p dir_graph = createGraph(DIRECTED,"./default_d_db.gdb");
    assert(dir_graph != NULL);
    result = addGraphElement(dir_graph, &(c[0]));
    assert(result == OK);
    result = addGraphElement(dir_graph, &(c[1]));
    assert(result == OK);
    result = addGraphElement(dir_graph, &(c[2]));
    assert(result == OK);
    result = addGraphElement(dir_graph, &(c[3]));
    assert(result == OK);
    result = addGraphElement(dir_graph, &(c[4]));
    assert(result == OK);

    result = addEdge(dir_graph, 0, 1, NULL);
    assert(result == OK);
    result = addEdge(dir_graph, 0, 4, NULL);
    assert(result == OK);
    result = addEdge(dir_graph, 1, 2, NULL);
    assert(result == OK);
    result = addEdge(dir_graph, 1, 3, NULL);
    assert(result == OK);
    result = addEdge(dir_graph, 1, 4, NULL);
    assert(result == OK);
    result = addEdge(dir_graph, 2, 3, NULL);
    assert(result == OK);
    result = addEdge(dir_graph, 3, 4, NULL);
    assert(result == OK);
*/
    printf("\nUNDIRECTED GRAPH");
    displayGraph(undir_graph);
    destroyGraph(undir_graph);
    
    //printf("\nDIRECTED GRAPH");
    //displayGraph(dir_graph);
    //destroyGraph(dir_graph);

    return 0;
}