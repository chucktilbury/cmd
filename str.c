#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

StrList* create_str_list() {
    return (StrList*)create_ptr_list();
}

void destroy_str_list(StrList* lst) {
    for(int x = 0; x < lst->len; x++)
        destroy_string(lst->list[x]);
    destroy_ptr_list(lst);
}

void add_str_list(StrList* lst, Str* str) {
    add_ptr_list(lst, str);
}

void reset_str_list(StrList* lst) {
    reset_ptr_list(lst);
}

Str* iterate_str_list(StrList* lst) {
    return (Str*)iterate_ptr_list(lst);
}

void push_str_list(StrList* lst, Str* str) {
    push_ptr_list(lst, str);
}

Str* peek_str_list(StrList* lst) {
    return (Str*)peek_ptr_list(lst);
}

Str* pop_str_list(StrList* lst) {
    return (Str*)pop_ptr_list(lst);
}

// Join a list where the str is between the elements of the list.
Str* join_str_list(StrList* lst, const char* str) {

    Str* s = create_string(NULL);
    Str* tmp;

    reset_str_list(lst);
    add_string_Str(s, iterate_str_list(lst));
    while(NULL != (tmp = iterate_str_list(lst))) {
        add_string_str(s, str);
        add_string_Str(s, tmp);
    }

    return s;
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

    char* ptr = _ALLOC(len + 1);

    va_start(args, str);
    vsnprintf(ptr, len + 1, str, args);
    va_end(args);

    Str* retv = create_string(ptr);
    _FREE(ptr);

    return retv;
}

void destroy_string(Str* ptr) {

    if(ptr != NULL) {
        _FREE(ptr->buf);
        _FREE(ptr);
    }
}

void add_string_char(Str* ptr, int ch) {

    assert(ptr != NULL);

    if(ptr->len + 1 > ptr->cap) {
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

    int len = strlen(str);
    if(ptr->len + len + 1 > ptr->cap) {
        while(ptr->len + len + 1 > ptr->cap)
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

    char* spt = _ALLOC(len + 1);

    va_start(args, str);
    vsnprintf(spt, len + 1, str, args);
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

int comp_str(Str* s1, Str* s2) {
    return strcmp(s1->buf, s2->buf);
}

int comp_str_const(Str* s1, const char* s2) {
    return strcmp(s1->buf, s2);
}

Str* copy_string(Str* str) {

    assert(str != NULL);
    return create_string(str->buf);
}

// if the index is <0 then count from the end.
void truncate_string(Str* str, int index) {

    assert(str != NULL);
    if(index > 0 && index < str->len)
        str->buf[index] = 0;
    else if(index < 0 && ((str->len + index) > 0))
        str->buf[str->len + index] = 0;
    else
        str->buf[0] = 0;

    str->len = strlen(str->buf);
    str->idx = 0; // reset the index
}

int len_string(Str* str) {

    assert(str != NULL);
    return str->len;
}

void add_string_Str(Str* ptr, Str* str) {

    assert(ptr != NULL);
    assert(str != NULL);

    add_string_str(ptr, str->buf);
}

void print_string(FILE* fp, Str* str) {

    fprintf(fp, "%s", raw_string(str));
}

void printf_string(FILE* fp, Str* str, ...) {

    va_list args;

    va_start(args, str);
    vfprintf(fp, raw_string(str), args);
    va_end(args);
}
