#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "expr.h"

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
        expr->expr_children = list_new();
    list_add_tail(expr->expr_children, &child->expr_node);
}

int expr_eval(Expr* expr) {
    switch (expr->expr_type) {
    case ET_LIST: {
        int value = 0; // empty expr is false
        Expr* child = NULL;
        while ((child = list_next(expr->expr_children, child)) != NULL)
            value |= expr_eval(child);
        return value; }
    case ET_NEG: {
        int value = expr_eval(expr->expr_neg_expr);
        if (value >= 0)
            value = !value;
        return value; }
    case ET_VAR:
        return -1;
    default:
        assert(0);
    }
}

int expr_eq(Expr* expr1, Expr* expr2) {
    return expr_eval(expr1) == expr_eval(expr2);
}

typedef struct MemBuf { char* buf; size_t len; } MemBuf;

static void membuf_addchar(MemBuf* mbuf, char ch) {
    *mbuf->buf++ = ch;
    mbuf->len--;
}

static void expr_to_string2(Expr* expr, MemBuf* mbuf) {
    if (mbuf->len <= 0) return;
    switch (expr->expr_type) {
    case ET_LIST: {
        Expr* child = NULL;
        while ((child = list_next(expr->expr_children, child)) != NULL)
            expr_to_string2(child, mbuf);
        break; }
    case ET_NEG:
        membuf_addchar(mbuf, '[');
        expr_to_string2(expr->expr_neg_expr, mbuf);
        membuf_addchar(mbuf, ']');
        break;
    case ET_VAR: {
        int plen = snprintf(mbuf->buf, mbuf->len, "%s", expr->expr_var_name);
        mbuf->buf += plen;
        mbuf->len -= plen;
        break; }
    default:
        abort();
    }
}

void expr_to_string(Expr* expr, char* buf, size_t len) {
    MemBuf mbuf = { buf, len };
    expr_to_string2(expr, &mbuf);
}

void expr_print(Expr* expr) {
    switch (expr->expr_type) {
    case ET_LIST: {
        Expr* child = NULL;
        while ((child = list_next(expr->expr_children, child)) != NULL)
            expr_print(child);
        break; }
    case ET_NEG:
        printf("[");
        expr_print(expr->expr_neg_expr);
        printf("]");
        break;
    case ET_VAR:
        printf("%s", expr->expr_var_name);
        break;
    default:
        abort();
    }
}
