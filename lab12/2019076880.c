#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define INF 1e9

#define DEBUG

FILE *fin;
FILE *fout;

typedef struct Node {
    int vertex;
    int dist;
    int prev;
}Node;

typedef struct Graph {
    int size;
    int** vertices;
    Node* nodes;
}Graph;

typedef struct Heap {
    int Capacity;
    int Size;
    Node* Element;
}Heap;

Graph* createGraph(int size);
void deleteGraph(Graph* g);
void printshortestPath(Graph* g);
void print_path(Graph* g, int v); // 최단경로를 출력해주는 helper function
Heap* createMinHeap(int heapSize);
void deleteMinHeap(Heap* minHeap);
void insertToMinHeap(Heap* minHeap, int vertex, int distance);
Node deleteMin(Heap* minHeap);

void main(int argc, char *argv[]) {
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    Graph* g;
    int size;
    fscanf(fin, "%d\n", &size);
    g = createGraph(size + 1); // 노드 개수 +1 만큼 배열을 할당해야 1번 인덱스부터 사용 가능

    char tmp = 0;
    while (tmp != '\n' && tmp != EOF) { // input parsing
        int node1, node2, weight;
        fscanf(fin, "%d-%d-%d", &node1, &node2, &weight);
        g->vertices[node1][node2] = weight;
        tmp = fgetc(fin);
    }

    printshortestPath(g);

    deleteGraph(g);
    
    fclose(fin);
    fclose(fout);
}

/* Create a new graph */
Graph* createGraph(int size) {
    Graph* new_graph;
    int i, j;

    new_graph = (Graph *)malloc(sizeof(Graph));
    new_graph->size = size - 1; // 매개변수로 size + 1값을 받으므로 노드 개수는 size - 1개

    /* 인접 행렬 초기화 */
    new_graph->vertices = (int **)malloc(sizeof(int *) * size); // 인접 행렬 행 동적할당
    for (i = 0; i < size; ++i) {
        new_graph->vertices[i] = (int *)malloc(sizeof(int) * size); // 인접 행렬
        for (j = 0; j < size; ++j)
            new_graph->vertices[i][j] = 0; // 일단 0으로 전부 초기화함
    }

    /* 노드 정보 초기화 */
    new_graph->nodes = (Node *)malloc(sizeof(Node) * size);
    for (i = 1; i < size; ++i) { // 노드 번호와 인덱스가 같도록 1부터 사용
        new_graph->nodes[i].vertex = i; // 노드 번호는 인덱스와 같은 값
        new_graph->nodes[i].dist = INF; // distance의 temporary값 초기화
        new_graph->nodes[i].prev = 0; // path에 대한 정보가 아직 없으므로 0으로 초기화
    }

    return new_graph;
}

/* free allocated memory of the graph */
void deleteGraph(Graph* g) {
    int i;

    free(g->nodes);
    for (i = 0; i < g->size + 1; ++i) 
        free(g->vertices[i]);
    free(g->vertices);
    free(g);
}

