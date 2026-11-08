
%{
#include <stdio.h>
#include <string.h>

char old[100], newstr[100];
%}

%%
[a-zA-Z]+ {
    char *p = yytext;
    char *pos;

    while ((pos = strstr(p, old)) != NULL) {
        printf("%.*s%s", (int)(pos - p), p, newstr);
        p = pos + strlen(old);
    }

    printf("%s", p);
}

.|\n { ECHO; }
%%

int yywrap() {
    return 1;
}

int main() {
    printf("Enter substring to replace: ");
    scanf("%99s", old);

    printf("Enter replacement substring: ");
    scanf("%99s", newstr);

    getchar();

    printf("Enter input string:\n");
    yylex();

    return 0;
}
