
%{
#include "program.tab.h"
%}

%%
"for"                   { return FOR; }
"++"|"--"               { return INCDEC; }
"<="|">="|"=="|"!="|"<"|">" { return RELOP; }
[0-9]+                  { return NUM; }
[a-zA-Z_][a-zA-Z0-9_]*  { return ID; }
[ \t\n]+                { /* Ignore whitespace */ }
.                       { return yytext[0]; }
%%

int yywrap() {
    return 1;
}
