#include<stdio.h>
#include<stdlib.h>

FILE *fin;
FILE *fout;

typedef struct BNode* BNodePtr;

struct BNode{
    int order;
    int size;           /* number of children */
    BNodePtr *child;    /* children pointers */
    int *key;           /* keys */
    int is_leaf;
}BNode;

BNodePtr CreateTree(int order);

void Insert(BNodePtr *root, int key);
BNodePtr Insert_sub(BNodePtr parent, BNodePtr node, int key, int pos);
BNodePtr split_node(BNodePtr parent, BNodePtr node, int pos);
BNodePtr Createnode(int order);

int Find(BNodePtr root, int key);

void PrintTree(BNodePtr root);

void DeleteTree(BNodePtr root);
void free_node(BNodePtr node);



int main(int argc, char* argv[]){
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    int order;
    fscanf(fin, "%d", &order);
    BNodePtr root = CreateTree(order);

    char cv;
    int key;
    while(!feof(fin)){
        fscanf(fin, "%c", &cv);
        switch(cv){
            case 'i':
                fscanf(fin, "%d", &key);
                if(Find(root, key))
                    fprintf(fout, "Insert Error: key %d is already in the tree!\n", key);
                else
                    Insert(&root, key);
                break;
            case 'f':
                fscanf(fin, "%d", &key);
                if(Find(root, key))
                    fprintf(fout, "Find key %d\n", key);
                else
                    fprintf(fout, "Finding Error: key %d is not in the tree!\n", key);
                break;
            case 'p':
                if (root->size == 1)
                    fprintf(fout, "Print Error: tree is empty!");
                else
                    PrintTree(root);
                fprintf(fout, "\n");
                break;
        }
    }
   
    DeleteTree(root);
    fclose(fin);
    fclose(fout);

    return 0;
}

/* 새로운 트리를 생성하는 함수 */
BNodePtr CreateTree(int order){
    BNodePtr root;
    int i;

    root = (BNodePtr)malloc(sizeof(BNode)); // 노드를 하나 생성
    root->order = order;
    root->size = 1; // 아직 연결된 자식 노드가 없음 && is_leaf == 1 -> 들어 있는 데이터 없음
    root->child = (BNodePtr *)malloc(sizeof(BNodePtr) * (root->order + 1)); // 최대 자식 개수는 order인데, full인 경우 일단 노드를 만들어준 후 split하므로 1 더해서 동적 할당
    for (i = 0; i < order; ++i)
        root->child[i] = NULL; // 아직 자식 노드가 하나도 없으므로 NULL로 초기화
    root->key = (int *)malloc(sizeof(int) * (root->order)); // key의 최대 개수는 order - 1인데, full인 경우 일단 insert한 후 split하기 위해 한칸 더 동적할당함
    root->is_leaf = 1; // 새로 만들어진 루트 노드이므로 리프 노드
    return (root);
}

/* insert key */
void Insert(BNodePtr *root, int key){ // 루트를 함수 내에서 변경시키기 위해 이중 포인터로 선언
    *root = Insert_sub(*root, *root, key, 0); // 리턴값이 있는 재귀함수 활용, 처음에 root 노드는 현재 노드이자 parent이므로 같이 parent, node 똑같이 매개변수 전달
}

/* 현재 노드와 그 노드의 부모 노드를 같이 매개변수로 받아서 split이후 연결이 끊기지 않도록 함
*  p_pos는 parent노드의 key값 중 현재 노드의 부모가 되는 위치를 의미함
*/
BNodePtr Insert_sub(BNodePtr parent, BNodePtr node, int key, int p_pos){
    int pos;

    for (pos = 0; pos < node->size - 1; ++pos){ // 현재 노드의 키 개수만큼 탐색하면서 내려가거나 값을 넣을 위치 찾기
        if (key < node->key[pos]) // 오름차순으로 정렬되어 있으므로
            break;
    }
    if (!node->is_leaf){ // 현재 노드가 leaf가 아니라면
        node->child[pos] = Insert_sub(node, node->child[pos], key, pos); // 찾은 pos의 경로를 따라 트리 아래쪽 탐색
        if (node->size > node->order) // 현재 노드가 꽉 찬 경우
            node = split_node(parent, node, p_pos); // split해 줌
    }
    else{ // 현재 노드가 leaf라면 insert할 위치를 찾은 것
        int i;

        for (i = node->size - 1; i > pos; --i){ // pos뒷부분의 key들을 한칸씩 뒤로 밀어줌
            node->key[i] = node->key[i - 1];
            node->child[i + 1] = node->child[i];
        }
        node->key[pos] = key; // 찾은 위치에 key값 insert
        node->size++; // key가 하나 추가되었으므로 +1
        if (node->size > node->order){ // 노드가 꽉 차서 split해야 하는 경우
            node = split_node(parent, node, p_pos);
        }
    }
    return node; // 리턴값을 받아서 위로 연결
}

