#include<stdio.h>
#include<stdlib.h>

FILE* fin;
FILE* fout;

typedef struct Array Array;

struct Array{
    int size;
    int* values;
};

Array* CreateArray(int size);
void QuickSort(Array* array, int left, int right);
int Partition(Array* array, int left, int right);
void PrintArray(Array* array, int left, int right);
void DeleteArray(Array* array);

int main(int argc, char *argv[]){
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    int size, i;
    Array* array;

    fscanf(fin, "%d", &size);
    array = CreateArray(size);
    for (i = 0; i < size; i++){
        fscanf(fin, "%d", &array->values[i]);
    }

    QuickSort(array, 0, size - 1);

    fprintf(fout, "\nSorting Result: \n");
    PrintArray(array, 0, size - 1);

    DeleteArray(array);

    fclose(fin);
    fclose(fout);
    
    return 0;
}

/* Create a new Array */
Array* CreateArray(int size){
    Array* new;
    new = (Array *)malloc(sizeof(Array));
    new->size = size;
    new->values = (int *)malloc(sizeof(int) * size);
    return new;
}

/* Perform quick sort on the interval [left, right] */
void QuickSort(Array* array, int left, int right){
    if (left < right){
        int pivot;
        /* divide */
        pivot = Partition(array, left, right); // pivot을 기준으로 구간을 나누어 swap된 배열의
        /* conquer */
        QuickSort(array, left, pivot - 1); // 왼쪽 구간에서 다시 sort
        QuickSort(array, pivot + 1, right); // 오른쪽 구간에서 다시 sort
    }
}

/* Set the pivot and obtain the proper index of the pivot through the swapping */
int Partition(Array* array, int left, int right){
    int p, i, j;
    p = array->values[right]; // pivot value is the rightmost value of the interval
    fprintf(fout, "Left: %d, Right: %d, Pivot: %d\n", left, right, p);
    PrintArray(array, left, right);
    i = left;
    j = right - 1;
    while (1){
        while (i < right && array->values[i] <= p) i++; // pivot 왼쪽 구간에는 pivot값보다 작거나 같은 값들이 와야 함. i는 right보다 작아야 구간 밖으로 넘어가지 않음
        while (j >= left && array->values[j] > p) j--; // pivot 오른쪽 구간에는 pivot값보다 큰 값들이 와야 함. j는 left보다 크거나 같아야 구간 밖으로 넘어가지 않음
        // 인덱스 조건을 구간 조건보다 앞에 써줘야 음수 인덱스를 참조하는 문제가 발생하지 않는다. (valgrind error)
        if (i < j){ // i 와 j가 역전되지 않은 경우에, i와 j를 swap
            int tmp;
            tmp = array->values[i];
            array->values[i] = array->values[j];
            array->values[j] = tmp;
            PrintArray(array, left, right);
        }
        else{ // swap element of index i with the pivot
            if (i != right){ // i가 pivot의 인덱스와 다를 때만 swap해주고 결과 출력해줌
                array->values[right] = array->values[i];
                array->values[i] = p;
                PrintArray(array, left, right);
            }
            return i; // return index of the pivot
        }
    }
}

/* Print all values of the array on the interval [left, right] */
void PrintArray(Array* array, int left, int right){
    int i;
    for (i = 0; i < array->size; ++i){
        if (left <= i && i <= right){
            fprintf(fout, "%-3d", array->values[i]);
        }
        else{
            fprintf(fout, "%-3s", "-"); // print '-' if the element is now out of the interval
        }
    }
    fprintf(fout, "\n");
}

/* Delete an array */
void DeleteArray(Array* array){
    free(array->values);
    free(array);
}