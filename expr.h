
typedef enum { ET_NULL, ET_NORM, ET_VAR } ExprType;

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
        ExprList* et_children;
        char* et_var_name;
    } et;
    #define expr_children et.et_children
    #define expr_var_name et.et_var_name
} Expr;


Expr* expr_new(void);
void expr_free(Expr* expr);
void expr_set_children(Expr* expr, ExprList* el);
void expr_set_var(Expr* expr, char const* var_name);
void expr_to_string(Expr* expr, char* buf, size_t buf_len);
void expr_print(Expr* expr);
int expr_eval(Expr* expr);

ExprList* expr_list_new(void);
void expr_list_free(ExprList* el);
void expr_list_add(ExprList* el, Expr* expr);

ExprList* expr_list_from_string(char const* str);
void expr_list_to_string(ExprList* el, char* buf, size_t buf_len);
