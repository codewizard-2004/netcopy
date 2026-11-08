
%{
#include "program.tab.h"
#include <stdio.h>
#include <string.h>
%}

%%
"<"[a-zA-Z][a-zA-Z0-9_]*">" {
    yylval.str = strdup(yytext + 1);
    yylval.str[strlen(yylval.str) - 1] = '\0';
    return NONTERMINAL;
}

\"[^\"]*\" {
    yylval.str = strdup(yytext);
    return TERMINAL;
}

"::="       { return DEFINE; }
"|"         { return OR; }
[ \t]+      { /* Ignore whitespace */ }
\n          { return '\n'; }
.           { return INVALID; }
%%

int yywrap() {
    return 1;
}
