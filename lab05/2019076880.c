#include <stdio.h>
#include <stdlib.h>

FILE *fin;
FILE *fout;

typedef struct BST* Tree;
typedef struct BST{
    int value;
    struct BST* left;
    struct BST* right;
}BST;

Tree insertNode(Tree root, int key, int *err_flag); // 에러 확인을 위한 매개변수 추가
Tree deleteNode(Tree root, int key, int *err_flag);
int findNode(Tree root, int key);
void printInorder(Tree root);
void deleteTree(Tree root);

void main(int argc, char *argv[]){
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");
    char cv;
    int key;
    int err_flag; // 에러 확인을 위한 변수 추가

    Tree root = NULL;
    while (!feof(fin)){
        fscanf(fin, "%c", &cv);
        switch(cv){
            case 'i':
                fscanf(fin, "%d", &key);
                err_flag = 0; 
                root = insertNode(root, key, &err_flag); // insertNode는 루트 노드의 주소를 반환한다.
                if (!err_flag) // 에러가 없는 경우 
                    fprintf(fout, "insert %d", key); // 인서트한 노드 정보 출력
                else if (err_flag == 1) // 1번 에러 : Insertion Error, 동일한 키값의 노드를 삽입하려고 한 경우
                    fprintf(fout, "Insertion Error: %d is already in the tree", key);
                fprintf(fout, "\n");
                break;
            case 'f':
                fscanf(fin, "%d", &key);
                if (findNode(root, key)) // 해당 키값의 노드를 찾은 경우
                    fprintf(fout, "%d is in the tree", key);
                else // 해당 키값의 노드를 찾지 못한 경우
                    fprintf(fout, "Finding Error: %d is not in the tree", key);
                fprintf(fout, "\n");
                break;
            case 'd': 
                fscanf(fin, "%d", &key);
                err_flag = 0;
                root = deleteNode(root, key, &err_flag); // deleteNode는 루트 노드의 주소를 반환한다.
                if (!err_flag) // 정상적으로 노드가 삭제된 경우
                    fprintf(fout, "delete %d", key);
                else // 삭제하려는 키값을 가진 노드가 없는 경우
                    fprintf(fout, "Deletion Error: %d is not in the tree", key);
                fprintf(fout, "\n");             
                break;
            case 'p':
                fscanf(fin, "%c", &cv);
                if (cv == 'i'){
                    if (root == NULL) // 루트 노드가 NULL이면 빈 트리
                        fprintf(fout, "Tree is empty");
                    else // 빈 트리가 아니라면 printInorder함수 호출
                        printInorder(root);
                }        
                fprintf(fout, "\n");
                break;
        }
    }
    deleteTree(root); // 트리 삭제
    fclose(fin);
    fclose(fout);
}

/* 매개변수로 받은 키값을 갖는 노드를 트리에 삽입하기, 트리의 루트 노드 주소값을 반환 */
Tree insertNode(Tree root, int key, int *err_flag){
    /* root == NULL 이면 그 자리가 새로운 노드의 자리! 트리의 끝에 새로운 노드를 달아준다. */
    if (root == NULL){ 
        root = (Tree)malloc(sizeof(BST)); 
        if (root == NULL){ // 동적 할당 에러가 발생한 경우
            fprintf(fout, "Out of Space!");
            *err_flag = 2; // 출력이 함수 내에서 이루어지고, 에러임을 알려주기 위해 2를 사용
        }
        /* 새로운 노드 세팅 */
        root->value = key;
        root->left = NULL;
        root->right = NULL;
    }
    /* 키값과 현재 노드의 값을 비교해서 왼쪽, 오른쪽 서브 트리로 내려간다. */
    else if (key < root->value) // 인서트할 키값이 더 작은 경우
        root->left = insertNode(root->left, key, err_flag); // 왼쪽 서브트리로 내려간다.
    else if (key > root->value) // 인서트할 키값이 더 큰 경우
        root->right = insertNode(root->right, key, err_flag); // 오른쪽 서브트리로 내려간다.
    else // 인서트할 키값과 현재 노드의 키값이 같은 경우
        *err_flag = 1; // 이미 키값이 존재하므로 에러 플래그를 바꿔준다.
    return root; // 현재 노드를 리턴해서 함수를 호출된 역순으로 종료시키고, 새로운 노드가 생성되었다면 위의 노드와 연결된다.
}

