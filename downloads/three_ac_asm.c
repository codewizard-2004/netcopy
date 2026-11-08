
#include <stdio.h>
#include <string.h>

int main() {
    int n;
    char tac[100];
    char result[20], op1[20], op2[20], op;

    printf("Enter number of instructions: ");
    scanf("%d", &n);
    getchar();

    printf("Enter three address code:\n");

    for (int i = 0; i < n; i++) {
        fgets(tac, sizeof(tac), stdin);

        if (sscanf(tac, " %19s = %19s %c %19s",
                   result, op1, &op, op2) == 4) {

            printf("MOV AX, %s\n", op1);

            switch (op) {
                case '+':
                    printf("ADD AX, %s\n", op2);
                    break;

                case '-':
                    printf("SUB AX, %s\n", op2);
                    break;

                case '*':
                    printf("MOV BX, %s\n", op2);
                    printf("MUL BX\n");
                    break;

                case '/':
                    printf("MOV BX, %s\n", op2);
                    printf("MOV DX, 0\n");
                    printf("DIV BX\n");
                    break;

                default:
                    printf("Invalid operator\n");
                    continue;
            }

            printf("MOV %s, AX\n\n", result);
        }
        else if (sscanf(tac, " %19s = %19s",
                        result, op1) == 2) {
            printf("MOV AX, %s\n", op1);
            printf("MOV %s, AX\n\n", result);
        }
    }

    return 0;
}
