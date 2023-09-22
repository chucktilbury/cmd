
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "cmd.h"
#include "mem.h"

// Individual command line item.
typedef struct {
    const char** value; // List of character pointers.
    int cap;            // number of list slots
    int len;            // number of list entries
    int idx;            // index used to iterate list
    const char* parm;   // string recognized from the command line
    const char* name;   // name to access the param by
    const char* def;    // default value
    unsigned char flags; //
} _cmd_item_t;

// Command line interface struct.
typedef struct {
    _cmd_item_t** list; // dynamic array of CmdItem
    int cap;            // list capacity.
    int len;            // number of items in the list.
    const char* description; // description.
    const char* fname;  // program file name (e.g. argv[0])
} _cmd_line_t;

// if the item exists, then return a pointer to is, else return NULL.
static _cmd_item_t* find_item(_cmd_line_t* ptr, const char* name) {

    for(int x = 0; x < ptr->len; x++) {
        const char* str = ptr->list[x]->name;
        if(strcmp(str, name) == 0)
            return ptr->list[x];
    }

    return NULL;
}

// select the longest match and return a pointer to it, else return NULL.
static _cmd_item_t* check_item(_cmd_line_t* ptr, const char* parm) {

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

    ptr->parm = _DUP_STR(parm);
    ptr->name = _DUP_STR(name);
    ptr->def = _DUP_STR(def_val);
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

    return 0;
}

// Check that all required parameters are present and have values. If they are
// not, then post an error and abort the program.
static void check_required(_cmd_line_t* ptr) {

    for(int x = 0; x < ptr->len; x++) {
        _cmd_item_t* ci = ptr->list[x];
        if(ci->flags & CMD_REQD && !(ci->flags & CMD_SEEN)) {
            fprintf(stderr, "CMD ERROR: Required parameter not found: %s\n", ci->parm);
            exit(1);
        }
    }
}

// Show the help text and exit.
static void show_help(_cmd_line_t* ptr) {

    printf("%s: ", "blart!");
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

    if(find_item(ptr, name) == NULL) {
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
    _cmd_item_t* ci = find_item(ptr, name);

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
