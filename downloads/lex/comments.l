%{
#include <stdio.h>
%}

%%

"//".*                  { /* Delete single-line comment */ }

"/*"([^*]|\*+[^*/])*\*+"/" {
                            /* Delete multi-line comment */
                         }

\n                      { printf("\n"); }

.                       { printf("%s", yytext); }

%%

int yywrap()
{
    return 1;
}

int main()
{
    printf("Enter the source code:\n");
    yylex();

    return 0;
}