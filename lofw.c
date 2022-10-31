#include <stdio.h>

extern void yyerror(const char* str);
extern int yylex(void);

void yyerror(const char* str) {
    fprintf(stderr, "ERROR: %s\n", str);
}

int yywrap() {
    return 1;
}

#include "lof.yacc.c"
