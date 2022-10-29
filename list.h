
typedef struct ListNode {
    struct ListNode* ln_next;
    struct ListNode* ln_prev;
} ListNode;

typedef struct List {
    struct ListNode* ls_head;
    struct ListNode* ls_tail;
} List;

List* list_new(void);
void list_insert(ListNode* ln, ListNode* after);
void list_add_tail(List* ls, ListNode* ln);
void* list_next(List* ls, void* ln);
