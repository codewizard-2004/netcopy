#include<stdio.h>
#include<string.h>

#define MAX 100

char stack[MAX] = "$";
int top = 0;

char operators[] = "i+*$";

//operator precedence table
char table[4][4] ={
      //i    +    *    $  
/*i*/ {' ', '>', '>', '>'},  
/*+*/ {'<', '>', '<', '>'},
/***/ {'<', '>', '>', '>'},
/*$*/ {'<', '<', '<', 'A'}
};

int get_index(char ch){
    for (int i = 0; i < 4; i++){
        if (ch == operators[i]){
            return i;
        }
    }

    return -1;
}

void shift(char ch){
    stack[++top] = ch;
    stack[top + 1] = '\0';
}

/*
    RULES:
    E = E + E
    E = E * E
    E = i
*/

int reduce(){
    char *rules[] = {"i", "E+E", "E*E"};

    for (int i = 0; i < 3; i++){
        int len = strlen(rules[i]);

        if (top + 1 >= len && strcmp(stack + top - len + 1, rules[i]) == 0){
            top = top - len + 1;
            stack[top] = 'E';
            stack[top + 1] = '\0';

            printf("REDUCE E -> %s\n", rules[i]);
            return 1;
        }
    }
    return 0;
}

int main() {
    char input[100];
    int pos = 0;

    printf("Enter expression: ");
    scanf("%98s", input);
    strcat(input, "$");

    printf("\n%-15s %-15s ACTION\n", "STACK", "INPUT");

    while (1) {
        // Find topmost terminal
        int j = top;
        while (stack[j] == 'E')
            j--;

        char a = stack[j];
        char b = input[pos];

        int row = get_index(a);
        int col = get_index(b);

        if (row == -1 || col == -1) {
            printf("INVALID SYMBOL\n");
            break;
        }

        char relation = table[row][col];

        printf("%-15s %-15s ", stack, input + pos);

        if (relation == '<') {
            printf("SHIFT %c\n", b);
            shift(b);
            pos++;
        }
        else if (relation == '>') {
            if (!reduce()) {
                printf("REJECTED\n");
                break;
            }
        }
        else if (relation == 'A') {
            if (strcmp(stack, "$E") == 0)
                printf("ACCEPTED\n");
            else
                printf("REJECTED\n");
            break;
        }
        else {
            printf("REJECTED\n");
            break;
        }
    }

    return 0;
}