#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *fin;
FILE *fout;

typedef struct _DisjointSet {
    int size_maze;
    int *ptr_arr;
}DisjointSets;

void init(DisjointSets *sets, DisjointSets *maze_print, int num);
void Union(DisjointSets *sets, int i, int j);
int find(DisjointSets *sets, int i);
void find_xy(int *xy, int edge, int num); // edge번호에 맞는 인접한 칸 번호를 저장해주는 함수
void createMaze(DisjointSets *sets, DisjointSets *maze_print, int num);
void printMaze(DisjointSets *print_maze, int num);
void freeMaze(DisjointSets *sets, DisjointSets *maze_print);
void print_first_and_last_row(int num); // 미로의 첫번째와 마지막 행 출력
void print_first_and_last_col(int num, int i, int j); // 미로의 첫번쨍와 마지막 열 출력

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));

    int num, i;
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    DisjointSets *sets, *maze_print;
    fscanf(fin, "%d", &num);

    sets = (DisjointSets*)malloc(sizeof(DisjointSets));
    maze_print = (DisjointSets*)malloc(sizeof(DisjointSets));

    init(sets, maze_print, num);
    createMaze(sets, maze_print, num);
    printMaze(maze_print, num);

    freeMaze(sets, maze_print);

    fclose(fin);
    fclose(fout);

    return 0;
}

/** edge를 표현하는 방식은 다음과 같습니다.
 * (1) vertical edge 
 *     edge의 오른쪽에 있는 칸 번호, 1부터 시작하는 가장 왼쪽 열의 칸 번호는 edge번호로 사용되지 않습니다. (미로의 가장 왼쪽 벽은 모든 미로에서 같은 규칙을 따르므로)
 * (2) horizontal edge
 *     edge의 위쪽에 있는 칸 번호 + num^2, vertical edge와의 중복을 피하기 위해 num을 제곱한 수(미로의 전체 칸 수)를 더했습니다.
**/

/* initialize */
void init(DisjointSets *sets, DisjointSets *maze_print, int num){
    int i;
    sets->size_maze = num * num;
    sets->ptr_arr = (int *)malloc(sizeof(int) * (sets->size_maze + 1)); // maze의 칸 개수만큼 배열 할당 (1번 인덱스부터 사용)
    for (i = 1; i <= sets->size_maze; ++i) // 배열 초기화, N^2개의 트리 생성
        sets->ptr_arr[i] = 0; // 모든 칸이 독립된 tree(집합)
    maze_print->size_maze = num*(num - 1) + num*num; // num * num maze에서 마지막 edge번호
    maze_print->ptr_arr = (int *)malloc(sizeof(int) * (maze_print->size_maze + 1)); // edge 번호를 담을 배열 할당
    maze_print->ptr_arr[0] = maze_print->ptr_arr[1] = -1; // 인덱스 0, 1인 노드는 루트 노드로 사용(0:벽 없음, 1:벽 없음)
    for (i = 2; i <= maze_print->size_maze; ++i)
        maze_print->ptr_arr[i] = 1; // 현재 edge는 모두 wall이므로 (막혀 있음) 루트를 1로 초기화
}

/* Union two sets */
void Union(DisjointSets *sets, int i, int j){ // i, j는 항상 루트 노드로 주어짐
    if (sets->ptr_arr[i] < sets->ptr_arr[j]) // rank가 낮은 트리를 높은 트리로 연결
        sets->ptr_arr[j] = i;
    else{
        if (sets->ptr_arr[i] == sets->ptr_arr[j]) // rank가 같은 경우
            sets->ptr_arr[j]--; // union된 트리는 rank가 하나 증가하므로 (노트의 번호인지, rank인지 구분하기 위해 음수 사용, 1을 빼 주면 rank가 하나 증가한다.)
        sets->ptr_arr[i] = j; // rank가 낮은 트리를 높은 트리로 연결
    }
}

/* find node of the tree */
int find(DisjointSets *sets, int i){ // i 노드를 갖고 있는 루트 노드 반환
    while (sets->ptr_arr[i] > 0) // 배열 값이 양수라면 parant를 나타내고 있으므로
        i = sets->ptr_arr[i];
    return i;
}

/* edge 번호를 받아서 인접한 두 칸의 번호를 계산해주는 함수 
 * edge 번호의 규칙에 따라, num^2보다 작거나 같은 번호는 vertical edge, 큰 번호는 horizontal edge를 나타냄
*/
void find_xy(int *xy, int edge, int num){
    if (edge <= num*num){ // vertical edge
        xy[0] = edge; // edge의 오른쪽 칸 번호
        xy[1] = xy[0] - 1; // edge의 왼쪽 칸 번호
    }
    else{ // horizontal edge
        xy[0] = edge - num*num; // edge의 위쪽 칸 번호
        xy[1] = xy[0] + num; // edge의 아래쪽 칸 번호
    }
}

