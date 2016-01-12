#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "graph.h"

/* Function to print the adjacency list of graph*/
void displayGraph(Graph_p graph)
{
    int i;
    for (i = 0; i < graph->num_vertices; i++)
    {
        Node_p node_ptr = graph->arr_list[i].head;
        printf("\n%d: ", i);
        while (node_ptr)
        {
            printf("%d->", node_ptr->vertex);
            node_ptr = node_ptr->next;
        }
        printf("NULL\n");
    }
}

typedef struct custom_payload {
        int x;
        bool b;
        char c;
    } Payload, *Payload_p;
    


int main()
{
    Payload_p c = (Payload_p)createMemory(5 * sizeof(Payload));
    assert(c != NULL);

    c[0].x=1;
    c[0].b=true;
    c[0].c='a';
    c[1].x=1;
    c[1].b=true;
    c[1].c='a';
    c[2].x=1;
    c[2].b=true;
    c[2].c='a';
    c[3].x=1;
    c[3].b=true;
    c[3].c='a';
    c[4].x=1;
    c[4].b=true;
    c[4].c='a';

    usr_ret_type result;

    Graph_p undir_graph = createGraph(UNDIRECTED);
    assert(undir_graph != NULL);
    result = addGraphElement(undir_graph, &(c[0]));
    assert(result == OK);
    result = addGraphElement(undir_graph, &(c[1]));
    assert(result == OK);
    result = addGraphElement(undir_graph, &(c[2]));
    assert(result == OK);
    result = addGraphElement(undir_graph, &(c[3]));
    assert(result == OK);
    result = addGraphElement(undir_graph, &(c[4]));
    assert(result == OK);
    
    result = addEdge(undir_graph, 0, 1, NULL);
    assert(result == OK);
    result = addEdge(undir_graph, 0, 4, NULL);
    assert(result == OK);
    result = addEdge(undir_graph, 1, 2, NULL);
    assert(result == OK);
    result = addEdge(undir_graph, 1, 3, NULL);
    assert(result == OK);
    result = addEdge(undir_graph, 1, 4, NULL);
    assert(result == OK);
    result = addEdge(undir_graph, 2, 3, NULL);
    assert(result == OK);
    result = addEdge(undir_graph, 3, 4, NULL);
    assert(result == OK);
    

    Graph_p dir_graph = createGraph(DIRECTED);
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
    
    printf("\nUNDIRECTED GRAPH");
    displayGraph(undir_graph);
    destroyGraph(undir_graph);
    
    printf("\nDIRECTED GRAPH");
    displayGraph(dir_graph);
    destroyGraph(dir_graph);

    return 0;
}