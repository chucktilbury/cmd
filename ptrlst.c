
#include <stddef.h>

#include "util.h"

PtrLst* create_ptr_lst() {

    PtrLst* ptr = _ALLOC_T(PtrLst);
    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->idx = 0;
    ptr->list = _ALLOC_ARRAY(void*, ptr->cap);

    return PTOH(ptr);
}

void destroy_ptr_lst(PtrLst* lst) {

    assert(lst != NULL);

    if(lst != NULL) {
        _FREE(lst->list);
        _FREE(lst);
    }
}

// Add a pointer to the pointer list. There is no sense of allocating memory
// for the added pointer. The caller is responsible for managing the memory.
void add_ptr_lst(PtrLst* lst, void* ptr) {

    assert(lst != NULL);

    if(lst->len+1 > lst->cap) {
        lst->cap <<= 1;
        lst->list = _REALLOC_ARRAY(lst->list, void*, lst->cap);
    }

    lst->list[lst->len] = ptr;
    lst->len++;
}

// Reset the iterator to the beginning of the list. There is no return value.
void reset_ptr_lst(PtrLst* lst) {

    assert(lst != NULL);
    lst->idx = 0;
}

// Iterate pointer list. When there are no more items to iterate, then return
// a NULL pointer.
const char* iterate_ptr_lst(PtrLst* lst) {

    assert(lst != NULL);
    const char* ptr = NULL;

    if(lst->idx < lst->len) {
        ptr = lst->list[lst->idx];
        lst->idx++;
    }

    return ptr;
}

