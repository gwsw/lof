#include <stdio.h>
#include <stdlib.h>
#include "expr.h"

static Expr* expr_from_node(ExprNode* en) {
	return (Expr*) en;
}

static void expr_iter_children(Expr* expr, void (*func)(Expr*,void*), void* func_data) {
	if (expr->expr_children == NULL)
		return;
	for (ExprNode* en = expr->expr_children->el_head; en != (ExprNode*) expr->expr_children; en = en->en_next) {
		(*func)(expr_from_node(en), func_data);
	}
}

Expr* expr_new(void) {
	Expr* expr = (Expr*) malloc(sizeof(Expr));
	expr->expr_children = NULL;
	return expr;
}

ExprList* expr_list_new(void) {
	ExprList* el = (ExprList*) malloc(sizeof(ExprList));
	el->el_head = el->el_tail = (ExprNode*) el;
	return el;
}

void expr_set_children(Expr* expr, ExprList* el) {
	expr->expr_children = el;
}

static void expr_insert(Expr* expr, Expr* after) {
	expr->e.en_next = after->e.en_next;
	expr->e.en_prev = &after->e;
	after->e.en_next->en_prev = &expr->e;
	after->e.en_next = &expr->e;
}

void expr_list_add(ExprList* el, Expr* expr) {
	expr_insert(expr, expr_from_node(el->el_tail));
}

void expr_free(Expr* expr) {
}

typedef struct ExprValue { int ev_Value; } ExprValue;

static void expr_eval_iter(Expr* expr, void* v) {
	ExprValue* ev = v;
	ev->ev_Value |= expr_eval(expr);
}

int expr_eval(Expr* expr) {
	ExprValue ev;
	ev.ev_Value = 0; // empty expr is false;
	expr_iter_children(expr, expr_eval_iter, &ev);
	return !ev.ev_Value;
}

void expr_print(Expr* expr, void* v) {
	printf("[");
	expr_iter_children(expr, &expr_print, v);
	printf("]");
}
