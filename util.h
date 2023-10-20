#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <setjmp.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>

//----------------------------------------------
// mem.c
//----------------------------------------------
#define _ALLOC(s) mem_alloc(s)
#define _ALLOC_T(t) (t*)mem_alloc(sizeof(t))
#define _ALLOC_ARRAY(t, n) (t*)mem_alloc(sizeof(t) * (n))
#define _REALLOC(p, s) mem_realloc((p), (s))
#define _REALLOC_T(p, t) (t*)mem_realloc((p), sizeof(t))
#define _REALLOC_ARRAY(p, t, n) (t*)mem_realloc((p), sizeof(t) * (n))
#define _DUP_MEM(p, s) mem_dup((p), (s))
#define _DUP_MEM_T(p, t) (t*)mem_dup((p), sizeof(t))
#define _DUP_STR(p) mem_dup_str(p)
#define _FREE(p) mem_free(((void*)p))

void* mem_alloc(size_t size);
void* mem_realloc(void* ptr, size_t size);
void* mem_dup(void* ptr, size_t size);
char* mem_dup_str(const char* str);
void mem_free(void* ptr);

//------------------------------------------------------
// datalst.c
//------------------------------------------------------
// Arbitrary sized data buffer list. All of the elements
// must be the same size. Copies of the data is saved in
// the list.
typedef struct {
    unsigned char* buffer; // buffer that holds the raw bytes.
    int cap;    // number of bytes there is room for
    int len;    // number of bytes in the list.
    int size;   // number of bytes that each item uses.
    bool changed; // used when iterating data
} BaseList;

typedef struct {
    BaseList* list; // list to iterate
    int index;      // current index of the data in the list
} BaseListIter;

typedef enum {
    LIST_OK,
    LIST_ERROR,
    LIST_END,
    LIST_CHANGED,
} BaseListResult;

BaseList* create_base_list(int size);
void destroy_base_list(BaseList* lst);
BaseListResult add_base_list(BaseList* lst, void* data);
BaseListResult get_base_list(BaseList* lst, int index, void* data);
BaseListResult ins_base_list(BaseList* lst, int index, void* data);
BaseListResult del_base_list(BaseList* lst, int index);
BaseListResult push_base_list(BaseList* lst, void* data);
BaseListResult peek_base_list(BaseList* lst, void* data);
BaseListResult pop_base_list(BaseList* lst, void* data);
BaseListResult clr_base_list(BaseList* lst);
// iterator
BaseListIter* init_base_list_iter(BaseList* lst);
BaseListResult iter_base_list(BaseListIter* iter, void* data);
BaseListIter* init_base_list_riter(BaseList* lst);
BaseListResult riter_base_list(BaseListIter* iter, void* data);
// info about the list
void* raw_base_list(BaseList* lst);
int len_base_list(BaseList* lst);

//------------------------------------------------------
// ptrlst.c
//------------------------------------------------------
// generic pointer list.
typedef BaseList PtrList;
typedef BaseListIter PtrListIter;

static inline PtrList* create_ptr_list() {
    return create_base_list(sizeof(void*));
}

static inline void destroy_ptr_list(PtrList* h) {
    destroy_base_list(h);
}

static inline void add_ptr_list(PtrList* h, void* ptr) {
    if(!(LIST_OK == add_base_list(h, &ptr))) {
        fprintf(stderr, "Fatal Error: Cannot add a pointer to the pointer list.\n");
        exit(1);
    }
}

static inline PtrListIter* init_ptr_list_iter(PtrList* h) {
    return init_base_list_iter(h);
}

static inline void* iterate_ptr_list(PtrListIter *ptr) {
    void* val;
    if(LIST_OK == iter_base_list(ptr, &val))
        return val;
    else
        return NULL;
}

static inline void push_ptr_list(PtrList* h, void* ptr) {
    push_base_list(h, &ptr);
}

static inline void* peek_ptr_list(PtrList* h) {
    void* val;
    if(LIST_OK == peek_base_list(h, &val))
        return val;
    else
        return NULL;
}

static inline void* pop_ptr_list(PtrList* h) {
    void* val;
    if(LIST_OK == pop_base_list(h, &val))
        return val;
    else
        return NULL;
}

//--------------------------------------------------------
// str.c
//--------------------------------------------------------
// Specialize the ptr list to be a str list.
typedef BaseList StrList;
typedef BaseListIter StrListIter;
typedef BaseList Str;

/*
typedef struct {
    char* buf;
    int cap;
    int len;
    int idx;
} Str;
*/

