%{
#include <string.h>
#include "lof.yacc.h"
extern void yyerror(char const*);
int line_num = 0;
%}
%%

"["                     { return LBRAC; }
"]"                     { return RBRAC; }
[A-Za-z_][A-Za-z0-9_]*  { yylval.string_val = strdup(yytext); return VAR; }

[ \t\r]+                { }
[\n]                    { ++line_num; }
.                       { char msg[32];
                          snprintf(msg, sizeof(msg), "[%d] illegal token: '%s'", line_num, yytext);
                          yyerror(msg); }
%%
