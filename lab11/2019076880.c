#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define DEBUG

FILE *fin;
FILE *fout;

typedef struct _Queue {
    int* key;
    int first, rear, qsize, max_queue_size;
}Queue;

typedef struct _Graph {
    int size;
    int* node;
    int** matrix;
}Graph;

Graph* CreateGraph(int* nodes, int n);
void InsertEdge(Graph* G, int a, int b);
void PrintGraph(Graph* G);
void DeleteGraph(Graph* G);
void Topsort(Graph* G);
Queue* MakeNewQueue(int X);
int IsEmpty(Queue* Q);
int Dequeue(Queue* Q);
void Enqueue(Queue* Q, int X);
void DeleteQueue(Queue* Q);
void countInput(int* n, char* str);
void parseInput(int* arr, char* str, int n);

int main(int argc, char* argv[]) {
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    char numbers[100], inputs[150];
    fgets(numbers, 100, fin);
    int n = 0;
    countInput(&n, numbers); // 정점 개수 세기
    int* nodes = (int*)malloc(sizeof(int)*n); // 정점 개수만큼 동적할당
    parseInput(nodes, numbers, n); // nodes에 정점 key값만 저장

    Graph* G = CreateGraph(nodes, n); // 정점 개수만큼 그래프 생성

    fgets(inputs, 100, fin); // 간선 정보 가져오기
    int len = strlen(inputs), i, j;
    for (i = 0; i < len; i += 4) {
        int a = inputs[i] - '0', b = inputs[i + 2] - '0';
        InsertEdge(G, a, b); // 간선 정보 그래프에 저장
    }

    PrintGraph(G);
    Topsort(G);
    DeleteGraph(G);

    fclose(fin);
    fclose(fout);

    return 0;
}

/* n은 node 개수 */
void countInput(int* n, char* str) {
    int len = strlen(str), i;
    for (i = 0; i < len; i++)
        if (0 <= str[i] - '0' && str[i] - '0' < 10) (*n)++;
}

void parseInput(int* arr, char* str, int n) {
    int len = strlen(str), i;
    int cnt = 0;
    for (i = 0; i < len; i++)
    if (0 <= str[i] - '0' && str[i] - '0' < 10) arr[cnt++] = str[i] - '0';
}

Graph* CreateGraph(int* nodes, int n) {
    Graph *new;
    int i, j, tmp;
    
    new = (Graph *)malloc(sizeof(Graph));
    new->size = n;
    /* 노드의 키값을 담은 배열인 nodes를 정렬하여 new->node로 저장 (버블 정렬 사용) */
    for (i = 0; i < n - 1; ++i) {
        for (j = 0; j < n - i - 1; ++j) {
            if (nodes[j] > nodes[j + 1]) {
                tmp = nodes[j];
                nodes[j] = nodes[j + 1];
                nodes[j + 1] = tmp;
            }
        }
    }
    new->node = nodes; // 정렬된 배열을 그래프 구조체에 저장

    /* 인접 행렬 동적 할당 */
    new->matrix = (int **)malloc(sizeof(int *) * new->size);
    for (i = 0; i < n; ++i) {
        new->matrix[i] = (int *)malloc(sizeof(int) * n);
        for (j = 0; j < n; ++j)
            new->matrix[i][j] = 0; // 일단 전부 0으로 초기화
    }
    return new;
}

/* Insert an edge from node a to node b */
void InsertEdge(Graph* G, int a, int b) {
    int a_idx, b_idx;
    int i, flag;

    flag = 0;
    for (i = 0; i < G->size; ++i) { // 노드의 키값에 해당하는 인덱스 값 찾기
        if (G->node[i] == a) {
            a_idx = i;
            flag = 1;
        }
        if (G->node[i] == b) {
            b_idx = i;
            if (flag) // 두 개의 인덱스를 다 찾은 경우 바로 종료
                break; 
        }
    }
    G->matrix[a_idx][b_idx] = 1; // a에서 b로 연결되어 있음을 표시
}

