#include<stdio.h>
#include<string.h>
#include<ctype.h>

// Create an array containing all the keywords
char *keywords[] = {
    "int", "float", "char", "double", "void",
    "if", "else", "while", "for", "do",
    "return", "switch", "continue", "break",
    "case"
};

// function to check if a string buffer is a keyword
int isKeyword(char buffer[]){
    int n = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < n; i++){
        if (strcmp(buffer, keywords[i]) == 0){
            return 1;
        }
    }
    return 0;
}

int main(){
    char ch, buffer[100];
    char operators[] = "+-*/=<>";
    char special[] = ";,{}()[]";
    int i;

    printf("Enter the c program:\n");

    while ((ch = getchar()) != EOF){
        // Ignore spaces, tabs and newlines
        if (isspace(ch)){
            continue;
        }

        // Ignore comments
        if (ch == '/'){
            char next = getchar();
            // If its a single line comment ignore until a newline or EOF
            if (next == '/'){
                while((ch == getchar()) != '\n' && ch != EOF){
                    continue;
                }
            }
            // If its a multiline comment ignore until */ is found
            if (next == '*'){
                char prev = 0;
                while ((ch == getchar()) != EOF){
                    if (prev == '*' && ch == '/'){
                        break;
                    }
                    prev = ch;
                }
                continue;
            }

            if (next != EOF){
                ungetc(next, stdin);
            }

        }

        // Check for operators
        if (strchr(operators, ch)){
            printf("%c:\t Operator\n", ch);
            continue;
        }
        // Check for special characters
        if (strchr(special, ch)){
            printf("%c:\t Special Character\n");
            continue;
        }
        // Check for keyword and identifiers
        if (isalpha(ch) || ch == '_'){
            i = 0;

            do {
                if (i < 99) buffer[i++] = ch;
                ch = getchar();
            }while (ch != EOF && (isalnum(ch) || ch == '_'));

            buffer[i] = '\0';

            if (ch != EOF){
                ungetc(ch, stdin);
            }

            if (isKeyword(buffer)){
                printf("%s:\tKeyword\n", buffer);
            }else{
                printf("%s:\tIdentifier\n", buffer);
            }

            continue;
        }

        // Numeric Constants
        if (isdigit(ch)){
            i = 0;
            do{
                if (i < 99) buffer[i++] = ch;
                ch = getchar();
            }while(ch != EOF && (isdigit(ch) || ch == '.'));

            buffer[i] = '\0';

            if (ch != EOF){
                ungetc(ch, stdin);
            }

            printf("%s:\tNumeric Constant\n", buffer);
            continue;
        }

        printf("%c:\tUnknown Token\n", ch);

    }

    return 0;
}
