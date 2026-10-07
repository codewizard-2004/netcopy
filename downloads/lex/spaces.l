%{
#include <stdio.h>
%}

%%

" "     { printf("$"); }

\n      { printf("\n"); }

.       { printf("%s", yytext); }

%%

int yywrap()
{
    return 1;
}

int main()
{
    printf("Enter the C program:\n");
    yylex();

    return 0;
}