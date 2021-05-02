#include<stdio.h>
#include<stdlib.h>
#define INF 1e9

FILE *fin;
FILE *fout;

typedef struct HeapStruct{
    int Capacity;
    int Size;
    int *Elements;
}Heap;

Heap* CreateHeap(int heapSize);
void Insert(Heap *heap, int value);
int Find(Heap *heap, int value);
int DeleteMax(Heap* heap);
void PrintHeap(Heap* heap);
int IsFull(Heap *heap);

int main(int argc, char* argv[]){
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    char cv;
    Heap* maxHeap;
    int heapSize, key, max_element;

    while(!feof(fin)){
        fscanf(fin, "%c", &cv);
        switch(cv){
            case 'n':
                fscanf(fin, "%d", &heapSize);
                maxHeap = CreateHeap(heapSize);
                break;
            case 'i':
                fscanf(fin, "%d", &key);
                Insert(maxHeap, key);
                break;
            case 'd':
                max_element = DeleteMax(maxHeap);
                if(max_element != -INF){
                    fprintf(fout, "Max Element: %d deleted\n", max_element);
                }
                break;
            case 'p':
                PrintHeap(maxHeap);
                break;
            case 'f':
                fscanf(fin, "%d", &key);
                if(Find(maxHeap, key)) fprintf(fout, "%d is in the heap\n", key);
                else fprintf(fout, "Finding Error: %d is not in the heap\n", key);
                break;
        }
    }
    free(maxHeap->Elements);
    free(maxHeap);
    fclose(fin);
    fclose(fout);

    return 0;
}

/* Create a new maxheap */
Heap* CreateHeap(int heapSize){
    Heap* new_maxHeap;
    new_maxHeap = (Heap* )malloc(sizeof(Heap)); // 새로운 힙의 메모리 동적 할당
    new_maxHeap->Capacity = heapSize; 
    new_maxHeap->Size = 0; // 현재 힙에 데이터가 없으므로 0으로 초기화
    new_maxHeap->Elements = (int *)malloc(sizeof(int) * (new_maxHeap->Capacity + 1)); // 1번 인덱스부터 사용하므로 1을 더해서 배열 동적 할당
    return new_maxHeap; // 만들어진 힙 반환
}

/* Insert new node */
void Insert(Heap *heap, int value){
    int i;
    if (IsFull(heap)){
        fprintf(fout, "Insert Error: heap is full\n");
        return;
    }
    // 키값 중복 시 에러
    if (Find(heap, value)){
        fprintf(fout, "Insert Error: %d is already in the heap\n", value);
        return;
    }
    /* percolating up, 현재 인서트 할 값의 적당한 자리를 찾아 올라감 */
    heap->Size++; // 새로운 값이 인서트되므로 힙의 사이즈 1 증가
    for (i = heap->Size; (i / 2) > 0 && heap->Elements[i / 2] < value; i /= 2){ // 가장 마지막 레벨의 가장 오른쪽 노드부터(인덱스가 Size인 노드) 자신의 부모 노드를 따라 올라가면서 적당한 자리 찾기
    // 부모 노드보다 인서트할 값이 작아서 그 자리가 새로운 값의 자리로 적당하다면 반복 종료
        heap->Elements[i] = heap->Elements[i / 2]; // 부모 노드의 값을 한 레벨 내려서 힙의 규칙에 맞게 배열함
    }
    heap->Elements[i] = value;
    fprintf(fout, "insert %d\n", value);
}

/* check whether the key exists in the heap */
int Find(Heap *heap, int value){
    int i;
    for (i = heap->Size; i > 0; --i){ // 가장 말단 노드부터 체크
        if (heap->Elements[i] == value) // 같은 값이 있으면 
            return 1; // 1 반환하여 표시
    }
    return 0;
}

int DeleteMax(Heap* heap){
    int i, child;
    int max_element, last_element;

    if (heap->Size == 0){ // heap이 비어있는 경우
        fprintf(fout, "Delete Error: heap is empty\n");
        return -INF; 
    }

    max_element = heap->Elements[1]; // max heap에서 가장 큰 값은 루트 노드의 값
    last_element = heap->Elements[heap->Size--]; // 마지막 노드를 루트로 가져와서 heap 규칙에 맞게 재조정, 노드가 하나 삭제되므로 Size 1 감소

    /* percolating down, 루트 노드부터 내려가면서 마지막 노드의 새로운 자리로 적당한 자리 찾기 */
    for (i = 1; i * 2 <= heap->Size; i = child){ // 자식 노드가 있는 동안 내려가면서 확인하기
        child = i * 2; // 자식 노드의 인덱스는 현재 노드 * 2
        if (child < heap->Size && heap->Elements[child + 1] > heap->Elements[child]) // 첫번째 조건은 right child가 존재함을 의미, 두번째 조건은 그 right child가 left child 보다 더 큰 값을 갖고 있음을 의미 
            child++; // 두 개의 자식 노드 중 더 큰 값과 비교하며 내려가야 하는데, 이 경우 right child가 더 큰 값이므로 child를 right child로 설정해줌
        if (last_element < heap->Elements[child]) // 마지막 노드의 값이 더 작다면, 더 내려간 자리에 위치해야 하므로 
            heap->Elements[i] = heap->Elements[child]; // child노드를 위로 올려주고 아래로 내려가서 다시 확인
        else
            break; // last_element가 위치할 자리를 찾은 경우 반복 종료
    }
    heap->Elements[i] = last_element; // 찾은 자리에 마지막 노드 값을 써주기
    return max_element; // 삭제된 값을 반환
}

/* Print the entire heap in level order traversal */
void PrintHeap(Heap* heap){
    int i;

    if (heap->Size == 0){ // heap이 비어있는 경우
        fprintf(fout, "Print Error: heap is empty\n");
        return; 
    }
    for (i = 1; i <= heap->Size; ++i){
        fprintf(fout, "%d ", heap->Elements[i]); // 인덱스 순서대로 heap 안에 있는 값들을 출력
    }
    fprintf(fout, "\n");
}

/* check whether the heap is full */
int IsFull(Heap *heap){
    if (heap->Size == heap->Capacity)
        return 1;
    else
        return 0;
}