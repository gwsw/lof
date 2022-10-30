#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "expr.h"

static Expr* expr_new(ExprType type) {
    Expr* expr = (Expr*) malloc(sizeof(Expr));
    expr->expr_type = type;
    return expr;
}

Expr* expr_list_new(void) {
    Expr* expr = expr_new(ET_LIST);
    expr->expr_children = NULL;
    return expr;
}

Expr* expr_var_new(char const* var_name) {
    Expr* expr = expr_new(ET_VAR);
    expr->expr_var_name = strdup(var_name);
    return expr;
}

Expr* expr_neg_new(Expr* neg_expr) {
    Expr* expr = expr_new(ET_NEG);
    expr->expr_neg_expr = neg_expr;
    return expr;
}

void expr_free(Expr* expr) {
    switch (expr->expr_type) {
    case ET_LIST: {
        if (expr->expr_children == NULL)
            break;
        Expr* child;
        while ((child = list_next(expr->expr_children, NULL)) != NULL) {
            list_unlink(&child->expr_node);
            expr_free(child);
        }
        free(expr->expr_children);
        break; }
    case ET_NEG:
        expr_free(expr->expr_neg_expr);
        break;
    case ET_VAR:
        free(expr->expr_var_name);
        break;
    default:
        assert(0);
    }
    free(expr);
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
        while ((child = list_next(expr->expr_children, child)) != NULL) {
            int cvalue = expr_eval(child);
            value = (cvalue < 0) ? -1 : (value | cvalue);
        }
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

static void expr_vars(Expr* expr, List* vars) {
    switch (expr->expr_type) {
    case ET_LIST: {
        Expr* child = NULL;
        while ((child = list_next(expr->expr_children, child)) != NULL)
            expr_vars(child, vars);
        break; }
    case ET_NEG: 
        expr_vars(expr->expr_neg_expr, vars);
        break;
    case ET_VAR: {
        Var* var = NULL;
        while ((var = list_next(vars, var)) != NULL) {
            int cmp = strcmp(expr->expr_var_name, var->v_name);
            if (cmp == 0) return; // already in list
            if (cmp < 0) break;
        }
        Var* nvar = malloc(sizeof(Var));
        nvar->v_name = strdup(expr->expr_var_name);
        list_insert(&nvar->v_node, var == NULL ? vars->ls_tail : var->v_node.ln_prev);
        break; }
    default:
        assert(0);
    }
}

void vars_free(List* vars) {
    Var* var;
    while ((var = list_next(vars, NULL)) != NULL) {
        list_unlink(&var->v_node);
        free(var->v_name);
        free(var);
    }
}

static Expr* expr_subst_vars2(Expr* expr, List* vars) {
    switch (expr->expr_type) {
    case ET_LIST: {
        Expr* nexpr = expr_list_new();
        Expr* child = NULL;
        while ((child = list_next(expr->expr_children, child)) != NULL)
            expr_add_child(nexpr, expr_subst_vars2(child, vars));
        return nexpr; }
    case ET_NEG:
        return expr_neg_new(expr_subst_vars2(expr->expr_neg_expr, vars));
    case ET_VAR: {
        int value = -1;
        Var* var = NULL;
        while ((var = list_next(vars, var)) != NULL) {
            if (strcmp(expr->expr_var_name, var->v_name) == 0) {
                value = var->v_value;
                break;
            }
        }
        assert(value >= 0);
        Expr* nexpr = expr_list_new();
        if (value)
            nexpr = expr_neg_new(nexpr);
        return nexpr; }
    default:
        abort();
    }
}

static Expr* expr_subst_vars(Expr* expr, List* vars, unsigned long val_mask) {
    Var* var = NULL;
    while ((var = list_next(vars, var)) != NULL) {
        var->v_value = val_mask & 1;
        val_mask >>= 1;
    }
    return expr_subst_vars2(expr, vars);
}

int expr_eq(Expr* expr1, Expr* expr2) {
    List* vars = list_new();
    expr_vars(expr1, vars);
    expr_vars(expr2, vars);
    int num_vars = list_count(vars);
    int eq = 1;
    for (unsigned long val_mask = 0; val_mask != (1<<num_vars); ++val_mask) {
        Expr* e1 = expr_subst_vars(expr1, vars, val_mask);
        Expr* e2 = expr_subst_vars(expr2, vars, val_mask);
        eq = (expr_eval(e1) == expr_eval(e2));
        expr_free(e1);
        expr_free(e2);
        if (!eq) break;
    }
    vars_free(vars);
    return eq;
}

typedef struct MemBuf { char* buf; size_t len; } MemBuf;

static void membuf_addchar(MemBuf* mbuf, char ch) {
    if (mbuf->len == 0) return;
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
    *mbuf.buf = '\0';
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
