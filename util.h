#ifndef _UTIL_H
#define _UTIL_H

#include <stddef.h>

//----------------------------------------------
// mem.c
//----------------------------------------------
#define _ALLOC(s)               mem_alloc(s)
#define _ALLOC_T(t)             (t*)mem_alloc(sizeof(t))
#define _ALLOC_ARRAY(t, n)      (t*)mem_alloc(sizeof(t)*(n))
#define _REALLOC(p, s)          mem_realloc((p), (s))
#define _REALLOC_T(p, t)        (t*)mem_realloc((p), sizeof(t))
#define _REALLOC_ARRAY(p, t, n) (t*)mem_realloc((p), sizeof(t)*(n))
#define _DUP_MEM(p, s)          mem_dup((p), (s))
#define _DUP_STR(p)             mem_dup_str(p)
#define _FREE(p)                mem_free(((void*)p))

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
} PtrLst;

PtrLst* create_ptr_lst();
void destroy_ptr_lst(PtrLst* h);
void add_ptr_lst(PtrLst* h, void* ptr);
void reset_ptr_lst(PtrLst* h);
const char* iterate_ptr_lst(PtrLst* h);

//--------------------------------------------------------
// str.c
//--------------------------------------------------------
// Specialize the ptr list to be a str list.
typedef PtrLst StrLst;

StrLst* create_str_lst();
void destroy_str_lst(StrLst* lst);
void add_str_lst(StrLst* lst, const char* str);
void reset_str_lst(StrLst* lst);
const char* iterate_str_lst(StrLst* lst);

typedef struct {
    char* buf;
    int cap;
    int len;
    int idx;
} Str;

Str* create_string(const char* str);
Str* create_string_fmt(const char* str, ...);
void destroy_string(Str* ptr);
void add_string_char(Str* ptr, int ch);
void add_string_str(Str* ptr, const char* str);
void add_string_fmt(Str* ptr, const char* str, ...);
void reset_string(Str* ptr);
int iterate_string(Str* ptr);
const char* raw_string(Str* ptr);

//-------------------------------------------------------------
// cmd.c
//-------------------------------------------------------------
// Flags are a bitmask. For example, OR them together, such as
// (CMD_LIST|CMD_REQD) to specify a required list parameter.
typedef enum {
    CMD_NONE = 0x00,
    CMD_REQD = 0x01,
    CMD_LIST = 0x02,
    CMD_BOOL = 0x04,
    CMD_SEEN = 0x08,
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
void add_cmd_line(CmdLine cl,
                const char* parm,
                const char* name,
                const char* dvalue,
                unsigned char flags);

// Read the actual command line into the data structure and abort
// the program if there is an error.
void parse_cmd_line(CmdLine cl, int argc, const char** argv);

// Iterate the named command parameter. If it is not a list, then
// return the same value upon multiple calls. Otherwise, iterate the
// list and return NULL after the last item. If the flag is set and
// the item is a list, then the iterator is reset. If the parameter
// is not a list, then it's ignored.
const char* get_cmd_line(CmdLine cl, const char* name, unsigned char flag);

// Print out the current state of the data structures for debugging.
void dump_cmd_line(CmdLine cl);

#endif /* _UTIL_H */
