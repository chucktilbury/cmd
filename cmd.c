
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "cmd.h"
#include "mem.h"

// string list.
typedef struct {
    void** list;
    int cap;
    int len;
    int idx;
} _ptr_lst_t;

static _ptr_lst_t* create_ptr_lst() {

    _ptr_lst_t* ptr = _ALLOC_T(_ptr_lst_t);
    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->idx = 0;
    ptr->list = _ALLOC_ARRAY(const char*, ptr->cap);

    return ptr;
}

static void destroy_ptr_lst(_ptr_lst_t* lst) {

    if(lst != NULL) {
        _FREE(lst->list);
        _FREE(lst);
    }
}

// Add a pointer to the pointer list. There is no sense of allocating memory
// for the added pointer. The caller is responsible for managing the memory.
static void add_ptr_lst(_ptr_lst_t* lst, void* ptr) {

    assert(lst != NULL);
    if(lst->len+1 > lst->cap) {
        lst->cap <<= 1;
        lst->list = _REALLOC_ARRAY(lst->list, void*, lst->cap);
    }

    lst->list[lst->len] = ptr;
    lst->len++;
}

// Reset the iterator to the beginning of the list. There is no return value.
static void reset_ptr_lst(_ptr_lst_t* lst) {

    assert(lst != NULL);
    lst->idx = 0;
}

// Iterate pointer list. When there are no more items to iterate, then return
// a NULL pointer.
static const char* iterate_ptr_lst(_ptr_lst_t* lst) {

    assert(lst != NULL);
    const char* ptr = NULL;

    if(lst->idx < lst->len) {
        ptr = lst->list[lst->idx];
        lst->idx++;
    }

    return ptr;
}

// Specialize the ptr list to be a str list.
typedef _ptr_lst_t _str_lst_t;

static _str_lst_t* create_str_lst() {
    return (_str_lst_t*)create_ptr_lst();
}

static void destroy_str_lst(_str_lst_t* lst) {
    for(int x = 0; x < lst->len; x++)
        _FREE(lst->list[x]);
    destroy_ptr_lst(lst);
}

static void add_str_lst(_str_lst_t* lst, const char* str) {
    add_ptr_lst(lst, (void*)_DUP_STR(str));
}

static void reset_str_lst(_str_lst_t* lst) {
    reset_ptr_lst(lst);
}

static const char* iterate_str_lst(_str_lst_t* lst) {
    return (const char*)iterate_ptr_lst(lst);
}

// types of values
typedef enum {
    CMD_STR,
    CMD_BOOL,
    CMD_INUM,
    CMD_FNUM,
    CMD_LIST,
} _cmd_type_t;

// Individual command line item.
typedef struct {
    _cmd_type_t type;       // type of arg in the union
    const char* parm;       // string recognized from the command line
    const char* name;       // name to access the param by
    const char* help;       // help string for this item
    // status
    bool seen;              // was seen by the command parser
    bool required;          // parameter is required
    // payload
    union {
        _str_lst_t* slist;  // parameter is a list of strings.
        const char* str;    // parameter is a string
        bool bval;          // parameter is a boolean
        long int inum;      // parameter is an integer
        double fnum;        // parameter is a float
    } value;
} _cmd_item_t;

// Specialize the ptr list to be a command item list.
typedef _ptr_lst_t _ci_lst_t;

// Command line interface struct.
typedef struct {
    _ci_lst_t* list;        // dynamic array of _cmd_item_t
    const char* description;// description.
    const char* fname;      // program file name (e.g. argv[0])
} _cmd_line_t;

static _cmd_item_t* alloc_cmd_item(const char* name,
                        const char* parm,
                        const char* help,
                        bool reqd) {

    // NUlL paramters not allowed.
    assert(name != NULL);
    assert(parm != NULL);
    assert(help != NULL);

    // allocate the generic data structure
    _cmd_item_t* ptr = _ALLOC_T(_cmd_item_t);
    ptr->name = _DUP_STR(name);
    ptr->parm = _DUP_STR(parm);
    ptr->help = _DUP_STR(help);
    ptr->seen = false;
    ptr->required = reqd;

    return ptr;
}

// create a CI for a list of strings
static _cmd_item_t* create_list_item(const char* name,
                        const char* parm, const char* help,
                        const char* value, bool reqd) {

    _cmd_item_t* ptr = alloc_cmd_item(name, parm, help, reqd);

    ptr->type = CMD_LIST;
    ptr->value.slist = create_str_lst();
    if(value != NULL)
        add_str_lst(ptr->value.slist, value);

    return ptr;
}

