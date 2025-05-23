#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

// struktura węzła

typedef struct node {

    int Number;
    struct node **Connections;

    // do Dijkstry
    int Visited;
    int *Distances;

} node_t;

// struktura grafu

typedef struct graph {

    int Height;
    int Width;

    node_t *Nodes;      // [*NODE0, *NODE1, *NODE2, *NODE3, (...)]
    int *NodeIndexes;    // [3, 5, 6, (...)]

} graph_t;

graph_t *new_graph (int Height, int Width, node_t *Nodes, int *NodeIndexes) {

    graph_t *NewGraph = malloc(sizeof(graph_t));

    NewGraph->Height = Height;
    NewGraph->Width = Width;
    NewGraph->Nodes = Nodes;
    NewGraph->NodeIndexes = NodeIndexes;

    return NewGraph;
}

node_t new_node (int Number, node_t **Connections) {
    node_t NewNode;
    
    NewNode.Number = Number;
    NewNode.Connections = Connections;

    NewNode.Visited = 0;
    int *Distances = malloc (128*sizeof(int));
    for (int i = 0; i < 128; i++) {
        Distances[i] = 999999;
    }
    NewNode.Distances = Distances;

    return NewNode;
}

void free_graph (graph_t *Graph) {
    for (int i = 0; i < (int) sizeof(Graph->NodeIndexes)/sizeof(int); i++) {
        free(Graph->Nodes[i].Connections);
        free(Graph->Nodes[i].Distances);
    }
    free(Graph->Nodes);
    free(Graph);
}

// print graph

void print_graph (graph_t *Graph) {
    int NodeCount = 0;
    if (Graph == NULL){
        printf("Something is wrong with graph!");
    }

    for (int i = 0; i < Graph->Width; i++) {
        printf("+ -- ");
    }
    printf("+\n");

    for (int j = 0; j < Graph->Height; j++) { 
        for (int k = 0; k < Graph->Width; k++){
            if (Graph->NodeIndexes[NodeCount] == j*(Graph->Width) + k){
                printf("|%3d ", Graph->Nodes[NodeCount].Number);
                NodeCount++; 
            } else {
                printf("|    ");
            }
            if (k == Graph->Width-1){
                printf("|\n");
                for (int l = 0; l < Graph->Width; l++) {
                    printf("+ -- ");
                }
                printf("+\n");
            }
        }
    }
    for (int i = 0; i < 105; i++) {
        int j = 0;
        while(Graph->Nodes[i].Connections != NULL && Graph->Nodes[i].Connections[j] != NULL) {
            printf("%d -> %d\n", Graph->Nodes[i].Number, Graph->Nodes[i].Connections[j]->Number);
            j++;
        }
    } 
}

