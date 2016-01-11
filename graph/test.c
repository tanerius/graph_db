#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

/* Function to print the adjacency list of graph*/
void displayGraph(graph_p graph)
{
    int i;
    for (i = 0; i < graph->num_vertices; i++)
    {
        adjlist_node_p adjListPtr = graph->adjListArr[i].head;
        printf("\n%d: ", i);
        while (adjListPtr)
        {
            printf("%d->", adjListPtr->vertex);
            adjListPtr = adjListPtr->next;
        }
        printf("NULL\n");
    }
}
    
int main()
{
    graph_p undir_graph = createGraph(5, UNDIRECTED);
    graph_p dir_graph = createGraph(5, DIRECTED);
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