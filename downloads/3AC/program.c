
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char values[100][20];
char ops[100];
int top = -1, opTop = -1, temp = 1;

// Push operand onto stack
void push(char str[]) {
    strcpy(values[++top], str);
}

// Generate three address code
void generate() {
    char b[20], a[20], result[20];
    char op = ops[opTop--];

    strcpy(b, values[top--]);
    strcpy(a, values[top--]);

    sprintf(result, "t%d", temp++);

    printf("%s = %s %c %s\n", result, a, op, b);

    push(result);
}

// Operator precedence
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

int main() {
    char expr[100], lhs[20], operand[20];
    int i = 0, j;

    printf("Enter expression: ");
    scanf("%99s", expr);

    // Extract left-hand side
    while (expr[i] != '=' && expr[i] != '\0') {
        i++;
    }

    if (expr[i] == '\0') {
        printf("Invalid assignment\n");
        return 1;
    }

    strncpy(lhs, expr, i);
    lhs[i] = '\0';
    i++;

    // Process right-hand side
    while (expr[i] != '\0') {
        char c = expr[i];

        if (isalnum(c) || c == '_') {
            j = 0;
            while (isalnum(expr[i]) || expr[i] == '_') {
                operand[j++] = expr[i++];
            }
            operand[j] = '\0';
            push(operand);
            continue;
        }

        if (c == '(') {
            ops[++opTop] = c;
        }
        else if (c == ')') {
            while (opTop >= 0 && ops[opTop] != '(')
                generate();
            opTop--;
        }
        else if (c == '+' || c == '-' ||
            c == '*' || c == '/') {
            while (opTop >= 0 &&
                ops[opTop] != '(' &&
                precedence(ops[opTop]) >= precedence(c)) {
                generate();
                }
                ops[++opTop] = c;
            }
            i++;
    }

    // Process remaining operators
    while (opTop >= 0) {
        generate();
    }

    // Final assignment
    printf("%s = %s\n", lhs, values[top]);

    return 0;
}