Str* join_str_list(StrList* lst, const char* str);
Str* copy_string(Str* str);
Str* create_string(const char* str);
Str* create_string_fmt(const char* str, ...);
void destroy_string(Str* ptr);
void add_string_char(Str* ptr, int ch);
void add_string_str(Str* ptr, const char* str);
void add_string_fmt(Str* ptr, const char* str, ...);
//void reset_string(Str* ptr);
//int iterate_string(Str* ptr);

const char* raw_string(Str* ptr);
int comp_string(Str* s1, Str* s2);
int comp_string_const(Str* s1, const char* s2);


void truncate_string(Str* str, int index);
void clear_string(Str* str);
int len_string(Str* str);
void add_string_Str(Str* ptr, Str* str);
void print_string(FILE* fp, Str* str);
void printf_string(FILE* fp, Str* str, ...);

static inline StrList* create_str_list() {
    return create_base_list(sizeof(void*));
}

static inline void destroy_str_list(StrList* lst) {
    destroy_base_list(lst);
}

static inline void add_str_list(StrList* lst, Str* str) {
    Str* ptr = copy_string(str);
    if(!(LIST_OK == add_base_list(lst, &ptr))) {
        fprintf(stderr, "Fatal Error: Cannot add a string to the string list.\n");
        exit(1);
    }
}

static inline StrListIter* init_str_list_iter(StrList* lst) {
    return init_base_list_iter(lst);
}

static inline Str* iterate_str_list(StrListIter* ptr) {
    void* val;
    if(LIST_OK == iter_base_list(ptr, &val))
        return val;
    else
        return NULL;
}

static inline void push_str_list(StrList* lst, Str* str) {
    push_base_list(lst, &str);
}

static inline Str* peek_str_list(StrList* lst) {
    void* val;
    if(LIST_OK == peek_base_list(lst, &val))
        return val;
    else
        return NULL;
}

static inline Str* pop_str_list(StrList* lst) {
    void* val;
    if(LIST_OK == pop_base_list(lst, &val))
        return val;
    else
        return NULL;
}

//-----------------------------------------------------------------
// hash.c
//-----------------------------------------------------------------

typedef struct {
    const char* key;
    void* data;
    size_t size;
} _hash_node;

/*
 * If a node's key is NULL, but the bucket pointer in the table
 * is not NULL, then the bucket is a tombstone.
 */
typedef struct {
    _hash_node** table;
    int cap;
    int count;
    int tombstones;
} HashTable;

typedef enum {
    HASH_OK,
    HASH_DUP,
    HASH_NF,
} HashResult;

HashTable* create_hashtable();
void destroy_hashtable(HashTable* table);
HashResult insert_hashtable(HashTable* table, const char* key, void* data, size_t size);
HashResult find_hashtable(HashTable* tab, const char* key, void* data, size_t size);
HashResult remove_hashtable(HashTable* tab, const char* key);

//-------------------------------------------------------------
// fileio.c
//-------------------------------------------------------------
// The file stack for input files is implicitly defined.
void open_input_file(const char* fname);
int get_char();
int consume_char();

int get_line_no();
int get_col_no();
const char* get_fname();

// Since multiple output files can be open in a moment, then it is accessed
// using an opaque handle.
typedef void* FPTR;
FPTR open_output_file(const char* fname);
void close_output_file(FPTR fp);

void emit_buf(FPTR h, void* buf, unsigned int size);
void emit_fmt(FPTR h, const char* fmt, ...);
void emit_Str(FPTR h, Str* str);
void emit_str(FPTR h, const char* str);

//-------------------------------------------------------------
// cmd.c
//-------------------------------------------------------------
// Flags are a bitmask. For example, OR them together, such as
// (CMD_LIST|CMD_REQD) to specify a required list parameter.
typedef enum {
    CMD_NONE = 0x00,
    CMD_REQD = 0x01,
    CMD_LIST = 0x02,
    CMD_STR = 0x04,
    CMD_BOOL = 0x08,
    CMD_FLOAT = 0x10,
    CMD_INT = 0x20,
    CMD_SEEN = 0x40,
    CMD_HELP = 0x80,
} CmdFlag;

// Opaque handle for command line.
typedef void* CmdLine;

// Create the command line data structure.
CmdLine create_cmd_line(const char* description);

// Destroy the command line data structure.
void destroy_cmd_line(CmdLine cl);

// Add a command line parameter to the parser.
//    parm = the neme to recognize on the command line.
//    name = the name to use when retrieving a parameter.
//    dvalue = the default value of the parameter.
//    flags = controls the specify the behavior of the
//            parameter on the command line.
// If this is called with a NULL param and CMD_LIST as a
// flag, then random strings (like file names) will be
// stored under the name.
void add_cmd(CmdLine cl,
             const char* parm,
             const char* name,
             const char* help,
             const char* dvalue,
             unsigned char flags);

