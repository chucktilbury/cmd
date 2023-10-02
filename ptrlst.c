
#include <assert.h>
#include <stddef.h>

#include "util.h"

PtrList* create_ptr_list() {

    PtrList* ptr = _ALLOC_T(PtrList);
    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->idx = 0;
    ptr->list = _ALLOC_ARRAY(void*, ptr->cap);

    return ptr;
}

void destroy_ptr_list(PtrList* lst) {

    if(lst != NULL) {
        _FREE(lst->list);
        _FREE(lst);
    }
}

// Add a pointer to the pointer list. There is no sense of allocating memory
// for the added pointer. The caller is responsible for managing the memory.
void add_ptr_list(PtrList* lst, void* ptr) {

    assert(lst != NULL);

    if(lst->len+1 > lst->cap) {
        lst->cap <<= 1;
        lst->list = _REALLOC_ARRAY(lst->list, void*, lst->cap);
    }

    lst->list[lst->len] = ptr;
    lst->len++;
}

// Reset the iterator to the beginning of the list. There is no return value.
void reset_ptr_list(PtrList* lst) {

    assert(lst != NULL);
    lst->idx = 0;
}

// Iterate pointer list. When there are no more items to iterate, then return
// a NULL pointer.
void* iterate_ptr_list(PtrList* lst) {

    assert(lst != NULL);
    void* ptr = NULL;

    if(lst->idx < lst->len) {
        ptr = lst->list[lst->idx];
        lst->idx++;
    }

    return ptr;
}

// Add a pointer to the end of the list, but call it a "push" operation.
void push_ptr_list(PtrList* lst, void* ptr) {

    assert(lst != NULL);
    assert(ptr != NULL);

    add_ptr_list(lst, ptr);
}

// Non-destructively return to value on the top of the stack.
void* peek_ptr_list(PtrList* lst) {

    assert(lst != NULL);
    if(lst->len > 0)
        return lst->list[lst->len-1];
    else
        return NULL;
}

// remove the item on the top of the stack and then return the item on the
// new top of the stack.
void* pop_ptr_list(PtrList* lst) {

    assert(lst != NULL);
    if(lst->len > 0)
        lst->len--;

    return peek_ptr_list(lst);
}