/* 매개변수로 받은 키값을 갖는 노드 삭제하기, 트리의 루트 노드 주소값을 반환 */
Tree deleteNode(Tree root, int key, int *err_flag){
    Tree tmp;
    if (root == NULL){
        *err_flag = 1;
        return root;
    }
    else if (key < root->value){ // 삭제할 키값이 현재 노드의 키값보다 작다면
        root->left = deleteNode(root->left, key, err_flag); // 왼쪽 서브 트리에서 삭제할 노드를 찾는다.
    }
    else if (key > root->value){ // 삭제할 키값이 현재 노드의 키값보다 크다면
        root->right = deleteNode(root->right, key, err_flag); // 오른쪽 서브 트리에서 삭제할 노드를 찾는다.
    }
    else if (root->left && root->right){ // 삭제할 노드를 찾았고, 그 노드가 왼쪽, 오른쪽 자식 노드를 모두 갖는 경우
        /* 오른쪽 서브 트리의 leftmost노드 찾기*/
        tmp = root->right;
        while (tmp->left != NULL)
            tmp = tmp->left;

        root->value = tmp->value; // 찾은 노드의 값을 현재 노드에 overwrite  
        /* 삭제할 자리의 노드로 알맞은 값을 올렸으므로 중복되는 값을 삭제한 서브 트리를 연결해준다. 
        이번에 호출되는 deleteNode함수에서 삭제되는 노드는 왼쪽 자식 노드가 없으므로 아래쪽 else구문에 걸린 후 return.*/
        root->right = deleteNode(root->right, root->value, err_flag); 
    }
    else{
        tmp = root; // 삭제할 노드 백업
        if (root->left == NULL) // 오른쪽 자식 노드만 있는 경우
            root = root->right; // 오른쪽 자식 노드를 그대로 올려서 연결
        else // 왼쪽 자식 노드만 있는 경우
            root = root->left; // 왼쪽 자식 노드를 그대로 올려서 연결
        free(tmp); // 노드 삭제 (메모리 해제)
    }
    return root;
}

/* 매개변수로 받은 키 값을 가지는 노드 찾기, 반환값은 1(존재함, TRUE), 0(존재하지 않음, FALSE) */
int findNode(Tree root, int key){
    if (root == NULL){ // 트리가 비었거나, 노드 탐색이 완료된 경우
            return 0;
    }
    if (key < root->value) // 찾아야 하는 값이 현재 보고 있는 루트의 값보다 작은 경우
        return findNode(root->left, key); // 왼쪽 서브 트리로 내려가서 찾아본다.
    if (key > root->value) // 반대로 찾아야 하는 값이 현재 보고 있는 루트의 값보다 큰 경우
        return findNode(root->right, key); // 오른쪽 서브 트리로 내려가서 찾아본다.
    if (root->value == key) // 
        return 1;
}

/* Inorder Traversal */
void printInorder(Tree root){
    if (root == NULL)
        return;
    /* 트리를 중위순회(왼쪽자식->부모->오른쪽자식) 하면서 값을 출력해준다. */
    printInorder(root->left); 
    fprintf(fout, "%d ", root->value);
    printInorder(root->right);
}

/* Postorder Traversal */
void deleteTree(Tree root){
    if (root == NULL)
        return;
    /* 트리를 후위순회(왼쪽자식->오른쪽자식->부모) 하면서 노드를 삭제한다. 
    루트 노드를 가장 마지막에 삭제해야 하므로 후위 순회 사용 */
    deleteTree(root->left);
    deleteTree(root->right);
    free(root);
}