// Read the actual command line into the data structure and abort
// the program if there is an error.
void parse_cmd_line(CmdLine cl, int argc, char** argv);

// retrieve the value stored by the name, or publish an error.
Str* get_cmd_str(CmdLine cl, const char* name);
const char* get_cmd_raw(CmdLine cl, const char* name);
long int get_cmd_int(CmdLine cl, const char* name);
unsigned long int get_cmd_unsigned(CmdLine cl, const char* name);
double get_cmd_float(CmdLine cl, const char* name);
StrList* get_cmd_list(CmdLine cl, const char* name);
bool get_cmd_bool(CmdLine cl, const char* name);
CmdFlag get_cmd_flag(CmdLine cl, const char* name);

// Print out the current state of the data structures for debugging.
void dump_cmd_line(CmdLine cl);

//------------------------------------------------------------------------------
//
// except.c
//
//------------------------------------------------------------------------------
// This implementation of exceptions is used to handler errors. THere are no
// classes in C. This uses an enum to register an exception handler by name.
// Using the non-local goto functionality and the preprocessor of C, a
// reasonably easy to maintain exception capability can be achieved. This has to
// use macros because the current context in the function that handles the
// exceptions is required.

// These data structures should never be accessed directly.
// They need to be defined here because of the macros.
typedef struct _exception_stack_ {
    jmp_buf jmp;
    struct _exception_stack_* next;
} _ExceptionStack;

typedef struct {
    _ExceptionStack* stack;
    const char* msg;
    const char* file;
    const char* func;
    int line;
} _ExceptionState;

// defined in exceptions.c
// This prevents exceptions from being thread safe without a lot more work.
extern _ExceptionState _exception_state;

// Set up a try block
#define TRY                                                          \
    do {                                                             \
        _ExceptionStack* _exception_ptr = _ALLOC_T(_ExceptionStack); \
        _exception_ptr->next = _exception_state.stack;               \
        _exception_state.stack = _exception_ptr;                     \
        int _exception_number = setjmp(_exception_ptr->jmp);         \
        if(_exception_number == 0)

// Catch a specific exception
#define EXCEPT(num) else if(_exception_number == (num))

// Catch any exception. This can be used with the FINAL macro or it can be
// ued instead of it.
#define ANY_EXCEPT() else if(_exception_number != 0)

// FINAL and/or ANY_EXCEPT clause is REQUIRED for the system to work, and it
// MUST be the last clause in the construct.
#define FINAL                                                                      \
    else {                                                                         \
        if(_exception_state.stack == NULL) {                                       \
            fprintf(stderr, "ERROR: unhandled exception 0x%04X: %s: %s: %d: %s\n", \
                    EXCEPTION_NUM, EXCEPTION_FILE, EXCEPTION_FUNC,                 \
                    EXCEPTION_LINE, EXCEPTION_MSG);                                \
            abort();                                                               \
        }                                                                          \
        else {                                                                     \
            INTERNAL_RAISE(_exception_number);                                     \
        }                                                                          \
    }                                                                              \
    }                                                                              \
    while(0)                                                                       \
        ;

// use this to raise an exception
#define RAISE(num, m)                               \
    do {                                            \
        _exception_state.line = __LINE__;           \
        if(_exception_state.file != NULL)           \
            _FREE(_exception_state.file);           \
        _exception_state.file = _DUP_STR(__FILE__); \
        if(_exception_state.func != NULL)           \
            _FREE(_exception_state.func);           \
        _exception_state.func = _DUP_STR(__func__); \
        if(_exception_state.msg != NULL)            \
            _FREE(_exception_state.msg);            \
        _exception_state.msg = _DUP_STR(m);         \
        INTERNAL_RAISE(num);                        \
    } while(0)

// internal use only
#define INTERNAL_RAISE(num)                                 \
    do {                                                    \
        jmp_buf buf;                                        \
        _ExceptionStack* ptr = _exception_state.stack;      \
        if(ptr != NULL)                                     \
            memcpy(buf, ptr->jmp, sizeof(jmp_buf));         \
        else {                                              \
            fprintf(stderr, "Exception internal error!\n"); \
            abort();                                        \
        }                                                   \
        _exception_state.stack = ptr->next;                 \
        _FREE(ptr);                                         \
        longjmp(buf, (num));                                \
    } while(0)

// use these macros in your exception handler
#define EXCEPTION_MSG _exception_state.msg
#define EXCEPTION_FILE _exception_state.file
#define EXCEPTION_LINE _exception_state.line
#define EXCEPTION_FUNC _exception_state.func
#define EXCEPTION_NUM _exception_number

#endif /* _UTIL_H */