/* Find shortest path by Dijkstra algorithm */
void printshortestPath(Graph* g) {
    Heap* pq = createMinHeap(100); // 큐 하나 생성
    int i;

    insertToMinHeap(pq, 1, 0); // 시작 정점에서부터 자기 자신으로의 최단거리는 0
    g->nodes[1].dist = 0; // 최단거리를 0으로 확정할 수 있음
    g->nodes[1].prev = 1; // 1번 노드의 prev노드는 자기 자신이므로
    while (pq->Size) { // 큐가 빌 때까지 반복
        Node current = deleteMin(pq); // 현재 큐에 들어있는 노드 중 가장 짧은 distance값을 가지는 노드를 꺼냄
        if (g->nodes[current.vertex].dist < current.dist) continue; // 이미 갱신된 최단거리가 있는 노드라면 skip
        for (i = 1; i <= g->size; ++i) {
            if (g->vertices[current.vertex][i]) { // 현재 방문한 노드와 연결되어있는 노드(인접 행렬 값이 0이 아님)에 대해 간선 가중치를 확인. 
                unsigned int next = g->nodes[current.vertex].dist + g->vertices[current.vertex][i]; // 현재 방문 노드까지의 최단거리 값과 인접 노드까지의 간선 거리 합
                unsigned int before = g->nodes[i].dist; // 현재 인접 노드의 최단거리 값
                if (next < before) { // 현재까지의 경로로 인접 노드에 가는 것보다 현재 방문 노드를 거쳐서 가는 것이 더 저렴한 경우, 업데이트
                    g->nodes[i].dist = next; // 인접 노드의 dist값 업데이트
                    g->nodes[i].prev = current.vertex; // 현재 방문 노드를 거쳐서 가므로 현재 방문 노드가 이 인접 노드의 prev노드가 된다
                    insertToMinHeap(pq, i, next); // 힙에 넣어서 다음으로 거리를 확정할 노드 찾기
                }
            }
        }
    }
    /* 1번 노드부터 나머지 모든 노드에 대한 shortest path와 distance 출력 */
    for (i = 2; i <= g->size; ++i) {
        if (g->nodes[i].dist == INF) // 최단 거리가 INF로 저장되어 있으면 1에서 갈 수 없는 노드임
            fprintf(fout, "Cannot reach to node %d.\n", i);
        else {
            print_path(g, g->nodes[i].prev); // 현재 노드의 바로 앞까지의 경로 출력
            fprintf(fout, "%d ", i); // 마지막 경로 (현재 노드) 출력
            fprintf(fout, "(cost: %d)\n", g->nodes[i].dist); // 최단 경로의 cost값(최단거리) 출력
        }
    }
    deleteMinHeap(pq); // 사용 완료한 힙 메모리 해제
}

void print_path(Graph* g, int v) {
    if (v == 1) {
        fprintf(fout, "1->"); // 첫 시작은 늘 1번 노드
        return ;
    }
    print_path(g, g->nodes[v].prev); // 최단경로를 역순으로 거슬러 올라가 출력하기 위해 재귀함수 활용
    fprintf(fout, "%d->", v);
}

/* Create a new min heap */
Heap* createMinHeap(int heapSize) {
    Heap* minheap;

    minheap = (Heap *)malloc(sizeof(Heap));
    minheap->Capacity = heapSize; 
    minheap->Size = 0;
    minheap->Element = (Node *)malloc(sizeof(Node) * (heapSize + 1)); // 1번 인덱스부터 사용하므로 1 더해서 동적할당

    return minheap;
}

/* Free memory of the min heap */
void deleteMinHeap(Heap* minHeap) {
    free(minHeap->Element);
    free(minHeap);
}

void insertToMinHeap(Heap* minHeap, int vertex, int distance) {
    int i;

    if (minHeap->Size == minHeap->Capacity)
        return ;
    minHeap->Size++; // 새로운 값이 인서트되므로 힙의 사이즈 1 증가
    /* percolating up, 빈칸을 올려주고 적당한 자리에 insert */
    for (i = minHeap->Size; (i / 2) > 0 && minHeap->Element[i / 2].dist > distance; i /= 2) // 노드의 distance값을 기준으로 정렬되어야 한다.
        minHeap->Element[i] = minHeap->Element[i / 2];
    minHeap->Element[i].vertex = vertex;
    minHeap->Element[i].dist = distance;
}

Node deleteMin(Heap* minHeap) {
    int i, child;
    Node min_element, last_element;

    min_element = minHeap->Element[1];
    last_element = minHeap->Element[minHeap->Size--];

    /* percolating down, 루트 노드부터 내려가면서 마지막 노드의 새로운 자리로 적당한 자리 찾기 */
    for (i = 1; i * 2 <= minHeap->Size; i = child) {
        child = i * 2;
        if (child < minHeap->Size && minHeap->Element[child + 1].dist < minHeap->Element[child].dist)
            child++;
        if (last_element.dist > minHeap->Element[child].dist)
            minHeap->Element[i] = minHeap->Element[child];
        else
            break;
    }
    minHeap->Element[i] = last_element;
    return min_element; 
}
