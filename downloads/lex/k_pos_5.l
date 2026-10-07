%{
#include <stdio.h>
%}

%%

^[a-zA-Z]{4}k[a-zA-Z]*$    { printf("Accepted\n"); }

.*                          { printf("Rejected\n"); }

%%

int yywrap()
{
    return 1;
}

int main()
{
    printf("Enter a string: ");
    yylex();

    return 0;
}