#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX 100
#define KEYWORDS 7

char *keywords[] = {
    "int", "float", "char", "if", "else", "while", "return"
};

int isKeyword(char *token) {
    for (int i = 0; i < KEYWORDS; i++) {
        if (strcmp(token, keywords[i]) == 0)
            return 1;
    }

    return 0;
}

// The first letter should be an alphabet or _ and then either alpanum or _ allowed
int isIdentifier(char *token) {
    if (!isalpha(token[0]) && token[0] != '_')
        return 0;

    for (int i = 1; token[i] != '\0'; i++) {
        if (!isalnum(token[i]) && token[i] != '_')
            return 0;
    }

    return 1;
}

int isNumber(char *token) {
    if (token[0] == '\0')
        return 0;

    for (int i = 0; token[i] != '\0'; i++) {
        if (!isdigit(token[i]))
            return 0;
    }

    return 1;
}

int isOperator(char ch) {
    return ch == '+' ||
           ch == '-' ||
           ch == '*' ||
           ch == '/' ||
           ch == '=' ||
           ch == '<' ||
           ch == '>' ||
           ch == '!' ||
           ch == '%' ||
           ch == '&' ||
           ch == '|';
}

int isTwoCharOperator(char a, char b) {
    char pair[3] = {a, b, '\0'};

    return strcmp(pair, ">=") == 0 ||
           strcmp(pair, "<=") == 0 ||
           strcmp(pair, "==") == 0 ||
           strcmp(pair, "!=") == 0 ||
           strcmp(pair, "&&") == 0 ||
           strcmp(pair, "||") == 0 ||
           strcmp(pair, "+=") == 0 ||
           strcmp(pair, "-=") == 0 ||
           strcmp(pair, "*=") == 0 ||
           strcmp(pair, "/=") == 0 ||
           strcmp(pair, "%=") == 0 ||
           strcmp(pair, "++") == 0 ||
           strcmp(pair, "--") == 0;
}

int isSeparator(char ch) {
    return ch == '(' ||
           ch == ')' ||
           ch == '{' ||
           ch == '}' ||
           ch == ';' ||
           ch == ',';
}

void classifyToken(char *token) {
    if (token[0] == '\0')
        return;

    if (isKeyword(token))
        printf("%s:\tKEYWORD\n", token);

    else if (isIdentifier(token))
        printf("%s:\tIDENTIFIER\n", token);

    else if (isNumber(token))
        printf("%s:\tNUMBER\n", token);

    else
        printf("%s:\tINVALID TOKEN\n", token);
}

void flushToken(char *token, int *i) {
    if (*i > 0) {
        token[*i] = '\0';
        classifyToken(token);
        *i = 0;
    }
}

int main() {
    FILE *fp = fopen("input.c", "r");

    if (fp == NULL) {
        printf("Cannot open file\n");
        return 1;
    }

    char ch;
    char next;
    char token[MAX];
    int i = 0;

    while ((ch = fgetc(fp)) != EOF) {
        /* Ignore whitespace */
        if (isspace(ch)) {
            flushToken(token, &i);
            continue;
        }

        if (ch == '/') {
            next = fgetc(fp);

            /* Ignore single-line comments */
            if (next == '/') {
                while ((ch = fgetc(fp)) != EOF && ch != '\n');
                continue;
            }

            /* Ignore multi-line comments */
            if (next == '*') {
                while ((ch = fgetc(fp)) != EOF) {
                    if (ch == '*') {
                        next = fgetc(fp);
                        if (next == '/')
                            break;
                        if (next != EOF)
                            ungetc(next, fp);
                    }
                }
                continue;
            }

            if (next != EOF)
                ungetc(next, fp);
        }

        /* Word-like token */
        if (isalnum(ch) || ch == '_') {
            token[i++] = ch;
        }

        /* Operator */
        else if (isOperator(ch)) {
            flushToken(token, &i);

            char next = fgetc(fp);
            if (next != EOF && isTwoCharOperator(ch, next)) {
                char op[3] = {ch, next, '\0'};
                printf("%s:\tOPERATOR\n", op);
            } else {
                printf("%c:\tOPERATOR\n", ch);
                if (next != EOF) {
                    ungetc(next, fp);
                }
            }
        }

        /* Separator */
        else if (isSeparator(ch)) {
            flushToken(token, &i);
            printf("%c:\tSEPARATOR\n", ch);
        }

        else {
            printf("%c:\tINVALID CHARACTER\n", ch);
        }
    }

    /* Process remaining token */
    if (i > 0) {
        token[i] = '\0';
        classifyToken(token);
    }

    fclose(fp);

    return 0;
}