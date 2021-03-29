#include <stdio.h>
#include <stdlib.h>

FILE *fin;
FILE *fout;

typedef struct ThreadedTree* ThreadedPtr;
typedef int ElementType;

struct ThreadedTree {
    int left_thread; // flag if ptr is thread
    ThreadedPtr left_child;
    ElementType data;
    ThreadedPtr right_child;
    int right_thread; // flag if ptr is thread
}ThreadedTree;

ThreadedPtr CreateTree();
int Insert(ThreadedPtr root, int root_idx, ElementType data, int idx);
void printInorder(ThreadedPtr root);
void DeleteTree(ThreadedPtr root);

int main(int argc, char *argv[]){
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    ThreadedPtr root = CreateTree();

    int NUM_NODES;
    fscanf(fin, "%d", &NUM_NODES);

    int root_idx = 0, idx = 0;

    while (++idx <= NUM_NODES){
        ElementType data;
        fscanf(fin, "%d", &data);

        if(Insert(root, root_idx, data, idx) == 0){
            return 0;
        }
    }

    printInorder(root);
    DeleteTree(root);

    fclose(fin);
    fclose(fout);

    return 0;
}

ThreadedPtr CreateTree(){
    ThreadedPtr tree = NULL;
    tree = (ThreadedPtr)malloc(sizeof(ThreadedTree));
    if (tree == NULL){
        fprintf(fout, "Out of Space!\n");
        return NULL;
    }
    tree->data = -1;
    tree->left_thread = 1;
    tree->left_child = tree;
    tree->right_thread = 1;
    tree->right_child = tree;
    return tree;
}

int Insert(ThreadedPtr root, int root_idx, ElementType data, int idx){
    ThreadedPtr newNode = NULL; // 새로운 노드 생성
    ThreadedPtr parent;

    newNode = (ThreadedPtr)malloc(sizeof(ThreadedTree)); // 새로운 노드에 대한 동적 메모리 할당
    if (newNode == NULL){ //동적 메모리 할당 실패 시
        fprintf(fout, "Out of Space!\n"); // 에러 메시지 출력
        return 0;
    }
    newNode->data = data; // 데이터 저장
    newNode->left_thread = 1; // 새로 삽입되는 노드는 leaf 노드이므로 플래그 1
    newNode->right_thread = 1; // 새로 삽입되는 노드는 leaf 노드이므로 플래그 1

    /*루트 노드부터 아래로 탐색할 때 인덱스를 2진법 숫자로 바꾼 후 앞에서부터 읽어서 1이면 오른쪽 자식 노드,
    0이면 왼쪽 자식 노드로 이동하면 그 인덱스에 해당하는 노드에 도달할 수 있다.*/

    /*idx번 노드를 삽입하기 위해 찾아야 하는 노드는 idx/2 (root_idx == 0 일 때)*/
    

    int *bin;
    int i, cnt, p_idx;
    p_idx = (idx - root_idx) / 2; // 찾아야 하는 부모 노드의 인덱스

    if (p_idx == 0) // 부모 노드의 인덱스가 0인 경우(첫 번째 노드 삽입)는 예외 처리
        parent = root;    
    
    /*p_idx를 2진법으로 바꿔서 배열에 저장하기*/
    else{
        for (i = p_idx, cnt = 0; i > 0; ++cnt) // 이진법 숫자 자릿수 세주기
            i /= 2;
        bin = (int *)malloc(sizeof(int) * cnt); // 필요한 만큼 배열 할당
        if (bin == NULL){ // 할당 실패 시 
            fprintf(fout, "Out of Space!\n");
            return 0;
        }
        for (i = cnt - 1; i >= 0; --i){ // 이진법 숫자를 배열에 저장
            bin[i] = p_idx % 2; // 2로 나눈 나머지를 뒤에서부터 저장한다.
            p_idx /= 2;
        }

        /*이진수 배열을 바탕으로 부모 노트 탐색 : 1이면 오른쪽, 0이면 왼쪽으로 이동*/
        parent = root; //루트 노드부터 아래로 탐색
        for (i = 0; i < cnt; ++i){
            if (bin[i]) // 1이면 오른쪽
                parent = parent->right_child;
            else // 0이면 왼쪽
                parent = parent->left_child;
        }
        free(bin); // bin 배열 사용 완료
    }

    /*이제 parent는 새로운 노드가 삽입되어야 할 위치(부모 노드)를 가리킨다.*/

    /*idx - root_idx 가 짝수이면 왼쪽 자식 노드로 연결*/
    if (!((idx - root_idx) % 2)){ 
        newNode->left_child = parent->left_child; // parent의 왼쪽 스레드를 물려받기
        newNode->right_child = parent; // newNode의 inorder successor는 parent 노드
        parent->left_child = newNode; // parent의 왼쪽 자식 노드로 연결
        parent->left_thread = 0; // parent의 왼쪽은 이제 스레드가 아님 (노드가 연결된 edge)
    }
    /*idx - root_idx 가 홀수이면 오른쪽 자식 노드로 연결*/
    else{ 
        newNode->left_child = parent; // 새로운 노드의 inorder predecessor는 parent노드
        newNode->right_child = parent->right_child; // parent의 오른쪽 스레드를 물려받기
        parent->right_child = newNode; // parent의 오른쪽 자식 노드로 연결
        parent->right_thread = 0; // parent의 오른쪽은 이제 스레드가 아님 (노드가 연결된 edge)
    }
    return 1;
}

void printInorder(ThreadedPtr root){
    ThreadedPtr now, tmp;

    fprintf(fout, "Inorder Traversal :");
    /*트리를 중위순회 하면서 데이터 출력*/
    now = root;
    while(1){
        /*'now' 노드의 inorder successor 찾기 : leftmost node of its right sub tree*/
        tmp = now->right_child; 
        if (!now->right_thread){ 
            while (!tmp->left_thread){
                tmp = tmp->left_child;
            }
        }
        now = tmp;
        if (now == root) // 트리의 마지막 노드의 스레드는 루트 노드를 가리킨다.
            break;
        fprintf(fout, " %d", now->data); // 노드 안의 데이터 출력

    }
}

/*후위순회 하면서 노드 삭제 (메모리 해제), 루트노드를 가장 마지막에 삭제해야 하므로 후위 순회!*/
void DeleteTree(ThreadedPtr root){
    if (root->data == -1){ // 루트 노드 먼저 삭제
        ThreadedPtr tmp;
        tmp = root;
        root = root->right_child; // 인덱스 1번 노드부터 후위 순회 시작됨
        free(tmp);
    }
    if (root->left_thread){ // 왼쪽으로 끝까지 내려갔을 때 재귀 호출 종료
        free(root);
        return ;
    }
    DeleteTree(root->left_child); // 왼쪽 자식 노드로 내려감
    if (root->right_thread){ // 오른쪽으로 끝까지 내려갔을 때 재귀호출 종료
        free(root);
        return;
    }
    DeleteTree(root->right_child); // 오른쪽 자식 노드로 내려감
    free(root); // 재귀 호출이 종료되면 후위 순회 순서에 맞게 메모리 해제
}