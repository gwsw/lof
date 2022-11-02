#include <stdlib.h>
#include "list.h"

List* list_new(void) {
    List* ls = (List*) malloc(sizeof(List));
    ls->ls_head = ls->ls_tail = &ls->n;
    return ls;
}

void list_insert(ListNode* ln, ListNode* after) {
    ln->ln_next = after->ln_next;
    ln->ln_prev = after;
    after->ln_next->ln_prev = ln;
    after->ln_next = ln;
}

void list_add_tail(List* ls, ListNode* ln) {
    list_insert(ln, ls->ls_tail);
}

void list_unlink(ListNode* ln) {
    ln->ln_next->ln_prev = ln->ln_prev;
    ln->ln_prev->ln_next = ln->ln_next;
}

int list_empty(List* ls) {
    return ls == NULL || ls->ls_head == &ls->n;
}

void* list_next(List* ls, void* vln) {
    if (ls == NULL) return NULL;
    ListNode* ln = vln;
    void* next = (ln == NULL) ? ls->ls_head : ln->ln_next;
    if (next == ls) next = NULL;
    return next;
}

int list_count(List* ls) {
    int count = 0;
    ListNode* ln = NULL;
    while ((ln = list_next(ls, ln)) != NULL)
        ++count;
    return count;
}
