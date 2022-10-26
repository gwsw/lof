%{
#include <stdio.h>
#include "expr.h"
%}
%start expr

%union {
    int bool_val;
	struct Expr* expr_val;
	struct ExprList* expr_list_val;
};

%token LBRAC
%token RBRAC

%nterm <expr_list_val> expr_list
%nterm <expr_val> expr

%%

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
expr_print($$,NULL); printf(" = %d\n", expr_eval($$));
		}
    ;

%%
