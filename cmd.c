
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "util.h"

// Individual command line item.
typedef struct {
    const char* parm;       // string recognized from the command line
    const char* name;       // name to access the param by
    const char* help;       // help string for this item
    CmdFlag flag;           // Flags controling the behavior of this item
    // all parameters are a list. A str is a single item.
    StrList* list;
    // A boolean value is always initialized to false and it set to true
    // if it's found in the command line.
    bool bval;
} CmdItem;

static CmdItem* create_item(const char* parm,
                        const char* name, const char* help, CmdFlag flag) {

    CmdItem* ci = _ALLOC_T(CmdItem);
    ci->parm = _DUP_STR(parm);
    ci->name = _DUP_STR(name);
    ci->help = _DUP_STR(help);
    ci->flag = flag;
    ci->bval = false;

    // don't allocate the list if it's a bool
    if(!(flag & CMD_BOOL))
        ci->list = create_str_list();

    return ci;
}

static void destroy_item(CmdItem* ptr) {

    if(ptr != NULL) {
        _FREE(ptr->parm);
        _FREE(ptr->name);
        _FREE(ptr->help);

        if(!(ptr->flag & CMD_BOOL))
            destroy_str_list(ptr->list);

        _FREE(ptr);
    }
}

typedef PtrList CmdItemList;

static CmdItemList* create_ci_list() {
    return (CmdItemList*)create_ptr_list();
}

static void destroy_ci_list(CmdItemList* h) {

    if(h != NULL) {
        for(int x = 0; x < h->len; x++)
            destroy_item(h->list[x]);
        destroy_ptr_list(h);
    }
}

static void add_ci_list(CmdItemList* h, CmdItem* ptr) {
    add_ptr_list(h, ptr);
}

static void reset_ci_list(CmdItemList* h) {
    reset_ptr_list(h);
}

static CmdItem* iterate_ci_list(CmdItemList* h) {
    return iterate_ptr_list(h);
}

typedef struct {
    CmdItemList* items;
    const char* desc;
    const char* fname;
} Cmd;

// Show the help text and exit.
static void show_help(Cmd* ptr) {

    CmdItem* ci;

    printf("%s use: %s\n", ptr->fname, ptr->desc);
    reset_ci_list(ptr->items);
    while(NULL != (ci = iterate_ci_list(ptr->items))) {
        printf("  %-5s ", ci->parm);
        if(ci->flag & CMD_STR)
            printf("STR   ");
        else if(ci->flag & CMD_LIST)
            printf("LIST  ");
        else if(ci->flag & CMD_BOOL)
            printf("(bool) ");
        else
            printf("       ");

        if(ci->flag & CMD_REQD)
            printf("%s (required)", ci->help);
        else
            printf("%s", ci->help);

        printf("\n");
    }
}

// Show an error and exit.
static void show_error(Cmd* ptr, const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "ERROR: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n\n");

    show_help(ptr);
    exit(1);
}

// find exact name
CmdItem* find_by_name(Cmd* ptr, const char* name) {

    assert(ptr != NULL);
    assert(name != NULL);

    CmdItem* ci;

    reset_ci_list(ptr->items);
    while(NULL != (ci = iterate_ci_list(ptr->items))) {
        if(!strcmp(ci->name, name))
            return ci;
    }

    return NULL;
}

// find by longest match
CmdItem* find_by_parm(Cmd* ptr, const char* parm) {

    assert(ptr != NULL);
    assert(parm != NULL);

    CmdItem* ci, *crnt = NULL;
    int len = 0, max = 0;

    reset_ci_list(ptr->items);
    while(NULL != (ci = iterate_ci_list(ptr->items))) {
        len = strlen(ci->parm);
        if(!strncmp(ci->parm, parm, len)) {
            if(len > max) {
                max = len;
                crnt = ci;
            }
        }
    }

    return crnt;
}

/*************************************************************************
 * API
 */

CmdLine create_cmd_line(const char* description) {

    Cmd* ptr = _ALLOC_T(Cmd);
    ptr->desc = _DUP_STR(description);
    ptr->fname = NULL;
    ptr->items = create_ci_list();

    return ptr;
}

void destroy_cmd_line(CmdLine cl) {

    if(cl != NULL) {
        Cmd* ptr = (Cmd*)cl;
        _FREE(ptr->desc);
        if(ptr->fname != NULL)
            _FREE(ptr->fname);
        destroy_ci_list(ptr->items);
        _FREE(ptr);
    }
}

void add_cmd(CmdLine cl,
                const char* parm,
                const char* name,
                const char* help,
                const char* dvalue,
                unsigned char flags) {

    assert(cl != NULL);
    CmdItem* ci;
    Cmd* cmd = (Cmd*)cl;

    // check for developer errors.
    if(NULL != find_by_name(cmd, name)) {
        fprintf(stderr, "cmd dev error: attempt to create duplicate name: %s\n", name);
        exit(1);
    }
    else {
        reset_ci_list(cmd->items);
        while(NULL != (ci = iterate_ci_list(cmd->items))) {
            if(!strcmp(ci->parm, parm)) {
                fprintf(stderr, "cmd dev error: attempt to create duplicate parameter: %s\n", name);
                exit(1);
            }
        }
    }

    // create a new item
    ci = create_item(parm, name, help, flags);

    // set up the default value
    if((!(flags & CMD_BOOL)) && dvalue != NULL) {
        push_str_list(ci->list, create_string(dvalue));
    }

    add_ci_list(cmd->items, ci);
}