// funkcja czytająca graf z pliku
graph_t *load_from_file (FILE *Stream) {

    if (Stream == NULL) {
        fprintf(stderr, "[!] Error: Can't open file!\n");
        return NULL;
    }

    char Width[64];                 // max 64B
    char NodeIndexes[65536];        // max 64kB
    char FirstIndexes[65536];
    char Connections[65536];
    char ConnectionIndexes[65536];

    fgets(Width, 64, Stream);
    fgets(NodeIndexes, 65536, Stream);
    fgets(FirstIndexes, 65536, Stream);
    fgets(Connections, 65536, Stream);
    fgets(ConnectionIndexes, 65536, Stream);

    int GraphWidth = atoi(Width);

    int GraphNodeIndexes[65536];

    char *NodeIndex = NodeIndexes;
    int CurrentIndex = 0;
    while (*NodeIndex) {
        if (isdigit(*NodeIndex)) {
            long Value = strtol(NodeIndex, &NodeIndex, 10);
            GraphNodeIndexes[CurrentIndex] = Value;
            CurrentIndex++;
        } else {
            NodeIndex++;
        }
    }
    GraphNodeIndexes[CurrentIndex] = -1;

    int GraphFirstIndexes[65536];

    char *FirstIndex = FirstIndexes;
    CurrentIndex = 0;
    while (*FirstIndex) {
        if (isdigit(*FirstIndex)) {
            long Value = strtol(FirstIndex, &FirstIndex, 10);
            GraphFirstIndexes[CurrentIndex] = Value;
            CurrentIndex++;
        } else {
            FirstIndex++;
        }
    }
    GraphFirstIndexes[CurrentIndex] = -1;

    int GraphConnections[65536];

    char *Connection = Connections;
    CurrentIndex = 0;
    while (*Connection) {
        if (isdigit(*Connection)) {
            long Value = strtol(Connection, &Connection, 10);
            GraphConnections[CurrentIndex] = Value;
            CurrentIndex++;
        } else {
            Connection++;
        }
    }
    GraphConnections[CurrentIndex] = -1;

    int GraphConnectionIndexes[65536];

    char *ConnectionIndex = ConnectionIndexes;
    CurrentIndex = 0;
    while (*ConnectionIndex) {
        if (isdigit(*ConnectionIndex)) {
            long Value = strtol(ConnectionIndex, &ConnectionIndex, 10);
            GraphConnectionIndexes[CurrentIndex] = Value;
            CurrentIndex++;
        } else {
            ConnectionIndex++;
        }
    }
    GraphConnectionIndexes[CurrentIndex] = -1;
    
    node_t *NewGraphNodes = malloc(65536 * sizeof(node_t));
    int NewGraphNodeIndexes[65536];
    
    int CurrentNodeIndex = 0;
    int CurrentNodeNumber = 0;
    int NewGraphHeight = 0;
    while(GraphFirstIndexes[NewGraphHeight] != -1) {
        if (GraphFirstIndexes[NewGraphHeight + 1] == GraphFirstIndexes[NewGraphHeight]) {
            NewGraphHeight++;
            continue;
        }
        while(GraphFirstIndexes[NewGraphHeight + 1] != CurrentNodeNumber && GraphFirstIndexes[NewGraphHeight + 1] != -1) {
            NewGraphNodes[CurrentNodeNumber] = new_node(CurrentNodeNumber, NULL);
            NewGraphNodeIndexes[CurrentNodeNumber] = GraphNodeIndexes[CurrentNodeIndex] + (NewGraphHeight * GraphWidth);
            CurrentNodeIndex++;
            CurrentNodeNumber++;
        }
        NewGraphNodeIndexes[CurrentNodeNumber] = -1;
        NewGraphHeight++;
    }
    NewGraphHeight--;
    
    graph_t *NewGraph = new_graph(NewGraphHeight, GraphWidth, NewGraphNodes, NewGraphNodeIndexes);

    int GraphConnectionsIterator = 1;
    int GraphConnectionIndexesIterator = 0;

    while (GraphConnectionIndexes[GraphConnectionIndexesIterator] != -1) {
        int CurrentNodeToMakeConnectionsFor = GraphConnections[GraphConnectionIndexes[GraphConnectionIndexesIterator]];
        for (int i = 0; i < CurrentNodeNumber; i++) {
            if (NewGraph->Nodes[i].Number == CurrentNodeToMakeConnectionsFor) {
                NewGraph->Nodes[i].Connections = malloc (65536 * sizeof(node_t *));
                int NewGraphNodeConnectionsIterator = 0;
                while (GraphConnectionsIterator != GraphConnectionIndexes[GraphConnectionIndexesIterator + 1] /* && GraphConnectionIndexes[GraphConnectionIndexesIterator + 1] != -1 */) {
                    if (GraphConnections[GraphConnectionsIterator] == -1) {
                        break;
                    }
                    NewGraph->Nodes[i].Connections[NewGraphNodeConnectionsIterator] = &NewGraph->Nodes[GraphConnections[GraphConnectionsIterator]];
                    NewGraphNodeConnectionsIterator++;
                    GraphConnectionsIterator++;
                }
                NewGraph->Nodes[i].Connections[NewGraphNodeConnectionsIterator] = NULL;
                CurrentNodeToMakeConnectionsFor++;
                GraphConnectionsIterator++;
                break;
            }
        }
        GraphConnectionIndexesIterator++;
    }
    return NewGraph;
}

