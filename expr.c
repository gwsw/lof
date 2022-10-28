#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "expr.h"

// ------------------------------------------------------------------
static Expr* expr_from_node(ExprNode* en) {
    return (Expr*) en;
}

static ExprList* expr_list_new(void) {
    ExprList* el = (ExprList*) malloc(sizeof(ExprList));
    el->el_head = el->el_tail = (ExprNode*) el;
    return el;
}

static void expr_list_free(ExprList* el) {
    ///@@@
}

static void expr_list_insert(Expr* expr, Expr* after) {
    expr->e.en_next = after->e.en_next;
    expr->e.en_prev = &after->e;
    after->e.en_next->en_prev = &expr->e;
    after->e.en_next = &expr->e;
}

static void expr_list_add(ExprList* el, Expr* expr) {
    expr_list_insert(expr, expr_from_node(el->el_tail));
}

#if 0
static void expr_list_to_string(ExprList* el, char* buf, size_t buf_len) {
    for (ExprNode* en = el->el_head; en != (ExprNode*) el && buf_len > 0; en = en->en_next) {
        expr_to_string(expr_from_node(en), buf, buf_len);
        size_t len = strlen(buf);
        buf += len;
        buf_len -= len;
    }
}
#endif

static void expr_iter_children(Expr* expr, void (*func)(Expr*,void*), void* func_data) {
    if (expr->expr_children == NULL)
        return;
    for (ExprNode* en = expr->expr_children->el_head; en != (ExprNode*) expr->expr_children; en = en->en_next) {
        (*func)(expr_from_node(en), func_data);
    }
}

// ------------------------------------------------------------------
Expr* expr_norm_new(void) {
    Expr* expr = (Expr*) malloc(sizeof(Expr));
    expr->expr_type = ET_LIST;
    expr->expr_children = NULL;
    return expr;
}

Expr* expr_var_new(char const* var_name) {
    Expr* expr = (Expr*) malloc(sizeof(Expr));
    expr->expr_type = ET_VAR;
    expr->expr_var_name = strdup(var_name);
    return expr;
}

Expr* expr_neg_new(Expr* neg_expr) {
    Expr* expr = (Expr*) malloc(sizeof(Expr));
    expr->expr_type = ET_NEG;
    expr->expr_neg_expr = neg_expr;
    return expr;
}

void expr_free(Expr* expr) {
}

void expr_add_child(Expr* expr, Expr* child) {
    assert(expr->expr_type == ET_LIST);
    if (expr->expr_children == NULL)
        expr->expr_children = expr_list_new();
    expr_list_add(expr->expr_children, child);
}

// ------------------------------------------------------------------
typedef struct ExprValue { int ev_Value; } ExprValue;

static void expr_eval_iter(Expr* expr, void* v) {
    ExprValue* ev = v;
    ev->ev_Value |= expr_eval(expr);
}

int expr_eval(Expr* expr) {
    switch (expr->expr_type) {
    case ET_LIST: {
        ExprValue ev;
        ev.ev_Value = 0; // empty expr is false
        expr_iter_children(expr, expr_eval_iter, &ev);
        return ev.ev_Value; }
    case ET_NEG:
        return !expr_eval(expr->expr_neg_expr);
    case ET_VAR:
        return -1;
    default:
        assert(0);
    }
}

int expr_eq(Expr* expr1, Expr* expr2) {
    return expr_eval(expr1) == expr_eval(expr2);
}

// ------------------------------------------------------------------
typedef struct ExprString { char* buf; size_t len; } ExprString;

static void expr_to_string2(Expr* expr, void* ves) {
    ExprString* es = ves;
    if (es->len <= 0) return;
    switch (expr->expr_type) {
    case ET_LIST:
        expr_iter_children(expr, &expr_to_string2, ves);
        break;
    case ET_NEG:
        if (es->len > 1) {
            *es->buf++ = '[';
            es->len--;
        }
        expr_to_string2(expr->expr_neg_expr, ves);
        if (es->len > 1) {
            *es->buf++ = ']';
            *es->buf = '\0';
        }
    case ET_VAR: {
        int len = snprintf(es->buf, es->len, "%s", expr->expr_var_name);
        es->buf += len;
        es->len -= len;
        break; }
    default:
        abort();
    }
}

void expr_to_string(Expr* expr, char* buf, size_t buf_len) {
    ExprString es = { buf, buf_len };
    expr_to_string2(expr, &es);
}

static void expr_print2(Expr* expr, void* v) {
    switch (expr->expr_type) {
    case ET_LIST:
        expr_iter_children(expr, &expr_print2, v);
        break;
    case ET_NEG:
        printf("[");
        expr_print2(expr->expr_neg_expr, v);
        printf("]");
        break;
    case ET_VAR:
        printf("(%s)", expr->expr_var_name);
        break;
    default:
        abort();
    }
}

void expr_print(Expr* expr) {
    expr_print2(expr, NULL);
}

