%{
#include <stdio.h>
#include <string.h>
#include "expr.h"
extern int yyparse(void);
extern void yyset_in(FILE*);

static ExprList* expr_list_returned;

ExprList* expr_list_from_string(char const* str) {
    FILE* str_fd = fmemopen((char*)str, strlen(str), "r");
    if (str_fd == NULL) return NULL;
    yyset_in(str_fd);
    expr_list_returned = NULL;
    if (yyparse()) expr_list_returned = NULL;
    fclose(str_fd);
    return expr_list_returned;
}

%}

%start expr_list_top

%union {
    int bool_val;
    char* string_val;
    struct Expr* expr_val;
    struct ExprList* expr_list_val;
};

%token LBRAC
%token RBRAC
%token <string_val> VAR

%nterm <expr_list_val> expr_list
%nterm <expr_val> expr

%%

expr_list_top :
      expr_list
        {
            expr_list_returned = $1;
        }
;

expr_list :
      /*empty*/
        {
            $$ = expr_list_new(); 
        }
    | expr_list expr
        {
            expr_list_add($1, $2);
            $$ = $1;
        }
    ;

expr :
      LBRAC expr_list RBRAC
        {
            $$ = expr_new();
            expr_set_children($$, $2);
        }
    | VAR
        {
            $$ = expr_new();
            expr_set_var($$, $1);
        }
    ;

%%
