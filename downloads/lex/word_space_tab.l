%{
#include <stdio.h>

int words = 0;
int spaces = 0;
int tabs = 0;
%}

%%

[ \t]+ {
    for (int i = 0; yytext[i] != '\0'; i++)
    {
        if (yytext[i] == ' ')
            spaces++;
        else if (yytext[i] == '\t')
            tabs++;
    }
}

[a-zA-Z0-9]+ {
    words++;
}

\n {
    /* Ignore newline */
}

. {
    /* Ignore other characters */
}

%%

int yywrap()
{
    return 1;
}

int main()
{
    printf("Enter text:\n");

    yylex();

    printf("\nNumber of words  = %d", words);
    printf("\nNumber of spaces = %d", spaces);
    printf("\nNumber of tabs   = %d\n", tabs);

    return 0;
}