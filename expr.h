
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
	ExprList* expr_children;
} Expr;


Expr* expr_new(void);
void expr_free(Expr* expr);
void expr_set_children(Expr* expr, ExprList* el);
void expr_print(Expr* expr, void* v);
int expr_eval(Expr* expr);

ExprList* expr_list_new(void);
void expr_list_add(ExprList* el, Expr* expr);
