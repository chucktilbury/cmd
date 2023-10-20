#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

// Join a list where the str is between the elements of the list.
Str* join_str_list(StrList* lst, const char* str) {

    Str* s = create_string(NULL);
    Str* tmp;

    StrListIter* sli = init_str_list_iter(lst);
    add_string_Str(s, iterate_str_list(sli));
    while(NULL != (tmp = iterate_str_list(sli))) {
        add_string_str(s, str);
        add_string_Str(s, tmp);
    }

    return s;
}

Str* create_string(const char* str) {

    // Str* ptr = _ALLOC_T(Str);
    // ptr->cap = 1 << 3;
    // ptr->len = 0;
    // ptr->idx = 0;
    // ptr->buf = _ALLOC_ARRAY(char, ptr->cap);
    Str* ptr = create_base_list(sizeof(char));

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
        //_FREE(ptr->buf);
        destroy_base_list(ptr);
        _FREE(ptr);
    }
}

void add_string_char(Str* ptr, int ch) {

    // assert(ptr != NULL);

    // if(ptr->len + 1 > ptr->cap) {
    //     ptr->cap <<= 1;
    //     ptr->buf = _REALLOC_ARRAY(ptr->buf, char, ptr->cap);
    // }

    // ptr->buf[ptr->len] = ch;
    // ptr->len++;
    // ptr->buf[ptr->len] = 0;
    add_base_list(ptr, &ch);
}

void add_string_str(Str* ptr, const char* str) {

    // assert(ptr != NULL);
    // assert(str != NULL);

    // int len = strlen(str);
    // if(ptr->len + len + 1 > ptr->cap) {
    //     while(ptr->len + len + 1 > ptr->cap)
    //         ptr->cap <<= 1;
    //     ptr->buf = _REALLOC_ARRAY(ptr->buf, char, ptr->cap);
    // }

    // strcat(ptr->buf, str);
    // ptr->len += len;
    for(const char* ch = str; *ch != '\0'; ch++)
        add_string_char(ptr, *ch);
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

// void reset_string(Str* ptr) {

//     assert(ptr != NULL);
//     ptr->idx = 0;
// }

// int iterate_string(Str* ptr) {

//     assert(ptr != NULL);
//     int ch = 0;

//     if(ptr->idx < ptr->len) {
//         ch = ptr->buf[ptr->idx];
//         ptr->idx++;
//     }

//     return ch;
// }

const char* raw_string(Str* ptr) {

    // assert(ptr != NULL);
    // return ptr->buf;

    // terminate the string
    int ch = 0;
    push_base_list(ptr, &ch);
    pop_base_list(ptr, NULL);

    return (const char*)raw_base_list(ptr);
}

int comp_string(Str* s1, Str* s2) {
    return strcmp(raw_string(s1), raw_string(s2));
}

int comp_string_const(Str* s1, const char* s2) {
    return strcmp(raw_string(s1), s2);
}

Str* copy_string(Str* str) {
    return create_string(raw_string(str));
}

// if the index is <0 then count from the end.
void truncate_string(Str* str, int index) {

    // assert(str != NULL);
    // if(index > 0 && index <= str->len)
    //     str->buf[index] = 0;
    // else if(index < 0 && ((str->len + index) > 0))
    //     str->buf[str->len + index] = 0;
    // else
    //     str->buf[0] = 0;

    // str->len = strlen(str->buf);
    // str->idx = 0; // reset the index

    int val = 0;
    ins_base_list(str, index, &val);
}

void clear_string(Str* str) {

    truncate_string(str, 0);
    clr_base_list(str);
}

int len_string(Str* str) {

    //assert(str != NULL);
    return len_base_list(str);
}

void add_string_Str(Str* ptr, Str* str) {

    // assert(ptr != NULL);
    // assert(str != NULL);

    add_string_str(ptr, raw_string(str));
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
