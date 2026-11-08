
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char input[200];
int pos = 0, tempCount = 0;

char *expr();
char *term();
char *factor();

char *newTemp() {
    char *t = malloc(20);
    sprintf(t, "t%d", ++tempCount);
    return t;
}

void skipSpaces() {
    while (input[pos] == ' ' || input[pos] == '\t')
        pos++;
}

char *generate(char *a, char op, char *b) {
    char *t = newTemp();
    printf("%s = %s %c %s\n", t, a, op, b);
    return t;
}

char *factor() {
    skipSpaces();

    if (input[pos] == '(') {
        pos++;
        char *t = expr();
        skipSpaces();
        if (input[pos] != ')') {
            printf("Invalid expression\n");
            exit(1);
        }
        pos++;
        return t;
    }

    if (input[pos] == '-') {
        pos++;
        char *a = factor();
        char *t = newTemp();
        printf("%s = -%s\n", t, a);
        return t;
    }

    if (isalnum((unsigned char)input[pos]) || input[pos] == '_') {
        char *t = malloc(100);
        int i = 0;

        while (isalnum((unsigned char)input[pos]) ||
            input[pos] == '_') {
            if (i >= 99) {
                printf("Token too long\n");
                exit(1);
            }
            t[i++] = input[pos++];
            }

            t[i] = '\0';
        return t;
    }

    printf("Invalid expression\n");
    exit(1);
}

char *term() {
    char *left = factor();
    skipSpaces();

    while (input[pos] == '*' || input[pos] == '/') {
        char op = input[pos++];
        char *right = factor();
        left = generate(left, op, right);
        skipSpaces();
    }

    return left;
}

char *expr() {
    char *left = term();
    skipSpaces();

    while (input[pos] == '+' || input[pos] == '-') {
        char op = input[pos++];
        char *right = term();
        left = generate(left, op, right);
        skipSpaces();
    }

    return left;
}

int main() {
    char variable[100];

    printf("Enter expression (e.g. a=b+c*d): ");
    fgets(input, sizeof(input), stdin);

    skipSpaces();

    int i = 0;
    if (!isalpha((unsigned char)input[pos]) &&
        input[pos] != '_') {
        printf("Invalid assignment\n");
    return 1;
        }

        while (isalnum((unsigned char)input[pos]) ||
            input[pos] == '_') {
            if (i >= 99) {
                printf("Identifier too long\n");
                return 1;
            }
            variable[i++] = input[pos++];
            }
            variable[i] = '\0';

        skipSpaces();

        if (input[pos] != '=') {
            printf("Invalid assignment\n");
            return 1;
        }

        pos++;

        printf("\nThree Address Code:\n");

        char *result = expr();

        skipSpaces();
        if (input[pos] != '\n' && input[pos] != '\0') {
            printf("Invalid expression\n");
            return 1;
        }

        printf("%s = %s\n", variable, result);

        return 0;
}