// create a CI for a boolean item. Boolean items are normally off and are
// switched on if they are found on the cammand line.
static _cmd_item_t* create_bool_item(const char* name,
                        const char* parm, const char* help,
                        bool value, bool reqd) {

    _cmd_item_t* ptr = alloc_cmd_item(name, parm, help, reqd);

    ptr->type = CMD_BOOL;
    ptr->value.bval = value;

    return ptr;
}

// create a CI for a single string item. The item can be NULL.
static _cmd_item_t* create_str_item(const char* name,
                        const char* parm, const char* help,
                        const char* value, bool reqd) {

    _cmd_item_t* ptr = alloc_cmd_item(name, parm, help, reqd);

    ptr->type = CMD_STR;
    if(value == NULL)
        ptr->value.str = NULL;
    else
        ptr->value.str = _DUP_STR(value);

    return ptr;
}

// create a CI for an integer, as defined by strtol(s, NULL, 10)
static _cmd_item_t* create_int_item(const char* name,
                        const char* parm, const char* help,
                        long int value, bool reqd) {

    _cmd_item_t* ptr = alloc_cmd_item(name, parm, help, reqd);

    ptr->type = CMD_INUM;
    ptr->value.inum = value;

    return ptr;
}

// create a CI for a float, as defined by strtod(s, NULL).
static _cmd_item_t* create_float_item(const char* name,
                        const char* parm, const char* help,
                        double value, bool reqd) {

    _cmd_item_t* ptr = alloc_cmd_item(name, parm, help, reqd);

    ptr->type = CMD_FNUM;
    ptr->value.fnum = value;

    return ptr;
}

static _ci_lst_t* create_ci_lst() {
    return (_ci_lst_t*)create_ptr_lst();
}

static void destroy_ci_lst(_ci_lst_t* lst) {
    for(int x = 0; x < lst->len; x++)
        destroy_item(lst->list[x]);
    destroy_ptr_lst(lst);
}

static void add_ci_lst(_ci_lst_t* lst, _cmd_item_t* ci) {
    add_ptr_lst(lst, ci);
}

static void reset_ci_lst(_ci_lst_t* lst) {
    reset_ptr_lst(lst);
}

static const char* iterate_ci_lst(_ci_lst_t* lst) {
    return (const char*)iterate_ptr_lst(lst);
}

// destroy a CI item
static void destroy_item(_cmd_item_t* ci) {

    if(ci != NULL) {
        _FREE(ci->name);
        _FREE(ci->parm);
        _FREE(ci->help);

        if(ci->type == CMD_STR && ci->value.str != NULL)
            _FREE(ci->value.str);
        else if(ci->type == CMD_LIST && ci->value.slist != NULL)
            destroy_str_lst(ci->value.slist);

        _FREE(ci);
    }
}















// Show the help text and exit.
static void show_help(_cmd_line_t* ptr) {

    printf("%s: ", "blart!");
}

// Show an error and exit.
static void show_error(_cmd_line_t* ptr, const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "ERROR: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n\n");

    show_help(ptr);
    exit(1);
}

// if the item exists, then return a pointer to is, else return NULL.
static _cmd_item_t* find_item_by_name(_cmd_line_t* ptr, const char* name) {

    for(int x = 0; x < ptr->len; x++) {
        const char* str = ptr->list[x]->name;
        if(strcmp(str, name) == 0)
            return ptr->list[x];
    }

    return NULL;
}

// select the longest partial match and return a pointer to it, else return
// NULL.
static _cmd_item_t* find_item_by_parm(_cmd_line_t* ptr, const char* parm) {

    int len = 0;
    int max = 0;
    int idx = -1;
    const char* str;

    for(int x = 0; x < ptr->len; x++) {
        str = ptr->list[x]->parm;
        len = strlen(str);
        if(strncmp(str, parm, len) == 0) {
            if(max < len) {
                max = len;
                idx = x;
            }
        }
    }

    if(idx >= 0)
        return ptr->list[idx];
    else
        return NULL;
}

