%{
#include <stdio.h>
%}

%%

^11[01]*101$    { printf("Accepted\n"); }

.*              { printf("Rejected\n"); }

%%

int yywrap()
{
    return 1;
}

int main()
{
    printf("Enter a binary string: ");
    yylex();

    return 0;
}

/*
String staring with 11 and ending with 101
*/