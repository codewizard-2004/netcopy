%{
#include <stdio.h>
#include <stdlib.h>

int yylex();
void yyerror(const char *s);
%}

/* Token declaration */
%token LETTER DIGIT

%%

variable:
        LETTER rest
        {
            printf("Valid variable\n");
            return 0;
        }
        ;

rest:
        /* empty */
        |
        rest LETTER
        |
        rest DIGIT
        ;

%%

void yyerror(const char *s)
{
    printf("Invalid variable\n");
}

int main()
{
    printf("Enter a variable: ");
    yyparse();

    return 0;
}