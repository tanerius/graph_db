#include <stdio.h>
#include <stdlib.h>
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
    
int main()
{
    Graph_p undir_graph = createGraph(5, UNDIRECTED);
    Graph_p dir_graph = createGraph(5, DIRECTED);
    addEdge(undir_graph, 0, 1);
    addEdge(undir_graph, 0, 4);
    addEdge(undir_graph, 1, 2);
    addEdge(undir_graph, 1, 3);
    addEdge(undir_graph, 1, 4);
    addEdge(undir_graph, 2, 3);
    addEdge(undir_graph, 3, 4);
    
    addEdge(dir_graph, 0, 1);
    addEdge(dir_graph, 0, 4);
    addEdge(dir_graph, 1, 2);
    addEdge(dir_graph, 1, 3);
    addEdge(dir_graph, 1, 4);
    addEdge(dir_graph, 2, 3);
    addEdge(dir_graph, 3, 4);
    
    printf("\nUNDIRECTED GRAPH");
    displayGraph(undir_graph);
    destroyGraph(undir_graph);
    
    printf("\nDIRECTED GRAPH");
    displayGraph(dir_graph);
    destroyGraph(dir_graph);

    return 0;
}