%option noyywrap

%{
#include <stdio.h>
%}

%%
"hello"     printf("Hello detected!\n");
.           printf("Other character: %s\n", yytext);
%%

int main()
{
    yylex();
    return 0;
}