/* create a maze */
void createMaze(DisjointSets *sets, DisjointSets *maze_print, int num){
    int xy[2]; // edge에 인접한 두 칸 번호를 저장하기 위한 배열
    int *is_used; // 사용한(확인한) edge를 표시하기 위한 배열
    int edge;
    int i, r_x, r_y;

    is_used = (int *)malloc(sizeof(int) * (maze_print->size_maze + 1));
    for (i = 2; i <= maze_print->size_maze; ++i) // 아직 전부 사용한 적 없으므로 0으로 초기화
        is_used[i] = 0;
    while(sets->size_maze != 1){ // set개수가 1개일 때까지 edge를 지운다. set개수가 1개가 되면 미로 완성!
        edge = rand() % maze_print->size_maze + 1; // 엣지 하나를 랜덤으로 고른다. 한번 고를 때마다 set개수로 조건 확인하도록 해서 무한루프 피하기
        if ((edge % num == 1 && edge <= num * num) || is_used[edge]) 
            continue; // 가장 왼쪽의 세로 엣지들은 확인 할 필요가 없는 미로의 경계 벽 이므로 제외, 이미 확인한 edge도 다시 확인 할 필요 없으므로 제외
        find_xy(xy, edge, num); // edge에 인접한 두 칸의 번호를 xy배열에 차례로 담는다.
        r_x = find(sets, xy[0]); // 두 칸 번호가 현재 속해 있는 tree의 루트 번호를 찾음
        r_y = find(sets, xy[1]);
        if (r_x != r_y){ // 두 칸이 한 트리에 있지 않다면
            Union(sets, r_x, r_y);
            maze_print->ptr_arr[edge] = 0; // 이제 두 칸 사이에 벽이 없으므로 0을 루트로 하는 트리로 옮긴다.
            sets->size_maze--; // 전체 집합의 개수(tree의 개수) 하나 줄었음
        }
        is_used[edge] = 1; // 확인한 edge표시
    }
    /* while을 빠져나오면 sets에는 하나의 tree(집합)만 존재하고, 사이클 없이 하나의 경로만 존재하는 미로가 완성됨 */
    free(is_used); // 더 이상 사용하지 않는 배열이므로 메모리 해제
}

/* 미로의 첫 행과 마지막 행은 크기와 미로 모양과 상관 없이 막혀 있어야 함 */
void print_first_and_last_row(int num){
    int j;
    for (j = 0; j < 4 * num + 1; ++j){
        if (j % 4 == 0) // 네칸마다 + 출력
            fprintf(fout, "+");
        else // 세개씩 - 출력
            fprintf(fout, "-");
    }
}

/* 미로의 첫 열과 마지막 열 프린트, 미로의 입구와 출구 만들기 */
void print_first_and_last_col(int num, int i, int j){
    if (j == 0 && i == 1)
        fprintf(fout, " "); // 미로의 입구
    else if (j == 4 * num && i == 2 * num - 1)
        fprintf(fout, " "); // 미로의 출구
    else
        fprintf(fout, "|"); // 미로의 좌우 경계    
}

void printMaze(DisjointSets *maze_print, int num){
    int i, j;
    int edge;

    for (i = 0; i < 2 * num + 1; ++i){ // 행 개수 2*num + 1 개
        if (i == 0 || i == 2 * num){ // 맨 위의 행과 맨 아래 행은 미로의 크기에 상관 없이 꽉 차 있음
            print_first_and_last_row(num); 
        }
        else{ // maze_print의 규칙에 따라 edge를 프린트한다.
            if (i % 2){ // "|" 로 시작되는 행 : vertical edge 정보 확인
                for (j = 0; j < 4 * num + 1; ++j){ // 열 개수 4 * num + 1개
                    if (j == 0 || j == 4*num){ // 맨 처음과 끝 : 미로 경계
                        print_first_and_last_col(num, i, j);
                    }
                    else if (j % 4){ // 가운데 공간
                        fprintf(fout, " "); // 공백으로 채우기
                    }
                    else{ // vertical edge가 들어갈 수 있는 공간
                        edge = (j / 4 + 1) + (i / 2 * num); // 현재 확인해야 하는 edge번호 구하기
                        if (maze_print->ptr_arr[edge]){ // 벽이 있음 (edge가 루트 1인 트리에 연결되어 있음)
                        /* maze_print도 disjoint set이므로 find함수를 사용하는 것이 일반화된 구현이지만, 이 경우에 '0'이 루트 노드로 사용되므로 sets과 다른 규칙을 따르고 있어서 같은 find함수(양수일 때 parant를 따라가도록 구현된 함수)를 사용할 수 없습니다.
                        maze_print는 createMaze함수에서 업데이트 되는 과정 특성 상 rank가 1 이상으로 증가하지 않으므로 배열에 저장된 값이 바로 루트 노드임을 이용해서, find함수 없이 바로 루트를 찾도록 구현하였습니다.*/
                            fprintf(fout, "|");
                        }
                        else{ 
                            fprintf(fout, " "); // 벽이 없음 (edge가 루트 0인 트리에 연결되어 있음)
                        }
                    }
                }
            }
            else{ // + 로 시작되는 행 : horizontal edge 정보 확인
                for (j = 0; j < 4 * num + 1; ++j){
                    if (j % 4 == 0){
                        fprintf(fout, "+"); // 4개마다 +로 막힘
                    }
                    else{ // edge가 있으면 "-", 없으면 " "을 추가해야 하는 부분
                        edge = (j / 4 + 1) + ((i - 1)/ 2 * num) + (num * num); // 현재 확인해야 하는 edge번호 구하기
                        if (maze_print->ptr_arr[edge]){ // 벽이 있는 경우
                            fprintf(fout, "-");
                        }
                        else{ // 벽이 없는 경우
                            fprintf(fout, " ");
                        }
                    }
                }
            }
        }
        fprintf(fout, "\n"); // 한 행을 프린트한 후에 개행
    }
}
 /* 동적 할당된 메모리 해제 */
void freeMaze(DisjointSets *sets, DisjointSets *maze_print){
    free(sets->ptr_arr);
    free(sets);
    free(maze_print->ptr_arr);
    free(maze_print);
}