// create and initialize a new parameter to be added to the
// command line parser.
static _cmd_item_t* create_item(const char* name,
                const char* parm,
                const char* def_val,
                unsigned char flags) {

    _cmd_item_t* ptr = _ALLOC_T(_cmd_item_t);
    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->idx = 0;
    ptr->value = _ALLOC_ARRAY(const char*, ptr->cap);

    if(parm != NULL)
        ptr->parm = _DUP_STR(parm);
    else
        ptr->parm = _DUP_STR("");

    if(name != NULL)
        ptr->name = _DUP_STR(name);
    else
        ptr->name = _DUP_STR("");

    if(def_val != NULL)
        ptr->def = _DUP_STR(def_val);
    else
        ptr->def = _DUP_STR("");

    ptr->flags = flags;

    return ptr;
}

// Destroy a _cmd_item_t*
static void destroy_item(_cmd_item_t* ptr) {

    if(ptr != NULL) {
        if(ptr->parm != NULL)
            _FREE(ptr->parm);
        if(ptr->name != NULL)
            _FREE(ptr->name);
        if(ptr->def != NULL)
            _FREE(ptr->def);

        for(int x = 0; x < ptr->len; x++) {
            _FREE(ptr->value[x]);
        }

        _FREE(ptr->value);
        _FREE(ptr);
    }
}

// Add a value to the command value item from the command line.
static void add_value(_cmd_item_t* ci, const char* val) {

    if(ci->len+1 > ci->cap) {
        ci->cap <<= 1;
        ci->value = _REALLOC_ARRAY(ci->value, const char*, ci->cap);
    }

    ci->value[ci->len] = _DUP_STR(val);
    ci->len++;
}

// Read the actual command line for a single parameter and store it according
// to the setup given by the add_cmd_line() functions. If an error is
// encountered, then post an error and abort the program.
static int get_parm(_cmd_line_t* ptr, int idx, const char** argv) {

    const char* str = argv[idx];
    if(str[0] == '-') {
        // Have a parameter. Find it in the list of parameters. If it doesn't
        // exist, then publish an error.
        _cmd_item_t* ci = find_item_by_parm(ptr, str);

        // If the parm does not exist in the data, then it's an error
        if(ci == NULL)
            show_error(ptr, "Unknown command parameter: %s\n", str);

        // If it has been seen and it's not a list, then that is an error.
        if(ci->flags & CMD_SEEN && !(ci->flags & CMD_LIST))
            show_error(ptr, "Duplicate command parameter is not a list: %s", ci->parm);

        // check if it's a true or false toggle
        if(ci->flags & CMD_TRUE || ci->flags & CMD_FALSE) {

        }

        // ci has the value of the parameter definition. See if the value
        // is embedded in this string, or if it's a separate string.
        int plen = strlen(ci->parm);
        if(strlen(str) > plen) {
            // The command line object is longer than the parm definition.
            char *spt = _DUP_STR(&str[plen]); // strip it leaving the value
            char* value;
            if(ispunct(spt[0]))
                value = &spt[1];
            else
                value = spt;

            add_value(ci, value);
            _FREE(spt);
        }
        else {
            idx++;
            str = argv[idx];

            if(str[0] == '-')
                show_error(ptr, "Expected a argument but got a parameter: %s", str);

            add_value(ci, str);
        }

        idx++;
        ci->flags |= CMD_SEEN;
    }
    else {
        // Have a stray string. See if there is a list to put it into or else
        // post an error and abort the program.
        _cmd_item_t* ci = find_item_by_parm(ptr, "");

        // If the parm does not exist in the data, then it's an error
        if(ci == NULL)
            show_error(ptr, "Unknown positional argument: %s", str);

        // If it has been seen and it's not a list, then that is an error.
        if(ci->flags & CMD_SEEN && !(ci->flags & CMD_LIST))
            show_error(ptr, "Invalid positional argument: %s", str);

        add_value(ci, str);
        ci->flags |= CMD_SEEN;
        idx++;
    }

    return idx;
}

// Check that all required parameters are present and have values. If they are
// not, then post an error and abort the program.
static void check_required(_cmd_line_t* ptr) {

    for(int x = 0; x < ptr->len; x++) {
        _cmd_item_t* ci = ptr->list[x];
        if(ci->flags & CMD_REQD && !(ci->flags & CMD_SEEN))
            show_error(ptr, "Required parameter not found: %s\n", ci->parm);
    }
}

//------------------------------------------------------------------------
// API
//------------------------------------------------------------------------

/*
 * Create the command line data structure. This must be called before any
 * of these API.
 */
