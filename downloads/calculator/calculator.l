%option noyywrap

%{
#include "calculator.tab.h"
#include <stdlib.h>
%}

%%

[0-9]+(\.[0-9]+)?   { yylval.dval = atof(yytext); return NUMBER; }
[ \t\r]+            ;
\n                  { return '\n'; }
"+"                 { return '+'; }
"-"                 { return '-'; }
"*"                 { return '*'; }
"/"                 { return '/'; }
"("                 { return '('; }
")"                 { return ')'; }
.                   { return yytext[0]; }

%%

/*
bison -d calculator.y
flex calculator.l
gcc lex.yy.c calculator.tab.c -o calculator
./calculator
*/
