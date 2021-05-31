#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define Del -1 // hash function으로 mod 연산을 활용하므로 -1의 값은 key값이 될 수 없음

FILE *fin;
FILE *fout;

typedef int ElementType;
typedef ElementType List;
typedef struct HashTbl* HashTable;

typedef struct HashTbl{
    int TableSize;
    List *TheLists;
}HashTbl;

HashTable createTable(int TableSize);
void Insert(HashTable H, ElementType key, int solution);
void Delete(HashTable H, ElementType key, int solution);
int Find(HashTable H, ElementType key, int solution);
void printTable(HashTable H);
void deleteTable(HashTable H);

int main(int argc, char *argv[]){
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");


    char solution_str[20];
    int solution, TableSize;

    fscanf(fin, "%s", solution_str);
    if(!strcmp(solution_str, "Linear"))
        solution = 1;
    else if(!strcmp(solution_str, "Quadratic"))
        solution = 2;
    else{
        fprintf(fout, "Error: Invalid hashing solution!");
        return 0;
    }

    fscanf(fin, "%d", &TableSize);

    HashTable H = createTable(TableSize);

    char cv;
    int key;
    while(!feof(fin)){
        fscanf(fin, "%c", &cv);
        switch(cv){
            case 'i':
                fscanf(fin, "%d", &key);
                Insert(H, key, solution);
                break;
            
            case 'f':
                fscanf(fin, "%d", &key);
                int result = Find(H, key, solution);
                if(result)
                    fprintf(fout, "%d is in the table\n", key);
                else
                    fprintf(fout, "%d is not in the table\n", key);
                break;
            
            case 'd':
                fscanf(fin, "%d", &key);
                Delete(H, key, solution);
                break;
            
            case 'p':
                printTable(H);
                break;
        }
    }

    deleteTable(H);

    fclose(fin);
    fclose(fout);

    return 0;
}

/* Creat a new Hash Table */
HashTable createTable(int TableSize){
    HashTable H;
    int i;

    H = (HashTable)malloc(sizeof(HashTbl));
    H->TableSize = TableSize;
    H->TheLists = (int *)malloc(sizeof(int) * TableSize);
    for (i = 0; i < TableSize; ++i) {
        H->TheLists[i] = 0; // initialize, 0 means that there have never been a value.
    }
    return H;
}

/* Insert value into the hash table */
void Insert(HashTable H, ElementType key, int solution){
    int i, k;

    /* Linear probing */
    if (solution == 1) {
        i = key % H->TableSize; // 시작 인덱스
        for (k = 0; !(H->TheLists[i] == 0 || H->TheLists[i] == Del); ++k) { // 0 또는 Del 상태인 경우 값을 넣을 수 있음
            i = ((key % H->TableSize) + k) % H->TableSize; // 한 칸씩 다음 칸을 보면서 빈자리 찾기
            if (k > H->TableSize) { // loop 횟수가 table size를 넘은 경우 : hash table이 가득 참
                fprintf(fout, "Insertion Error: table is full\n");
                return;
            }
            if (H->TheLists[i] == key) { // 키값 중복 체크
                fprintf(fout, "Insertion Error: %d already exists at address %d\n", key, i);
                return;
            }
        }
    }
    /* Quadratic probing */
    else {
        i = key % H->TableSize; // 시작 인덱스
        for (k = 0; !(H->TheLists[i] == 0 || H->TheLists[i] == Del); ++k) { // 0 또는 Del 상태인 경우 값을 넣을 수 있음
            i = ((key % H->TableSize) + (k * k)) % H->TableSize; // k^2 칸씩 다음 칸을 보면서 빈자리 찾기
            if (k > H->TableSize) { // loop 횟수가 table size를 넘은 경우 : hash table이 가득 참
                fprintf(fout, "Insertion Error: table is full\n");
                return;
            }
            if (H->TheLists[i] == key) { // 키값 중복 체크
                fprintf(fout, "Insertion Error: %d already exists at address %d\n", key, i);
                return;
            }
        }
    }
    H->TheLists[i] = key;
    fprintf(fout, "Insert %d into address %d\n", key, i);
}

/* Delete the value from the hash table */
void Delete(HashTable H, ElementType key, int solution) {
    int i, k;

    /* Linear probing */
    if (solution == 1) {
        i = key % H->TableSize; // 시작 인덱스
        for (k = 0; H->TheLists[i]; ++k) {
            i = ((key % H->TableSize) + k) % H->TableSize; // 한 칸씩 다음 칸을 보면서 같은 값 찾기
            if (k > H->TableSize)
                break;
            if (H->TheLists[i] == key) { // 삭제할 값을 찾은 경우
                H->TheLists[i] = Del; // 삭제 되었음을 표시하는 값으로 바꾼다
                fprintf(fout, "Delete %d\n", key);
                return;
            }
        }
    }
    /* Quadratic probing */
    else {
        i = key % H->TableSize; // 시작 인덱스
        for (k = 0; H->TheLists[i]; ++k) {
            i = ((key % H->TableSize) + (k * k)) % H->TableSize; // k^2 칸씩 다음 칸을 보면서 같은 값 찾기
            if (k > H->TableSize)
                break;
            if (H->TheLists[i] == key) { // 삭제할 값을 찾은 경우
                H->TheLists[i] = Del; // 삭제 되었음을 표시하는 값으로 바꾼다
                fprintf(fout, "Delete %d\n", key);
                return;
            }
        }
    }
    /* 삭제할 값을 못 찾은 경우 */
    fprintf(fout, "Deletion Error: %d is not in the table\n", key);
}

/* Find the value, return 1 if the key exists */
int Find(HashTable H, ElementType key, int solution) {
    int i, k;

    /* Linear probing */
    if (solution == 1) {
        i = key % H->TableSize; // 시작 인덱스
        for (k = 0; H->TheLists[i]; ++k) { // key값을 못 찾고 0을 만난 경우, key값이 hash table에 없는 것이므로 탐색 종료
            i = ((key % H->TableSize) + k) % H->TableSize; // 한 칸씩 다음 칸을 보면서 빈자리 찾기
            if (k > H->TableSize) // 원래 인덱스로 돌아온 경우 key값이 hash table에 없는 것이므로 탐색 종료
                break;
            if (H->TheLists[i] == key) { // 값을 찾은 경우
                return 1;
            }
        }
    }
    /* Quadratic probing */
    else {
        i = key % H->TableSize; // 시작 인덱스
        for (k = 0; H->TheLists[i]; ++k) { // key값을 못 찾고 0을 만난 경우, key값이 hash table에 없는 것이므로 탐색 종료
            i = ((key % H->TableSize) + (k * k)) % H->TableSize; // k^2 칸씩 다음 칸을 보면서 빈자리 찾기
            if (k > H->TableSize) // 원래 인덱스로 돌아온 경우 key값이 hash table에 없는 것이므로 탐색 종료
                break;
            if (H->TheLists[i] == key) { // 값을 찾은 경우
                return 1;
            }
        }
    }
    /* key값이 hash table에 없는 경우 */
    return 0;
}

/* Print all values from a hash table */
void printTable(HashTable H) {
    int i;
    
    for (i = 0; i < H->TableSize; ++i) {
        if (H->TheLists[i] == Del) // Del 상태인 경우에도 값이 없는 것이므로 0 출력
            fprintf(fout, "%d ", 0);
        else
            fprintf(fout, "%d ", H->TheLists[i]);
    }
    fprintf(fout, "\n");
}
 /* Delete a hash table */
void deleteTable(HashTable H) {
    free(H->TheLists);
    free(H);
}