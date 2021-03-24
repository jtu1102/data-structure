#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ISFULL 1
#define ISEMPTY 2
#define DIVIDEBYZERO 3

FILE *fin;
FILE *fout;

typedef struct Stack{
    int *key;
    int top;
    int max_stack_size;
}Stack;

Stack* CreateStack(int max);
void Push(Stack* S, int X);
int Pop(Stack* S);
int Top(Stack* S);

void DeleteStack(Stack* S);
int IsEmpty(Stack* S);
int IsFull(Stack* S);

void main(int argc, char *argv[]){
    fin = fopen(argv[1], "r");
    fout = fopen(argv[2], "w");

    Stack* stack;
    char input_str[101];
    int max = 20, i = 0, a, b, result;
    int error_flag = 0; // 0은 에러가 없음을 의미

    fgets(input_str, 101, fin);
    stack = CreateStack(max); // max stack size 20으로 새로운 스택 생성
    fprintf(fout, "Top Numbers :"); // 왼쪽에 공백을 붙이고 출력하기 위해서 콜론 뒤에 공백을 없앴습니다.
    while(input_str[i] != '#'){
        if (input_str[i] >= '0' && input_str[i] <= '9'){ // 숫자인 경우 스택에 push
            if (IsFull(stack)) // push할 공간이 남아 있는지 확인
                error_flag = ISFULL; // 스택이 full상태일 때에 대한 에러 플래그 변경
            else{ //스택에 숫자를 push하는 것이 가능할 때
            Push(stack, input_str[i] - '0'); // 문자열을 정수 형태로 바꾸어서 저장한다.
            }
        }
        else // 연산자인 경우 스택에서 숫자 두개를 pop한 후 연산하고 결과를 스택에 다시 push
        {
            if (IsEmpty(stack)) // pop할 숫자가 있는지 확인
                error_flag = ISEMPTY; // 스택이 비었다면 에러 플래그를 그에 맞게 변경
            else
                a = Pop(stack); // 첫번째 숫자 pop
            if (IsEmpty(stack)) // 두번째 숫자도 마찬가지로 스택이 비었는지 확인한 후 pop
                error_flag = ISEMPTY;
            else
                b = Pop(stack);
            if (error_flag != ISEMPTY){ // 에러 플래그가 0이어서 두 개의 숫자를 정상적으로 pop한 경우에만 연산 진행
                switch(input_str[i]){ // 현재 문자열에 해당하는 연산 진행
                        case '+':
                                result = b + a; // 연산
                                Push(stack, result); //연산 결과 스택에 push
                                break;
                        case '-':
                                result = b - a;
                                Push(stack, result);
                                break;
                        case '*':
                                result = b * a;
                                Push(stack, result);
                                break;
                        case '/':
                                if (a == 0){ // 나누는 수가 0일때에 대한 처리
                                    error_flag = DIVIDEBYZERO; // 에러 플래그를 해당하는 숫자로 변경
                                    break;
                                }
                                result = b / a; 
                                Push(stack, result);
                                break;
                        case '%':
                                if (a == 0){ // 나머지 연산도 마찬가지로 a가 0일 때 오류
                                    error_flag = DIVIDEBYZERO;
                                    break;
                                }
                                result = b % a;
                                Push(stack, result);
                                break;
                        default:
                                break;
                    }
            }
        }
        if (error_flag) // 에러의 경우 종료
            break;
        fprintf(fout, " %d", Top(stack)); // 현재 스택의 가장 위에 있는 요소를 출력, 왼쪽에 공백으로 구분
        i++;
    }
    fprintf(fout, "\n"); // 둘째줄로 내려감
    if (error_flag){ // 각각의 에러에 따른 에러 메시지 출력
        switch(error_flag){
            case ISFULL:
                fprintf(fout, "Error : Invalid Postfix Expression, Stack is Full!\n");
                break;
            case ISEMPTY:
                fprintf(fout, "Error : Invalid Postfix Expression, Stack is Empty!\n");
                break;
            case DIVIDEBYZERO:
                fprintf(fout, "Error : Invalid Postfix Expression, Divide by Zero!\n");
                break;
            default:
                break;
        }
    }
    else{
        if (stack->top) // top인덱스가 0 이상이라는 것은 스택에 남아 있는 요소가 1개보다 많다는 뜻
            fprintf(fout, "Error : Invalid Postfix Expression, %d numbers are left!\n", stack->top + 1);
        else // top인덱스가 0이면 정상적으로 연산이 완료된 것이므로 그 요소를 꺼내서 출력
            fprintf(fout, "Evaluation Result : %d\n", Pop(stack));
    }
    fclose(fin); // 파일 디스크립터를 닫아주고 
    fclose(fout);
    DeleteStack(stack); // 스택을 삭제(메모리 해제)
}

/* Create a new Stack */
Stack* CreateStack(int max){
    Stack* S = NULL;
    S = (Stack*)malloc(sizeof(Stack));
    S->key = (int*)malloc(sizeof(int) * max);
    S->max_stack_size = max;
    S->top = -1;
    return S;
}
/* Push data 'X' in the stack */
void Push(Stack* S, int X){
    S->key[++S->top] = X;
}

/* Pop top data of the stack */
int Pop(Stack* S){
    int ret;
    ret = S->key[S->top--]; // 가장 위의 데이터를 꺼내고 top인덱스를 하나 감소시킨다.
    return (ret);
}

/* return Top element of the stack */
int Top(Stack* S){
    return (S->key[S->top]);
}

/* Delete the stack (free memory) */
void DeleteStack(Stack* S){
    free(S->key);
    free(S);
}

/* if the stack is empty, return 1, else return 0 */
int IsEmpty(Stack* S){
    if (S->top == -1)
        return (1);
    return (0);
}

/* if the stack is full, return 1, else return 0 */
int IsFull(Stack* S){
    if (S->top == S->max_stack_size - 1) // 스택이 가득 차면 top인덱스가 스택 사이즈 - 1과 같다.
        return (1);
    return (0);
}