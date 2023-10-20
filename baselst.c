
#include "util.h"

static inline void expand_buffer(List* lst) {

    if(lst->len+lst->size > lst->cap) {
        while(lst->len+lst->size > lst->cap)
            lst->cap <<= 1;
        lst->buffer = _REALLOC(lst->buffer, lst->size*lst->cap);
    }
}


List* create_list(int size) {

    List* ptr = _ALLOC_T(List);

    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->size = size;
    ptr->buffer = _ALLOC(ptr->size*ptr->cap);
    ptr->changed = false;

    return ptr;
}

void destroy_list(List* lst) {

    if(lst != NULL) {
        _FREE(lst->buffer);
        _FREE(lst);
    }
}

// append a datum to the list
ListResult add_list(List* lst, void* data) {

    expand_buffer(lst);

    memcpy(&lst->buffer[lst->len], data, lst->size);
    lst->len += lst->size;
    lst->changed = true;

    return LIST_OK;
}

ListResult get_list(List* lst, int index, void* data) {

    if((lst->size * index) < lst->len) {
        memcpy(data, &lst->buffer[lst->size*index], lst->size);
        return LIST_OK;
    }
    else
        return LIST_ERROR;
}

ListResult ins_list(List* lst, int index, void* data) {

    // aid in debugging
    int start, end, size;
    start = lst->size*index;
    end = start+lst->size;
    size = lst->len-start;

    if((lst->size * index) < lst->len) {
        expand_buffer(lst);

        // make room
        memmove(&lst->buffer[end], &lst->buffer[start], size);

        // copy data to the location in the buffer
        memcpy(&lst->buffer[start], data, lst->size);
        lst->len += lst->size;
        lst->changed = true;

        return LIST_OK;
    }
    else
        return LIST_ERROR;
}

ListResult del_list(List* lst, int index) {

    // aid in debugging
    int start, end, size;
    start = lst->size*index;
    end = start+lst->size;
    size = lst->len-end;

    if(index >= 0 && ((lst->size * index) < lst->len)) {
        memmove(&lst->buffer[start], &lst->buffer[end], size);
        lst->len -= lst->size;
        lst->changed = true;
        return LIST_OK;
    }
    else {
        return LIST_ERROR;
    }
}

ListResult push_list(List* lst, void* data) {

    return add_list(lst, data);
}

ListResult peek_list(List* lst, void* data) {

    if(lst->len >= lst->size) {
        memcpy(data, &lst->buffer[lst->len-lst->size], lst->size);
        return LIST_OK;
    }
    else
        return LIST_ERROR;
}

// places the NEW top of stack into the var.
ListResult pop_list(List* lst, void* data) {

    if(lst->len > 0)
        lst->len -= lst->size;

    if(data != NULL)
        return peek_list(lst, data);
    else
        return LIST_OK;
}

ListResult clr_list(List* lst) {

    lst->len = 0;
    lst->changed = true;

    return LIST_OK;
}

// Return the number of items in the list
int len_list(List* lst) {

    return lst->len / lst->size;
}

// Get the raw list
void *raw_list(List* lst) {

    return (void*)lst->buffer;
}

ListIter* init_list_iter(List* lst) {

    ListIter* iter = _ALLOC_T(ListIter);
    iter->index = 0;
    iter->list = lst;
    lst->changed = false;

    return iter;
}

// If the list changes during iterations, then return an error.
ListResult iter_list(ListIter* iter, void* data) {

    if(!iter->list->changed) {
        if((iter->list->size * iter->index) < iter->list->len)
            return get_list(iter->list, iter->index++, data);
        else
            return LIST_END;
    }
    else
        return LIST_CHANGED;
}

ListIter* init_list_riter(List* lst) {

    ListIter* iter = _ALLOC_T(ListIter);
    iter->index = (lst->len / lst->size) - 1;
    iter->list = lst;
    lst->changed = false;

    return iter;
}

// If the list changes during iterations, then return an error.
ListResult riter_list(ListIter* iter, void* data) {

    if(!iter->list->changed) {
        if(iter->index >= 0)
            return get_list(iter->list, iter->index--, data);
        else
            return LIST_END;
    }
    else
        return LIST_CHANGED;
}

