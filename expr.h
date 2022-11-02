#include "list.h"

typedef enum { ET_NULL, ET_LIST, ET_NEG, ET_VAR } ExprType;

typedef struct Expr {
    ListNode expr_node;
    ExprType expr_type;
    ///unsigned int expr_depth;
    unsigned int expr_pos;
    unsigned int expr_width;
    union {
        List* et_list_children;
        char* et_var_name;
        struct Expr* et_neg_expr;
    } et;
    #define expr_children et.et_list_children
    #define expr_var_name et.et_var_name
    #define expr_neg_expr et.et_neg_expr
} Expr;

typedef struct Var {
    ListNode v_node;
    char* v_name;
    int v_value;
} Var;


Expr* expr_list_new(void);
Expr* expr_var_new(char const* var_name);
Expr* expr_neg_new(Expr* expr);
void expr_free(Expr* expr);
void expr_add_child(Expr* expr, Expr* child);
void expr_set_depth(Expr* expr, unsigned int depth);
void vars_free(List* list);
int expr_eq(Expr* expr1, Expr* expr2);
int expr_eval(Expr* expr);
Expr* expr_from_string(char const* str);
void expr_to_string(Expr* expr, char* buf, size_t buf_len);
void expr_negdump(Expr* expr);
void expr_print(Expr* expr);