Str* get_cmd_str(CmdLine cl, const char* name) {

    assert(cl != NULL);
    assert(name != NULL);
    CmdItem* ci = find_by_name((Cmd*)cl, name);
    assert(ci != NULL);

    return peek_str_list(ci->list);
}

bool get_cmd_bool(CmdLine cl, const char* name) {

    assert(cl != NULL);
    assert(name != NULL);
    CmdItem* ci = find_by_name((Cmd*)cl, name);
    assert(ci != NULL);

    return ci->bval;
}

StrList* get_cmd_list(CmdLine cl, const char* name) {

    assert(cl != NULL);
    assert(name != NULL);
    CmdItem* ci = find_by_name((Cmd*)cl, name);
    assert(ci != NULL);

    return ci->list;
}

CmdFlag get_cmd_flag(CmdLine cl, const char* name) {

    assert(cl != NULL);
    assert(name != NULL);
    CmdItem* ci = find_by_name((Cmd*)cl, name);
    assert(ci != NULL);

    return ci->flag;
}

// split the string at len. i.e.
// string = "123456789" and len = 3
// then return a string = "456789"
// if len is greater or equal to strlen() then return NULL.
static const char* split_arg(int len, const char* str) {

    int slen = strlen(str);
    if(slen > len) {
        if(str[len] == ':' || str[len] == '=')
            len++;
        if(strlen(&str[len]) > 0)
            return &str[len];
        else
            return NULL;
    }
    else
        return NULL;
}

static int get_cats(Cmd* cmd, const char* str) {

}

static int get_dash(Cmd* cmd, CmdItem* ci, int idx, char** argv) {

    const char* str = argv[idx];

            if(ci->flag & CMD_LIST) {
                // more than one instances are allowed.
                const char* tmp = split_arg(strlen(ci->parm), str);
                if(tmp != NULL)
                    add_str_list(ci->list, tmp);
                else {

                }
            }

            // check to see if the supplied arg is the same length as the one
            // that was found on the cmd line.
            int len = strlen(ci->parm);
            if(strlen(str) > len) {
                // the arg is concatenated to the parameter
                str = &str[len];
                if(str[0] == ':' || str[0] == '=')
                    str = &str[1];

            }

}

// get one command line argument.
static int get_cmd(Cmd* cmd, int idx, char** argv) {

    CmdItem* ci;
    const char* str = argv[idx];
    if(str[0] == '-') {
        if(!strcmp(str, "-h")||!strcmp(str, "--help")||!strcmp(str, "-?")) {
            show_help(cmd);
            exit(1);
        }

        // have a defined command
        ci = find_by_parm(cmd, str);
        if(ci != NULL) {
            idx = get_dash(cmd, ci, idx, argv);
        }
        else {
            show_error(cmd, "Unknown command line argument: %s", str);
        }
    }
    else {
        // have a stand-alone string
        ci = find_by_name(cmd, "");
        if(ci != NULL)
            add_str_list(ci->list, create_string(str));
        else
            show_error(cmd, "Unexpected stand-alone argument: %s", str);
    }
    return idx+1;
}

// Read the command line from the system.
void parse_cmd_line(CmdLine cl, int argc, const char** argv) {

    Cmd* cmd = (Cmd*)cl;
    cmd->fname = _DUP_STR(argv[0]);
    int idx;

    for(idx = 1; idx < argc; /* empty */) {
        idx = get_cmd(cmd, idx, argv);
    }
}

// use for debugging....
void dump_cmd_line(CmdLine cl) {

    Cmd* cmd = (Cmd*)cl;
    CmdItem* ci;

    reset_ci_list(cmd->items);
    while(NULL != (ci = iterate_ci_list(cmd->items))) {
        printf("%s:\n", ci->name);
        printf("    %-5s%s\n", ci->parm, ci->help);
        printf("    flags: (CMD_NONE");
        if(ci->flag & CMD_REQD) printf("|CMD_REQD");
        if(ci->flag & CMD_LIST) printf("|CMD_LIST");
        if(ci->flag & CMD_STR)  printf("|CMD_STR");
        if(ci->flag & CMD_BOOL) printf("|CMD_BOOL");
        if(ci->flag & CMD_SEEN) printf("|CMD_SEEN");
        printf(")\n");
        printf("    values: ");
        if(ci->flag & CMD_BOOL)
            printf("%s\n", (ci->bval)? "true": "false");
        else {
            Str* str;
            reset_str_list(ci->list);
            while(NULL != (str = iterate_str_list(ci->list)))
                printf("        %s\n", raw_string(str));
        }
    }
}


