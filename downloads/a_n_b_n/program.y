
%{
#include <stdio.h>

int yylex(void);
void yyerror(const char *s);
%}

%token A B INVALID

%%
start : S '\n' { printf("Valid String\n"); }
      ;

S : A S B
  | A B
  ;
%%

void yyerror(const char *s) {
    printf("Invalid String\n");
}

int main() {
    printf("Enter string: ");
    yyparse();
    return 0;
}