// macierz sąsiedztwa
typedef struct adj_mtx {

    int Size;
    int *Adjacencies;

} adj_mtx_t;

// zwraca liczbę węzłów w grafie
int how_many_nodes(graph_t *Graph){
    int size = 0;
    while (Graph->NodeIndexes[size] != -1){
        size++;
    }
    return size;
}

//generuje macierz sasiedztwa
adj_mtx_t *generate_adj_mtx (graph_t *Graph) {

    adj_mtx_t *NewMtx = malloc(sizeof(adj_mtx_t));

    int GraphSize = how_many_nodes(Graph);
    NewMtx->Size = GraphSize;

    int *Adjacencies = calloc(GraphSize * GraphSize, sizeof(int));

    /* for (int i = 0; i < GraphSize * GraphSize; i++) {
        Adjacencies[i] = 0;
    } */

    for (int i = 0; i < GraphSize; i++) {

        if (Graph->Nodes[i].Connections == NULL) {
            continue;
        }

        int CurrentNode = 0;
        while (Graph->Nodes[i].Connections[CurrentNode] != NULL) {
            Adjacencies[GraphSize * Graph->Nodes[i].Number + Graph->Nodes[i].Connections[CurrentNode]->Number] = 1;
            Adjacencies[GraphSize * Graph->Nodes[i].Connections[CurrentNode]->Number + Graph->Nodes[i].Number] = true;
            CurrentNode++;
        }
    }

    NewMtx->Adjacencies = Adjacencies;

    return NewMtx;
}

void print_adj_mtx (adj_mtx_t *Mtx) {
    printf("|  ");
    for (int i = 0; i < Mtx->Size; i++) {
        printf("|%2d", i);
    }
    printf("|\n");
    for (int i = 0; i < Mtx->Size; i++) {
        printf("|%2d", i);
        for (int j = 0; j < Mtx->Size; j++) {
            if (Mtx->Adjacencies[(Mtx->Size * i) + j] == 1) {
                printf("|[]");
            } else {
                printf("|  ");
            }
        }
        printf("|\n");
    }
}

void Dijkstra (graph_t *Graph, int StartingNode, int NodeDistancesIndex) {
    int VisitedNodes = 1;
    Graph->Nodes[StartingNode].Visited = 1;
    Graph->Nodes[StartingNode].Distances[NodeDistancesIndex] = 0;

    adj_mtx_t *GraphAdjacencyMatrix = generate_adj_mtx(Graph);

    while (VisitedNodes < GraphAdjacencyMatrix->Size) {
        for (int i = 0; i < GraphAdjacencyMatrix->Size; i++) {
            if (GraphAdjacencyMatrix->Adjacencies[StartingNode * GraphAdjacencyMatrix->Size + i] == 0) {
                continue;
            } else if (Graph->Nodes[i].Visited == 0 && Graph->Nodes[i].Distances[NodeDistancesIndex] > (Graph->Nodes[StartingNode].Distances[NodeDistancesIndex] + 1)) {
                Graph->Nodes[i].Distances[NodeDistancesIndex] = (Graph->Nodes[StartingNode].Distances[NodeDistancesIndex] + 1);
            }
        }

        int UnvisitedNodeIndex = 0;
        for (int i = 0; i < GraphAdjacencyMatrix->Size; i++) {
            if (Graph->Nodes[i].Visited == 0) {
                UnvisitedNodeIndex = i;
                break;
            }
        }

        for (int i = 0; i < GraphAdjacencyMatrix->Size; i++) {
            if (Graph->Nodes[i].Visited == false && Graph->Nodes[i].Distances[NodeDistancesIndex] < Graph->Nodes[UnvisitedNodeIndex].Distances[NodeDistancesIndex]) {
                UnvisitedNodeIndex = i;
            }
        }

        StartingNode = UnvisitedNodeIndex;
        Graph->Nodes[StartingNode].Visited = 1;
        VisitedNodes++;
    }

    for (int i = 0; i < GraphAdjacencyMatrix->Size; i++) {
        Graph->Nodes[i].Visited = 0;
    }

    free(GraphAdjacencyMatrix->Adjacencies);
    free(GraphAdjacencyMatrix);
}

