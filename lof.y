%{

#include <stdio.h>
#include <string.h>
#include "expr.h"
extern int yyparse(void);
extern void yyset_in(FILE*);

static Expr* expr_returned;

Expr* expr_from_string(char const* str) {
    FILE* str_fd = fmemopen((char*)str, strlen(str), "r");
    if (str_fd == NULL) return NULL;
    yyset_in(str_fd);
    expr_returned = NULL;
    if (yyparse()) expr_returned = NULL;
    fclose(str_fd);
    return expr_returned;
}

%}

%start expr_top

%union {
    char* string_val;
    struct Expr* expr_val;
};

%token LBRAC RBRAC 
%token <string_val> VAR

%nterm <expr_val> expr
%nterm <expr_val> term

%%

expr_top :
      expr
        {
            expr_returned = $1;
        }
;

expr :
      /*empty*/
        {
            $$ = expr_list_new(); 
        }
    | expr term
        {
            expr_add_child($1, $2);
            $$ = $1;
        }
    ;

term :
      LBRAC expr RBRAC
        {
            $$ = expr_neg_new($2);
        }
    | VAR
        {
            $$ = expr_var_new($1);
        }
    ;

%%
