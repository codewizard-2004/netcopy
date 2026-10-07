%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

%union {
    double dval;
}

%token <dval> NUMBER
%type <dval> expr term factor

%left '+' '-'
%left '*' '/'
%right UMINUS

%%

input:
      /* empty */
    | input line
    ;

line:
      '\n'
    | expr '\n'   { printf("Result = %.2f\n", $1); }
    ;

expr:
      expr '+' term  { $$ = $1 + $3; }
    | expr '-' term  { $$ = $1 - $3; }
    | term           { $$ = $1; }
    ;

term:
      term '*' factor  { $$ = $1 * $3; }
    | term '/' factor
      {
          if ($3 == 0.0) {
              yyerror("division by zero");
              $$ = 0.0;
          } else {
              $$ = $1 / $3;
          }
      }
    | factor           { $$ = $1; }
    ;

factor:
      '(' expr ')'     { $$ = $2; }
    | '-' factor %prec UMINUS { $$ = -$2; }
    | NUMBER           { $$ = $1; }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Error: %s\n", s);
}

int main(void)
{
    printf("Enter arithmetic expressions (one per line).\n");
    printf("Supported: +, -, *, /, parentheses, unary minus.\n");
    printf("Press Ctrl+D to exit.\n");
    return yyparse();
}
