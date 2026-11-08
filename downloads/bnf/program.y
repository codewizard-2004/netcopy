
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);

char *join(const char *a, const char *sep, const char *b) {
    size_t n = strlen(a) + strlen(sep) + strlen(b) + 1;
    char *result = malloc(n);
    if (!result) exit(1);
    snprintf(result, n, "%s%s%s", a, sep, b);
    return result;
}
%}

%union {
    char *str;
}

%token <str> NONTERMINAL TERMINAL
%token DEFINE OR INVALID
%type <str> rhs sequence symbol

%%
input : input rule
      | /* empty */
      ;

rule : NONTERMINAL DEFINE rhs '\n' {
    printf("%s : %s ;\n", $1, $3);
    free($1);
    free($3);
}
     | '\n'
     ;

rhs : rhs OR sequence {
        $$ = join($1, " | ", $3);
        free($1);
        free($3);
    }
    | sequence { $$ = $1; }
    ;

sequence : sequence symbol {
             $$ = join($1, " ", $2);
             free($1);
             free($2);
         }
         | symbol { $$ = $1; }
         ;

symbol : NONTERMINAL { $$ = $1; }
       | TERMINAL    { $$ = $1; }
       ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Invalid BNF: %s\n", s);
}

int main() {
    printf("Enter BNF rules:\n");
    return yyparse();
}