/* Print the graph (adjacent matrix) */
void PrintGraph(Graph* G) {
    int i, j;

    for (i = 0; i < G->size + 1; ++i) {
        for (j = 0; j < G->size + 1; ++j) {
            if (i && j) // 인접 행렬 안의 값 출력
                fprintf(fout, "%d  ", G->matrix[i - 1][j - 1]);
            else if (!i) { // 가로 노드 key값 출력
                if (!j)
                    fprintf(fout, "   ");
                else
                    fprintf(fout, "%d  ", G->node[j - 1]);
            }
            else if (!j) // 세로 노드 key값 출력
                fprintf(fout, "%d  ", G->node[i - 1]);
        }
        fprintf(fout, "\n");
    }
    fprintf(fout, "\n");
}

/* Delete the graph */
void DeleteGraph(Graph* G) {
    int i, j;

    free(G->node);
    for (i = 0; i < G->size; ++i)
        free(G->matrix[i]);
    free(G->matrix);
    free(G);
}

/* Print the result of topological sort */
void Topsort(Graph* G) {
    int i, j;
    int *indeg, *chk_cycle, *sort_res;
    Queue *q;

    indeg = (int *)malloc(sizeof(int) * G->size); // 각 노드의 in-degree값을 저장할 배열
    chk_cycle = (int *)malloc(sizeof(int) * G->size); // 큐에 노드가 들어갔음을 체크할 배열
    sort_res = (int *)malloc(sizeof(int) * G->size); // topological sorting된 결과의 순서대로 노드를 저장할 배열
    for (i = 0; i < G->size; ++i)
        indeg[i] = chk_cycle[i] = 0; // 0으로 초기화
    for (i = 0; i < G->size; ++i) {
        for (j = 0; j < G->size; ++j) {
            if (G->matrix[i][j]) // i 와 j가 연결되어 있다면, j는 terminal node이므로 i 에 의해 in-degree 값이 +1 됨
                indeg[j]++;
        }
    }
    q = MakeNewQueue(51); // input file을 최대 100만큼 읽어오므로 노드 개수 최대 51개로 설정

    /* Topological sort */
    for (i = 0; i < G->size; ++i) {
        if (!indeg[i]) { // in-degree값이 0인 노드를
            Enqueue(q, i); // 큐에 넣는다
            chk_cycle[i] = 1;
        }
    }
    i = 0;
    while (!IsEmpty(q)) { // 큐가 빌 때까지 반복
        int now;
        now = Dequeue(q); // in-degree 값이 현재 0인 노드를 하나 꺼낸다
        sort_res[i++] = G->node[now]; // 노드의 키값을 sorting 결과로 저장
        for (j = 0; j < G->size; ++j) {
            if (G->matrix[now][j]){ // now 노드와 연결되어 있는 노드에 대하여
                if (!(--indeg[j])) { // edge를 제거하므로 in-degree값이 하나 감소, 이때 그 값이 만일 0이라면
                    Enqueue(q, j); // 큐에 넣는다
                    chk_cycle[j] = 1;
                }
            }
        }
    }
    DeleteQueue(q); // 큐 사용 완료, 삭제

    /* cycle error를 확인해서 에러 메시지 출력 */
    for (i = 0; i < G->size; ++i) {
        if (!chk_cycle[i]) {
            fprintf(fout, "Sorting Error: Cycle!");
            break;
        }
    }

    /* cycle error가 아닌 경우 sort 결과 출력 */
    if (i == G->size) {
        for (i = 0; i < G->size; ++i)
            fprintf(fout, "%d ", sort_res[i]);
    }
    fprintf(fout, "\n");

    free(indeg);
    free(chk_cycle);
    free(sort_res);
}

/* Make a new queue (circular queue) */
Queue* MakeNewQueue(int X) {
    Queue *q;

    q = (Queue *)malloc(sizeof(Queue));
    q->max_queue_size = X;
    q->key = (int *)malloc(sizeof(int) * q->max_queue_size);
    q->first = 1;
    q->rear = 0;
    q->qsize = 0;
    return q;
}

/* check whether the queue is empty */
int IsEmpty(Queue* Q) {
    if (!Q->qsize)
        return 1;
    return 0;
}

int Dequeue(Queue* Q) {
    Q->qsize--;
    return Q->key[Q->first++];
}

void Enqueue(Queue* Q, int X) {
    Q->qsize++;
    if (++Q->rear == Q->max_queue_size)
        Q->rear = 0;
    Q->key[Q->rear] = X;
}

void DeleteQueue(Queue* Q) {
    free(Q->key);
    free(Q);
}