CmdLine create_cmd_line(const char* description) {

    assert(description != NULL);

    _cmd_line_t* ptr = _ALLOC_T(_cmd_line_t);
    ptr->cap = 1 << 3;
    ptr->len = 0;
    ptr->list = _ALLOC_ARRAY(_cmd_item_t*, ptr->cap);

    ptr->description = _DUP_STR(description);
    ptr->fname = NULL;

    return ptr;
}

/*
 * Destroy the command line data structure and free all of the memory.
 */
void destroy_cmd_line(CmdLine cl_ptr) {

    assert(cl_ptr != NULL);
    _cmd_line_t* ptr = (_cmd_line_t*)cl_ptr;

    if(ptr->list != NULL) {
        for(int x = 0; x < ptr->len; x++)
            destroy_item(ptr->list[x]);
        _FREE(ptr->list);
    }

    if(ptr->description != NULL)
        _FREE(ptr->description);

    if(ptr->fname != NULL)
        _FREE(ptr->fname);

    _FREE(ptr);
}

/*
 * Add a command line parameter to the parser.
 *
 *   parm = the name to recognize on the command line.
 *   name = the name to use when retrieving a parameter.
 *   dvalue = the default value of the parameter.
 *   flags = controls the specify the behavior of the
 *           parameter on the command line.
 *
 * If this is called with a NULL param and CMD_LIST as a
 * flag, then random strings (like file names) will be
 * stored under the name.
 */
void add_cmd_line(CmdLine cl_ptr,
                const char* parm,
                const char* name,
                const char* dvalue,
                unsigned char flags) {

    assert(cl_ptr != NULL);
    assert(name != NULL);
    _cmd_line_t* ptr = (_cmd_line_t*)cl_ptr;

    if(find_item_by_name(ptr, name) == NULL) {
        if(ptr->len+1 > ptr->cap) {
            ptr->cap <<= 1;
            ptr->list = _REALLOC_ARRAY(ptr->list, _cmd_item_t*, ptr->cap);
        }

        ptr->list[ptr->len] = create_item(name, parm, dvalue, flags);
        ptr->len++;
    }
    else {
        // developer error: duplicate names are not allowed
        fprintf(stderr, "cmd dev error: duplicate names: %s\n", name);
        exit(1);
    }
}

/*
 * Read the actual command line into the data structure and abort the program
 * if there is an error on the command line. This implements a simple state
 * machine that parses the individual strings given by argv[].
 */
void parse_cmd_line(CmdLine cl_ptr, int argc, const char** argv) {

    assert(cl_ptr != NULL);
    assert(argc > 0);
    assert(argv != NULL);

    _cmd_line_t* ptr = (_cmd_line_t*)cl_ptr;

    ptr->fname = _DUP_STR(argv[0]);

    for(int idx = 1; idx < argc; /* empty */) {
        idx = get_parm(ptr, idx, argv);
    }

    check_required(ptr);
}

/*
 * Iterate the named command parameter. If it is not a list, then return the
 * same value upon multiple calls. Otherwise, iterate the list and return NULL
 * after the last item. If the flag is set and the item is a list, then the
 * iterator is reset. If the parameter is not a list, then it's ignored.
 */
const char* get_cmd_line(CmdLine cl_ptr, const char* name, bool flag) {

    assert(cl_ptr != NULL);
    assert(name != NULL);
    _cmd_line_t* ptr = (_cmd_line_t*)cl_ptr;

    // This find is done every time to allow more than one item to be
    // iterated at the same time. This will likely need to be optimized if
    // a lot of iteration is called for.
    _cmd_item_t* ci = find_item_by_name(ptr, name);

    if(ci != NULL) {
        if(ci->flags & CMD_LIST) {
            if(flag)
                ci->idx = 0;

            const char* str = NULL;
            if(ci->idx < ci->len) {
                str = ci->value[ci->idx];
                ci->idx++;
            }

            return str;
        }
        else {
            return ci->value[0];
        }
    }
    else {
        // developer error
        fprintf(stderr, "cmd dev error: attempt to iterate non-existant item: %s\n", name);
        exit(1);
    }

    // cannot happen, but make the compiler happy
    return NULL;
}

/*
 * Print out the current state of the data structures for debugging.
 */
void dump_cmd_line(CmdLine cl) {

    assert(cl != NULL);
    _cmd_line_t* ptr = (_cmd_line_t*)cl;
}
