
#include "util.h"

// convert item index to a byte index.
#define BIDX(p, v) ((p)->size*(v))
// convert byte index to item index.
#define IIDX(p, v) ((v)/(p)->size)

#define _ALLOC_BUFFER(ptr)  _ALLOC(ptr->size*ptr->cap)
#define _REALLOC_BUFFER(ptr) _REALLOC(ptr->buffer, ptr->size*ptr->cap)

static inline void expand_buffer(BaseList* lst) {

    if(lst->len+lst->size > lst->cap) {
        while(lst->len+lst->size > lst->cap)
            lst->cap <<= 1;
        lst->buffer = _REALLOC_BUFFER(lst);
    }
}


BaseList* create_base_list(int size) {

    BaseList* ptr = _ALLOC_T(BaseList);

    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->size = size;
    ptr->buffer = _ALLOC_BUFFER(ptr);
    ptr->changed = false;

    return ptr;
}

void destroy_base_list(BaseList* lst) {

    if(lst != NULL) {
        _FREE(lst->buffer);
        _FREE(lst);
    }
}

// append a datum to the list
BaseListResult add_base_list(BaseList* lst, void* data) {

    expand_buffer(lst);

    memcpy(&lst->buffer[lst->len], data, lst->size);
    lst->len += lst->size;
    lst->changed = true;

    return BASE_LST_OK;
}

BaseListResult get_base_list(BaseList* lst, int index, void* data) {

    if(BIDX(lst, index) < lst->len) {
        memcpy(data, &lst->buffer[lst->size*index], lst->size);
        return BASE_LST_OK;
    }
    else
        return BASE_LST_ERROR;
}

BaseListResult ins_base_list(BaseList* lst, int index, void* data) {

    // aid in debugging
    int start, end, size;
    start = lst->size*index;
    end = start+lst->size;
    size = lst->len-start;

    if(BIDX(lst, index) < lst->len) {
        expand_buffer(lst);

        // make room
        memmove(&lst->buffer[end], &lst->buffer[start], size);

        // copy data to the location in the buffer
        memcpy(&lst->buffer[start], data, lst->size);
        lst->len += lst->size;
        lst->changed = true;

        return BASE_LST_OK;
    }
    else
        return BASE_LST_ERROR;
}

BaseListResult del_base_list(BaseList* lst, int index) {

    // aid in debugging
    int start, end, size;
    start = lst->size*index;
    end = start+lst->size;
    size = lst->len-end;

    if(index >= 0 && BIDX(lst, index) < lst->len) {
        memmove(&lst->buffer[start], &lst->buffer[end], size);
        lst->len -= lst->size;
        lst->changed = true;
        return BASE_LST_OK;
    }
    else {
        return BASE_LST_ERROR;
    }
}

BaseListResult push_base_list(BaseList* lst, void* data) {

    return add_base_list(lst, data);
}

BaseListResult peek_base_list(BaseList* lst, void* data) {

    if(lst->len >= lst->size) {
        memcpy(data, &lst->buffer[lst->len-lst->size], lst->size);
        return BASE_LST_OK;
    }
    else
        return BASE_LST_ERROR;
}

// places the NEW top of stack into the var.
BaseListResult pop_base_list(BaseList* lst, void* data) {

    if(lst->len > 0)
        lst->len -= lst->size;

    return peek_base_list(lst, data);
}

int len_base_list(BaseList* lst) {

    return IIDX(lst, lst->len);
}

void *raw_base_list(BaseList* lst) {

    return (void*)lst->buffer;
}

BaseListIter* init_base_list_iter(BaseList* lst) {

    BaseListIter* iter = _ALLOC_T(BaseListIter);
    iter->index = 0;
    iter->list = lst;
    lst->changed = false;

    return iter;
}

// TODO! check for changes during iterations
BaseListResult iter_base_list(BaseListIter* iter, void* data) {

    if(!iter->list->changed) {
        if(BIDX(iter->list, iter->index) < iter->list->len)
            return get_base_list(iter->list, iter->index++, data);
        else
            return BASE_LST_END;
    }
    else
        return BASE_LST_ERROR;
}

