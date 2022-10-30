%option nounput
%option noinput

%{
#include <string.h>
#include "lof.yacc.h"
extern void yyerror(char const*);
%}
%%

"["                     { return LBRAC; }
"]"                     { return RBRAC; }
[A-Za-z_][A-Za-z0-9_]*  { yylval.string_val = strdup(yytext); return VAR; }

[ \t\r\n]+              { }
.                       { char msg[32];
                          snprintf(msg, sizeof(msg), "illegal token: '%s'", yytext);
                          yyerror(msg); }
%%