// zapis do pliku tekstowego
void write_to_txt(graph_t *Graph){//, char *name, int k){
    FILE *out = fopen("res.txt", "w");
    if (out == NULL){
        printf("Can't access the file!\n");
        return;
    }
    //fprintf(out, "%s\n", name);                        // DOPISAC POZOSTALE ARGS
    //fprintf(out, "%d\n", k);
    int Index = 0;
    for (int i=0; i<Graph->Height; i++){
        fprintf(out, "[");
        for (int j=0; j<Graph->Width; j++){
            if (Graph->NodeIndexes[Index] == (i*Graph->Width)+j){
                fprintf(out, "1. ");
                Index++;
            } else {
                fprintf(out, "0. ");
            }
        }
        fprintf(out, "]\n");
    }
    for (int i = 0; i < 105; i++) {
        int j = 0;
        while(Graph->Nodes[i].Connections != NULL && Graph->Nodes[i].Connections[j] != NULL) {
            fprintf(out, "%d - %d\n", Graph->Nodes[i].Number, Graph->Nodes[i].Connections[j]->Number);
            j++;
        }
    }
    fclose(out);
    printf("zapisano txt\n");
}

// sprawdza czy jest na końcu tablicy
int d (graph_t *Graph, int i, int j){
    if (Graph->Nodes[i].Connections != NULL && Graph->Nodes[i].Connections[j] != NULL){
        return 1;
    } else {
        return 0;
    }
}

//zapis do pliku binarnego 
void write_to_bin(graph_t *Graph){ 
    int Index = 0;
    char minus = '-';
    char one = '1';
    char zero = '0';
    char dot = '.';
    char n = '\n';

    FILE *out = fopen("res.bin", "wb+");
    if (out == NULL){
        printf("Can't open the file!");
        return;
    }
    for (int i = 0; i<Graph->Height; i++){
        for (int j = 0; j<Graph->Width; j++){
            if (Graph->NodeIndexes[Index] == (i*Graph->Width)+j){
                fwrite(&one, sizeof(char), 1, out);
                Index++;
            } else {
                fwrite(&zero, sizeof(char), 1, out);
            }
            fwrite(&dot, sizeof(char), 1, out);
        }
        fwrite(&n, sizeof(char), 1, out);
    }

    char tmp;
    char buffer[12];
    for (int i = 0; i < 105; i++) {
        int j = 0;
        while(d(Graph, i, j)) {
            sprintf(buffer, "%d", Graph->Nodes[i].Number);
            fwrite(buffer, sizeof(char), strlen(buffer), out);
            fwrite(&minus, sizeof(char), 1, out);
            sprintf(buffer, "%d", Graph->Nodes[i].Connections[j]->Number);
            fwrite(buffer, sizeof(char), strlen(buffer), out);
            fwrite(&n, sizeof(char), 1, out);
            j++;
        }
    }
    fclose(out);
    printf("zapisano bin");
}

// sprawdza czy tablica zawiera dany element
int contains (int **miniGraphs, int subgraph, int nodesCounter, int elem, int index, int *assigned) {
    int ile = 0;
    for (int j = 0; j < subgraph; j++){
        for (int m = 0; m < nodesCounter; m++){
            if (miniGraphs[j][m] != elem){
                ile++;
            }
        }
    }
    if (ile == subgraph*nodesCounter){
        miniGraphs[subgraph][index] = elem;
        assigned[elem] = 1;
        return 0;
    }
    return 1;
}

void free_minigraphs(int **miniGraphs, int *assigned, int k){
    for (int i = 0; i < k; i++){
        free(miniGraphs[i]);
    }
    free(miniGraphs);
    free(assigned);
}


