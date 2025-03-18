#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// struktura węzła

typedef struct node {

    int Number;
    struct node *Connections;

} node_t;

// struktura grafu

typedef struct graph {

    int Height;
    int Width;

    node_t *Nodes;      // [*NODE0, *NODE1, *NODE2, *NODE3, (...)]
    int NodeIndexes;    // [3, 5, 6, (...)]

} graph_t;

// funkcja czytająca graf z pliku

graph_t *load_from_file (FILE *Stream) {
    return NULL;
}

// algorytm dzielący graf

graph_t **partition_graph (graph_t *Graph, int PartitionCount, double ErrorMargin) {
    return NULL;
}

// print graph

void print_graph (graph_t *Graph) {
    return;
}

// funkcje pomocnicze

// main

int main (int argc, char **argv) {
    return EXIT_SUCCESS;
}