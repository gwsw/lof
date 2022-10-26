%{
#include <string.h>
#include "lof.yacc.h"
extern void yyerror(char const*);
int line_num = 0;
%}
%%

"["                  { return LBRAC; }
"]"                  { return RBRAC; }

[ \t\r]+             { }
[\n]                 { ++line_num; }
.                    { char msg[32];
                       snprintf(msg, sizeof(msg), "[%d] illegal token: '%s'", line_num, yytext);
                       yyerror(msg); }
%%
