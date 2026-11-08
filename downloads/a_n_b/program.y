
%{
#include <stdio.h>

int yylex();
void yyerror(const char *s);
%}

%%
start : S '\n' { printf("Valid String\n"); }
      ;

S : 'a' S
  | 'b'
  ;
%%

void yyerror(const char *s) {
    printf("Invalid String\n");
}

int main() {
    printf("Enter a string: ");
    yyparse();
    return 0;
}
