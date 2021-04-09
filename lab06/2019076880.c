#include<stdio.h>
#include<stdlib.h>

FILE *fin;
FILE *fout;

struct AVLNode;
typedef struct AVLNode *Position;
typedef struct AVLNode *AVLTree;
typedef int ElementType;

typedef struct AVLNode{
    ElementType element;
    AVLTree left, right;
    int height;
}AVLNode;

int Height(Position P);
AVLTree Insert(ElementType X, AVLTree T);
AVLTree Delete(ElementType X, AVLTree T);
Position SingleRotateWithLeft(Position node);
Position SingleRotateWithRight(Position node);
Position DoubleRotateWithLeft(Position node);
Position DoubleRotateWithRight(Position node);
void PrintInorder(AVLTree T);
void DeleteTree(AVLTree T);

int main(int argc, char *argv[]){
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");
    AVLTree Tree = NULL;
    char cv;
    int key;

    while (!feof(fin)){
        fscanf(fin, "%c", &cv);
        switch(cv){
            case 'i':
                fscanf(fin, "%d\n", &key);
                Tree = Insert(key, Tree);
                break;
            case 'd':
                fscanf(fin, "%d\n", &key);
                Tree = Delete(key, Tree);
                break;
        }
        PrintInorder(Tree);
        fprintf(fout, "\n");
    }

    DeleteTree(Tree);
    fclose(fin);
    fclose(fout);

    return 0;
}

int Height(Position P){
    if (P == NULL) // 널 포인터의 멤버를 참조하는 경우 방지!
        return -1; 
    else
        return P->height;
}

AVLTree Insert(ElementType X, AVLTree T){
    if (T == NULL){
        T = malloc(sizeof(AVLNode)); // 새로운 노드 생성
        if (T == NULL){
            fprintf(fout, "Out of Space!");
            return T;
        }
        /* setting the new node */
        T->element = X;
        T->left = T->right = NULL;
        T->height = 0; // 새로 생성된 노드는 말단 노드이므로 heigt는 0
    }
    /* 키값과 현재 노드의 값을 비교해서 왼쪽, 오른쪽 서브 트리로 내려간다. */
    else if (X < T->element){ // 인서트할 키값이 더 작은 경우
        T->left = Insert(X, T->left); // 왼쪽 서브트리로 내려간다.

        /* 인서트가 완료된 이후 AVL규칙에 어긋나는 노드 배치가 있는지 확인해서 필요한 rebalancing 과정을 수행한다.*/
        if (Height(T->left) - Height(T->right) >= 2){ // 현재 왼쪽 서브트리에 새로운 노드가 생성되었으므로 왼쪽 height에서 오른쪽 height를 뺌
            if (X < T->left->element) // 왼쪽 서브 트리의 왼쪽 끝에 인서트되면서 문제가 발생 -> LL상태
                T = SingleRotateWithLeft(T); 
            else // 왼쪽 서브 트리의 오른쪽 끝에 인서트되면서 문제가 발생 -> LR상태
                T = DoubleRotateWithLeft(T);
        }
    }
    else if (X > T->element){ // 인서트할 키값이 더 큰 경우
        T->right = Insert(X, T->right); // 오른쪽 서브트리로 내려간다.

        /* 인서트가 완료된 이후 AVL규칙에 어긋나는 노드 배치가 있는지 확인해서 필요한 rebalancing 과정을 수행한다.*/
        if (Height(T->right) - Height(T->left) >= 2){ // 현재 오른쪽 서브트리에 새로운 노드가 생성되었으므로 오른쪽 height에서 왼쪽 height를 뺌
            if (X > T->right->element) // 오른쪽 서브 트리의 오른쪽 끝에 인서트되면서 문제가 발생 -> RR상태
                T = SingleRotateWithRight(T); 
            else // 오른쪽 서브 트리의 왼쪽 끝에 인서트되면서 문제가 발생 -> RL상태
                T = DoubleRotateWithRight(T);
        }
    }
    else // 인서트할 키값과 현재 노드의 키값이 같은 경우
        fprintf(fout, "Insertion Error: %d is already in the tree!\n", X); // 이미 키값이 존재하므로 에러 메시지를 띄움
    T->height = Height(T->left) > Height(T->right) ? Height(T->left) + 1 : Height(T->right) + 1; // 왼쪽 서브 트리와 오른쪽 서브 트리의 height 중 큰 값에 +1한 것이 현재 루트 노드의 height
    return T; // 현재 루트를 리턴해서 함수를 호출된 역순으로 종료시키고, 새로운 노드가 생성되었다면 위의 노드와 연결된다.
}

