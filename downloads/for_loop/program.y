
%{
#include <stdio.h>

int yylex(void);
void yyerror(const char *s);
%}

%token FOR ID NUM RELOP INCDEC

%%
start : FOR '(' init ';' condition ';' update ')' {
    printf("Valid For Loop\n");
}
;

init : ID '=' NUM
     | ID '=' ID
     ;

condition : ID RELOP NUM
          | ID RELOP ID
          ;

update : ID INCDEC
       | INCDEC ID
       | ID '=' ID '+' NUM
       | ID '=' ID '-' NUM
       ;
%%

void yyerror(const char *s) {
    printf("Invalid For Loop\n");
}

int main() {
    printf("Enter a for loop: ");
    yyparse();
    return 0;
}