BNodePtr split_node(BNodePtr parent, BNodePtr node, int pos){
    int mid = (node->order - 1) / 2;
    int i;
    BNodePtr right_child;
    BNodePtr new_root;

    /* right child를 생성하고 키값 옮겨 담기 */
    right_child = Createnode(node->order); // right child 생성
    right_child->is_leaf = node->is_leaf; // right child의 leaf여부는 현재 노드의 leaf여부와 같다.
    for (i = 0; i < node->order - mid - 1; ++i, ++(right_child->size)) // 오른쪽에 있는 노드 개수만큼 옮겨담기, size하나씩 증가시킴
        right_child->key[i] = node->key[mid + i + 1]; // node의 오른쪽 값들을 옮기기
    node->size = mid + 1; // 원래 노드는 왼쪽 child가 되고, 키 개수가 mid개 이므로 size는 mid + 1

    /* leaf노드가 아닌 경우에만 child처리, child포인터 split하기 */
    if (!node->is_leaf){
        for (i = 0; i < node->order - mid; ++i)
            right_child->child[i] = node->child[mid + i + 1];
    }
    /* parent 처리, 중앙값 위로 올리고, parent와 split한 두 node연결하기 */
    if (parent == node){ // 루트 노드를 split해야 하는 경우

        new_root = Createnode(node->order);
        new_root->is_leaf = 0; // split된 두 노드의 부모 노드 이므로 leaf가 아니다
        new_root->key[0] = node->key[mid]; // node의 중앙값을 루트로 올려줌
        new_root->child[0] = node;
        new_root->child[1] = right_child;
        new_root->size++; // 키를 하나 넣었으므로 size증가시키기
        return new_root;
    }
    else{ // 루트 노드가 아닌 경우, 기존에 있는 parent node를 활용
        for (i = parent->size - 1; i > pos; --i){
            parent->key[i] = parent->key[i - 1]; // 한칸씩 뒤로 옮기기
            parent->child[i + 1] = parent->child[i];
        }
        parent->key[pos] = node->key[mid]; // 중앙값을 해당하는 자리로 올림
        parent->size++; // 부모 노드에 키가 하나 추가되었으므로 1 증가
        parent->child[pos + 1] = right_child; // 오른쪽 노드 새로 연결 (왼쪽 노드는 기존 노드, size만 변경해서 split완료)
    }
    return node;
}

/* 노드 하나를 동적할당하는 함수, size, is_leaf는 노드마다 다르므로 초기화하지 않고 공통되는 부분만 초기화 */
BNodePtr Createnode(int order){ 
	BNodePtr new;
    int i;

    new = (BNodePtr)malloc(sizeof(BNode)); 
    new->order = order;
    new->key = (int *)malloc(sizeof(int) * (order)); // key의 최대 개수는 order - 1인데, full인 경우 일단 insert한 후 split하기 위해 한칸 더 동적할당함
    new->child = (BNodePtr *)malloc(sizeof(BNodePtr) * (order + 1)); // 최대 자식 개수는 order인데, full인 경우 일단 노드를 만들어준 후 split하므로 1 더해서 동적 할당
    for (i = 0; i < order; ++i)
        new->child[i] = NULL;
    new->size = 1; // 아직 아무 데이터도 없고 child도 없음, child 포인터를 담으면서 +1 해주기 때문에 1로 초기화
    return new;
}

/* find node that has key */
int Find(BNodePtr root, int key){
    int i;

    for (i = 0; i < root->size; ++i){ // 현재 노드의 키 개수만큼 탐색하면서 내려갈 위치 찾기
        if (i == root->size - 1 || key < root->key[i]) 
        // 내려갈 위치를 찾았거나, i == root->size - 1 인 경우 break. 
        // i == root->size - 1일 때 break 하는 이유는 key배열의 마지막 인덱스를 참조하게 되므로 이를 막아주기 위함 
            break;
        if (key == root->key[i]) // 찾으면 1 리턴
            return 1;
    }
    if (!root->is_leaf) // 현재 노드가 leaf가 아니라면
        return (Find(root->child[i], key)); // 찾은 경로를 따라 트리 아래쪽 탐색, 리턴값을 연달아 받음
    return 0; // 다 내려왔는데 key를 찾지 못한 경우 0 리턴
}

/* print Tree, inorder traversal */
void PrintTree(BNodePtr root){
    int i;

    if (!root->is_leaf){
        for (i = 0; i < root->size; ++i){
            PrintTree(root->child[i]); // 재귀로 들어가면서 트리 순회
            if (i < root->size - 1) // i == root->size - 2가 key배열의 마지막 인덱스이므로 
                fprintf(fout, "%d ", root->key[i]);
        }
    }
    else{ // 리프 노드를 만난 경우 
        for (i = 0; i < root->size - 1; ++i){
            fprintf(fout, "%d ", root->key[i]); // key배열의 값을 순서대로 쭉 출력
        }
    }
}

/* depth first search -> free memory */
void DeleteTree(BNodePtr root){
    int i;

    if (root->is_leaf == 0){
        for (i = 0; i < root->size; ++i){ // 자식 노드가 있는 동안 내려가줌
            DeleteTree(root->child[i]);
        }
    }
    // 리프 노드를 만나면 바로 free, 있는 자식 노드를 다 내려가본 후 free
    free_node(root);
}

/* 노드의 메모리를 해제해주는 helper function */
void free_node(BNodePtr node){
    free(node->child);
    free(node->key);
    free(node);
}