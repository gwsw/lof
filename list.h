
typedef struct ListNode {
    struct ListNode* ln_next;
    struct ListNode* ln_prev;
} ListNode;

typedef struct List {
    ListNode n;
    #define ls_head n.ln_next
    #define ls_tail n.ln_prev
} List;

List* list_new(void);
void list_insert(ListNode* ln, ListNode* after);
void list_add_tail(List* ls, ListNode* ln);
void list_unlink(ListNode* ln);
void* list_next(List* ls, void* ln);
int list_count(List* ls);
