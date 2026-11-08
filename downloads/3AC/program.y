
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);

int tempCount = 0;

char *newTemp() {
    char *t = malloc(20);
    sprintf(t, "t%d", ++tempCount);
    return t;
}
%}

%union {
    char *str;
}

%token <str> ID NUM
%type <str> expr

%left '+' '-'
%left '*' '/'
%right UMINUS

%%
input : input stmt
      | /* empty */
      ;

stmt : ID '=' expr '\n' {
    printf("%s = %s\n", $1, $3);
}
     ;

expr : expr '+' expr {
    $$ = newTemp();
    printf("%s = %s + %s\n", $$, $1, $3);
}
     | expr '-' expr {
    $$ = newTemp();
    printf("%s = %s - %s\n", $$, $1, $3);
}
     | expr '*' expr {
    $$ = newTemp();
    printf("%s = %s * %s\n", $$, $1, $3);
}
     | expr '/' expr {
    $$ = newTemp();
    printf("%s = %s / %s\n", $$, $1, $3);
}
     | '-' expr %prec UMINUS {
    $$ = newTemp();
    printf("%s = -%s\n", $$, $2);
}
     | '(' expr ')' { $$ = $2; }
     | ID           { $$ = $1; }
     | NUM          { $$ = $1; }
     ;
%%

void yyerror(const char *s) {
    printf("Invalid expression\n");
}

int main() {
    printf("Enter expressions:\n");
    yyparse();
    return 0;
}
