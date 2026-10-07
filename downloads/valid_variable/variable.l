%{
#include "variable.tab.h"
%}

%option noyywrap

%%

[a-zA-Z]       { return LETTER; }

[0-9]          { return DIGIT; }

\n             { return 0; }

.              { return yytext[0]; }

%%

/*
bison -d variable.y
flex variable.l
gcc lex.yy.c variable.tab.c -o variable
*/