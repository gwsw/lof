
typedef enum { ET_NULL, ET_LIST, ET_NEG, ET_VAR } ExprType;

typedef struct ExprNode {
    struct ExprNode* en_next;
    struct ExprNode* en_prev;
} ExprNode;

typedef struct ExprList {
    struct ExprNode* el_head;
    struct ExprNode* el_tail;
} ExprList;

typedef struct Expr {
    ExprNode e;
    ExprType expr_type;
    union {
        ExprList* et_list_children;
        char* et_var_name;
        struct Expr* et_neg_expr;
    } et;
    #define expr_children et.et_list_children
    #define expr_var_name et.et_var_name
    #define expr_neg_expr et.et_neg_expr
} Expr;


Expr* expr_norm_new(void);
Expr* expr_var_new(char const* var_name);
Expr* expr_neg_new(Expr* expr);
void expr_free(Expr* expr);
void expr_set_neg(Expr* expr, int neg);
void expr_add_child(Expr* expr, Expr* child);
void expr_set_var(Expr* expr, char const* var_name);
int expr_eq(Expr* expr1, Expr* expr2);
Expr* expr_from_string(char const* str);
void expr_to_string(Expr* expr, char* buf, size_t buf_len);
void expr_print(Expr* expr);
int expr_eval(Expr* expr);