AVLTree Delete(ElementType X, AVLTree T){
    AVLTree tmp;
    if (T == NULL){
        fprintf(fout, "Deletion Error: %d is not in the tree!\n", X); 
        return T;
    }
    else if (X < T->element){ // 삭제할 키값이 현재 노드의 키값보다 작다면
        T->left = Delete(X, T->left); // 왼쪽 서브 트리에서 삭제할 노드를 찾는다.
    }
    else if (X > T->element){ // 삭제할 키값이 현재 노드의 키값보다 크다면
        T->right = Delete(X, T->right); // 오른쪽 서브 트리에서 삭제할 노드를 찾는다.
    }
    else if (T->left && T->right){ // 삭제할 노드를 찾았고, 그 노드가 왼쪽, 오른쪽 자식 노드를 모두 갖는 경우
        /* 오른쪽 서브 트리의 leftmost노드 찾기*/
        tmp = T->right;
        while (tmp->left != NULL)
            tmp = tmp->left;

        T->element = tmp->element; // 찾은 노드의 값을 현재 노드에 overwrite  
        /* 삭제할 자리의 노드로 알맞은 값을 올렸으므로 중복되는 값을 삭제한 서브 트리를 연결해준다. 
        이번에 호출되는 deleteNode함수에서 삭제되는 노드는 왼쪽 자식 노드가 없으므로 아래쪽 else구문에 걸린 후 return.*/
        T->right = Delete(T->element, T->right); 
    }
    else{
        tmp = T; // 삭제할 노드 백업
        if (T->left == NULL) // 오른쪽 자식 노드만 있는 경우
            T = T->right; // 오른쪽 자식 노드를 그대로 올려서 연결
        else // 왼쪽 자식 노드만 있거나 자식 노드가 없는 경우
            T = T->left; // 왼쪽 자식 노드를 그대로 올려서 연결
        free(tmp); // 노드 삭제 (메모리 해제)
    }
    if (T == NULL) // 자식 노드가 없을 때 삭제된 경우, 현재 노드에서는 rebalancing을 skip, 이 부분이 없으면 널 노드의 멤버를 참조하는 오류 발생
        return T;
    T->height = Height(T->left) > Height(T->right) ? Height(T->left) + 1 : Height(T->right) + 1; // 왼쪽 서브 트리와 오른쪽 서브 트리의 height 중 큰 값에 +1한 것이 현재 루트 노드의 height
    
    /* 노드 삭제가 완료된 이후 rebalancing */
    if (Height(T->left) - Height(T->right) >= 2){ // 왼쪽 서브 트리의 노드 개수가 문제가 되는 경우
        if (Height(T->left->left) > Height(T->left->right)) // 왼쪽 서브트리의 왼쪽 서브트리 height가 왼쪽 서브트리의 오른쪽 서브 트리 height보다 클때-> LL상태
            T = SingleRotateWithLeft(T); 
        else // 반대의 경우 -> LR상태
            T = DoubleRotateWithLeft(T);
    }
    else if (Height(T->right) - Height(T->left) >= 2){ // 오른쪽 서브 트리의 노드 개수가 문제가 되는 경우
        if (Height(T->right->right) > Height(T->right->left)) // 오른쪽 서브 트리의 오른쪽 서브 트리 height가 오른쪽 서브 트리의 왼쪽 서브 트리height보다 클 때 -> RR상태
            T = SingleRotateWithRight(T);
        else // 반대의 경우 -> RL상태
            T = DoubleRotateWithRight(T);
    }
    return T;
}

/* Rebalancing when LL state*/
Position SingleRotateWithLeft(Position node){
    Position lnode;
    lnode = node->left; 
    node->left = lnode->right; // 매개변수로 받은 노드(현재 루트 노드)의 왼쪽 자식 노드의 오른쪽 서브 트리를 루트 노드의 왼쪽 서브 트리로 연결
    lnode->right = node; // 왼쪽 자식 노드의 오른쪽 서브 트리로 루트 노드를 연결
    /* 왼쪽으로 길게 나열된 모양에서 삼각형 모양의 balancing이 된 트리로 바뀜 */

    /* 트리 높이 정보 갱신 */
    node->height = Height(node->left) > Height(node->right) ? Height(node->left) + 1 : Height(node->right) + 1; // 왼쪽 서브 트리와 오른쪽 서브 트리의 height 중 큰 값에 +1한 것이 현재 루트 노드의 height
    lnode->height = Height(lnode->left) > node->height ? Height(lnode->left) + 1 : node->height + 1; // 바로 위해서 구한 height는 lnode의 오른쪽 서브 트리 height이므로 이용해서 lnode의 height정보 갱신

    return lnode; // 새로운 루트 노드 반환
}

/* Rebalancing when RR state */
Position SingleRotateWithRight(Position node){
    Position rnode;
    rnode = node->right;
    node->right = rnode->left;
    rnode->left = node;

    /* 트리 높이 정보 갱신 */
    node->height = Height(node->left) > Height(node->right) ? Height(node->left) + 1 : Height(node->right) + 1; // 왼쪽 서브 트리와 오른쪽 서브 트리의 height 중 큰 값에 +1한 것이 현재 루트 노드의 height
    rnode->height = Height(rnode->right) > node->height ? Height(rnode->right) + 1 : node->height + 1; // 바로 위해서 구한 height는 lnode의 오른쪽 서브 트리 height이므로 이용해서 lnode의 height정보 갱신
    
    return rnode;
}

/* Rebalancing when LR state */
Position DoubleRotateWithLeft(Position node){
    /* 우선 LL상태로 만들어 주기 위해서 루트 노드의 왼쪽 서브 트리를 left rotation */
    node->left = SingleRotateWithRight(node->left);

    /* 이제 LL상태가 되었음 */
    return SingleRotateWithLeft(node);
}

/* Rebalancing when RL state */
Position DoubleRotateWithRight(Position node){
    /* 우선 RR상태로 만들어 주기 위해서 루트 노드의 오른쪽 서브 트리를 right rotation */
    node->right = SingleRotateWithLeft(node->right);

    /* 이제 RR상태가 되었음 */
    return SingleRotateWithRight(node);
}

/* Inorder Traversal */
void PrintInorder(AVLTree T){
    if (T == NULL)
        return;
    /* 트리를 중위순회(왼쪽자식->부모->오른쪽자식) 하면서 값을 출력해준다. */
    PrintInorder(T->left);
    fprintf(fout, "%d(%d) ", T->element, T->height);
    PrintInorder(T->right);
}

/* Postorder Traversal */
void DeleteTree(AVLTree T){
    if (T == NULL)
        return;
    /* 트리를 후위순회(왼쪽자식->오른쪽자식->부모) 하면서 노드를 삭제한다. 
    루트 노드를 가장 마지막에 삭제해야 하므로 후위 순회 사용 */
    DeleteTree(T->left);
    DeleteTree(T->right);
    free(T);
}