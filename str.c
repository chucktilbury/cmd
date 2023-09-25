
#include <stdarg.h>
#include <string.h>

#include "util.h"

StrLst* create_str_lst() {
    return (StrLst*)create_ptr_lst();
}

void destroy_str_lst(StrLst* lst) {
    for(int x = 0; x < lst->len; x++)
        _FREE(lst->list[x]);
    destroy_ptr_lst(lst);
}

void add_str_lst(StrLst* lst, const char* str) {
    add_ptr_lst(lst, (void*)_DUP_STR(str));
}

void reset_str_lst(StrLst* lst) {
    reset_ptr_lst(lst);
}

const char* iterate_str_lst(StrLst* lst) {
    return (const char*)iterate_ptr_lst(lst);
}

Str* create_string(const char* str) {

    Str* ptr = _ALLOC_T(Str);
    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->idx = 0;
    ptr->buf = _ALLOC_ARRAY(char, ptr->cap);

    if(str != NULL)
        add_string_str(ptr, str);

    return ptr;
}

Str* create_string_fmt(const char* str, ...) {

    assert(str != NULL);
    va_list args;

    va_start(args, str);
    size_t len = vsnprintf(NULL, 0, str, args);
    va_end(args);

    char* ptr = _ALLOC(len+1);

    va_start(args, str);
    vsnprintf(ptr, len+1, str, args);
    va_end(args);

    Str* retv = create_string(ptr);
    _FREE(ptr);

    return retv;
}

void destroy_string(Str* ptr) {

    assert(ptr != NULL);
    _FREE(ptr->buf);
    _FREE(ptr);
}

void add_string_char(Str* ptr, int ch) {

    assert(ptr != NULL);

    if(ptr->len+1 > ptr->cap) {
        ptr->cap <<= 1;
        ptr->buf = _REALLOC_ARRAY(ptr->buf, char, ptr->cap);
    }

    ptr->buf[ptr->len] = ch;
    ptr->len++;
    ptr->buf[ptr->len] = 0;
}

void add_string_str(Str* ptr, const char* str) {

    assert(ptr != NULL);
    assert(str != NULL);

    size_t len = strlen(str);
    if(ptr->len+len+1 > ptr->cap) {
        while(ptr->len+len+1 > ptr->cap)
            ptr->cap <<= 1;
        ptr->buf = _REALLOC_ARRAY(ptr->buf, char, ptr->cap);
    }

    strcat(ptr->buf, str);
    ptr->len += len;
}

void add_string_fmt(Str* ptr, const char* str, ...) {

    assert(ptr != NULL);
    assert(str != NULL);

    va_list args;

    va_start(args, str);
    size_t len = vsnprintf(NULL, 0, str, args);
    va_end(args);

    char* spt = _ALLOC(len+1);

    va_start(args, str);
    vsnprintf(spt, len+1, str, args);
    va_end(args);

    add_string_str(ptr, spt);
    _FREE(spt);
}

void reset_string(Str* ptr) {

    assert(ptr != NULL);
    ptr->idx = 0;
}

int iterate_string(Str* ptr) {

    assert(ptr != NULL);
    int ch = 0;

    if(ptr->idx < ptr->len) {
        ch = ptr->buf[ptr->idx];
        ptr->idx++;
    }

    return ch;
}

const char* raw_string(Str* ptr) {

    assert(ptr != NULL);
    return ptr->buf;
}

