#ifndef _UTIL_H
#define _UTIL_H

#include <stdbool.h>
#include <stddef.h>

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
// ptrlst.c
//------------------------------------------------------
// generic pointer list.
typedef struct {
    void** list;
    int cap;
    int len;
    int idx;
} PtrList;

PtrList* create_ptr_list();
void destroy_ptr_list(PtrList* h);
void add_ptr_list(PtrList* h, void* ptr);
void reset_ptr_list(PtrList* h);
void* iterate_ptr_list(PtrList* h);
void push_ptr_list(PtrList* h, void* ptr);
void* peek_ptr_list(PtrList* h);
void* pop_ptr_list(PtrList* h);

//--------------------------------------------------------
// str.c
//--------------------------------------------------------
// Specialize the ptr list to be a str list.
typedef PtrList StrList;

typedef struct {
    char* buf;
    int cap;
    int len;
    int idx;
} Str;

StrList* create_str_list();
void destroy_str_list(StrList* lst);
void add_str_list(StrList* lst, Str* str);
void reset_str_list(StrList* lst);
Str* iterate_str_list(StrList* lst);
void push_str_list(StrList* lst, Str* str);
Str* peek_str_list(StrList* lst);
Str* pop_str_list(StrList* lst);
Str* join_str_list(StrList* lst, const char* str);

Str* create_string(const char* str);
Str* create_string_fmt(const char* str, ...);
void destroy_string(Str* ptr);
void add_string_char(Str* ptr, int ch);
void add_string_str(Str* ptr, const char* str);
void add_string_fmt(Str* ptr, const char* str, ...);
void reset_string(Str* ptr);
int iterate_string(Str* ptr);
const char* raw_string(Str* ptr);
int comp_str(Str* s1, Str* s2);
int comp_str_const(Str* s1, const char* s2);

Str* copy_string(Str* str);
void truncate_string(Str* str, int index);
int len_string(Str* str);
void add_string_Str(Str* ptr, Str* str);

//-----------------------------------------------------------------
// hash.c
//-----------------------------------------------------------------
typedef void* HashTable;

typedef enum {
    HASH_OK,
    HASH_DUP,
    HASH_NF,
} HashResult;

HashTable create_hash();
void destroy_hash(HashTable table);
HashResult insert_hash(HashTable table, const char* key, void* data, size_t size);
HashResult find_hash(HashTable tab, const char* key, void* data, size_t size);
HashResult remove_hash(HashTable tab, const char* key);

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

#endif /* _UTIL_H */