// algorytm dzielący graf
void **partition_graph (graph_t *NewGraph, int k, double ErrorMargin, int **miniGraphs, int *assigned) {
    if (ErrorMargin <= 0.0 || ErrorMargin > 0.5) {
        printf("Wrong Error Margin!");
        return NULL;
    }

    int nodesCounter = how_many_nodes(NewGraph)/k; //liczba wierzcholkow w jednym podgrafie

    if (how_many_nodes(NewGraph) % k != 0){
        nodesCounter++;
    }

    int distance = 0; 
    int subgraph = 0; //numer podgrafu
    int index = 0; //index w tablicy podgrafu
    Dijkstra(NewGraph, 0, 0);

    while (subgraph < k-1){
        index = 0;
        distance = 0;
        while (index < nodesCounter){
            int added = 0;
            for (int i = 0; i < how_many_nodes(NewGraph); i++){
                if (NewGraph->Nodes[i].Distances[0] == distance && assigned[i] == 0){
                    miniGraphs[subgraph][index] = NewGraph->Nodes[i].Number;
                    assigned[i] = 1;
                    index++;
                    added = 1;
                }
                if (index >= nodesCounter) break;
            }
            if (!added) distance++; // jeżeli nie udało się dodać
        }
        subgraph++;
    }

    index = 0;
    for (int i = 0; i < how_many_nodes(NewGraph); i++){ //ostatni graf to zlepek pozostalych wezlow
        if (!contains(miniGraphs, subgraph, nodesCounter, NewGraph->Nodes[i].Number, index, assigned)){
            index++;
        }
    }
    for (int i = 0; i < k; i++){
        printf("Podgraf%d: ", i);
        for (int j = 0; j < nodesCounter; j++){
            if (miniGraphs[i][j] >= how_many_nodes(NewGraph) || miniGraphs[i][j] < 0){
                printf("");
            } else {
                printf("%d ", miniGraphs[i][j]);
            }
        }
        printf("\n");
    }
}


int main (int argc, char **argv) { //in, k, margines, typ
    
    int k = 2; //przykladowa liczba czesci do podzialu  przyklad - 2,3,4; graf - 5,9,18,35
    double ErrorMargin = 0.1;
    char *file = "przyklad.csrrg";
    int type = 0;

    if (argc == 1){
        printf("Too few arguments!\n");
        return 1;
    } else if (argc == 2) {
        file = argv[1];
    } else if (argc == 3) {
        file = argv[1];
        k = atoi(argv[2]);
    } else if (argc == 4) {
        file = argv[1];
        k = atoi(argv[2]);
        ErrorMargin = atof(argv[3]);
    } else if (argc == 5) {
        file = argv[1];
        k = atoi(argv[2]);
        ErrorMargin = atof(argv[3]);
        if (argv[4] == "t") {
            type = 1;
        } else if (argv[4] == "s"){
            type = 2;
        } else {
            printf("Wrong type\n");
            type = 2;
            //return 3;
        }
    } else {
        printf("Incorrect arguments!\n");
        return 2;
    }

    FILE *Stream = fopen(file, "r+");
    graph_t *NewGraph = malloc(sizeof(graph_t));
    NewGraph = load_from_file(Stream);  
    fclose(Stream);
    print_graph(NewGraph);
    if (k <= 0 || k > how_many_nodes(NewGraph)/2) {
        printf("Wrong number of parts\n");
    }

    if (type == 1){
        write_to_txt(NewGraph);
    } else if (type == 2) {
        write_to_bin(NewGraph);
    } 

    int **miniGraphs = malloc(k * sizeof(int*));
    for (int i = 0; i < k; i++){
        miniGraphs[i] = malloc((how_many_nodes(NewGraph)/k) * sizeof(int));
    }
    int *assigned = calloc(how_many_nodes(NewGraph), sizeof(int)); //0 - nieprzydzielony, 1 - przydzielony

    partition_graph(NewGraph, k, ErrorMargin, miniGraphs, assigned);

    free_minigraphs(miniGraphs, assigned, k);
    free_graph(NewGraph);
    return 0;
}
