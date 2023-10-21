
#include "util.h"

// The index is the item index, not the byte index. This function converts
// it to the byte index. If the idx is negative, then convert it to the
// correct byte index counting from the end.
static inline int normalize_index(List* lst, int idx) {

    int val;

    if(0 > idx)
        // if idx == -1, then result should be len.
        val = lst->len + (idx + 1);
    else
        val = idx;

    val *= lst->size;

    // TODO: raise exception instead of fatal error
    if(val < 0 || val > lst->len) {
        fprintf(stderr, "Fatal Error: index out of range: %d\n", val);
        exit(1);
    }

    return val;
}

static inline void expand_buffer(List* lst) {

    if(lst->len + lst->size > lst->cap) {
        while(lst->len + lst->size > lst->cap)
            lst->cap <<= 1;
        lst->buffer = _REALLOC(lst->buffer, lst->size * lst->cap);
    }
}


List* create_list(int size) {

    List* ptr = _ALLOC_T(List);

    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->size = size;
    ptr->buffer = _ALLOC(ptr->size * ptr->cap);
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
ListResult append_list(List* lst, void* data) {

    expand_buffer(lst);

    memcpy(&lst->buffer[lst->len], data, lst->size);
    lst->len += lst->size;
    lst->changed = true;

    return LIST_OK;
}

ListResult read_list(List* lst, int index, void* data) {

    int idx = normalize_index(lst, index);
    memcpy(data, &lst->buffer[idx], lst->size);
    return LIST_OK;

    // if((lst->size * index) < lst->len) {
    //     memcpy(data, &lst->buffer[lst->size * index], lst->size);
    //     return LIST_OK;
    // }
    // else
    //     return LIST_ERROR;
}

ListResult write_list(List* lst, int index, void* data) {

    int idx = normalize_index(lst, index);
    memcpy(&lst->buffer[idx], data, lst->size);
    return LIST_OK;
}

ListResult insert_list(List* lst, int index, void* data) {

    // aid in debugging
    int start, end, size;
    //start = lst->size * index;
    start = lst->size * normalize_index(lst, index);
    end = start + lst->size;
    size = lst->len - start;

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

ListResult delete_list(List* lst, int index) {

    // aid in debugging
    int start, end, size;
    //start = lst->size * index;
    start = lst->size * normalize_index(lst, index);
    end = start + lst->size;
    size = lst->len - end;

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

    return append_list(lst, data);
}

ListResult peek_list(List* lst, void* data) {

    if(lst->len >= lst->size) {
        memcpy(data, &lst->buffer[lst->len - lst->size], lst->size);
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
void* raw_list(List* lst) {

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
            return read_list(iter->list, iter->index++, data);
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
            return read_list(iter->list, iter->index--, data);
        else
            return LIST_END;
    }
    else
        return LIST_CHANGED;
}
