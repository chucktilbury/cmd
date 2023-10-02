
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
    int max_args;
    int index;
} Cmd;

// Show the help text and exit.
static void show_help(Cmd* ptr) {

    CmdItem* ci;

    printf("use: %s ", ptr->fname);
    reset_ci_list(ptr->items);
    int len = 0;
    while(NULL != (ci = iterate_ci_list(ptr->items))) {
        if(strlen(ci->parm) > 0)
            printf("[%s] ", ci->parm);
        else
            printf("[file list] ");
        int tmp = strlen(ci->parm);
        if(tmp > len)
            len = tmp;
    }
    len /= 2;

    printf("\n\n%s\n\n", ptr->desc);
    reset_ci_list(ptr->items);
    while(NULL != (ci = iterate_ci_list(ptr->items))) {
        printf(" %*s%*s ", (int)(len+(strlen(ci->parm)/2)), ci->parm, (int)(len-(strlen(ci->parm)/2)), "");

        if(ci->flag & CMD_STR)
            printf("(STR)   ");
        else if(ci->flag & CMD_LIST)
            printf("(LIST)  ");
        else if(ci->flag & CMD_INT)
            printf("(INT)   ");
        else if(ci->flag & CMD_LIST)
            printf("(FLOAT) ");
        else if(ci->flag & CMD_BOOL)
            printf("(BOOL)  ");
        else
            printf("        ");

        if(ci->flag & CMD_REQD)
            printf("- %s (required)", ci->help);
        else
            printf("- %s", ci->help);
        printf("\n");
    }
    printf("\n\n");
}

// Show an error and exit.
static void show_error(Cmd* ptr, const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "CMD ERROR: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n\n");

    show_help(ptr);
    exit(1);
}

// find exact name
static CmdItem* find_by_name(Cmd* ptr, const char* name) {

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
static CmdItem* find_by_parm(Cmd* ptr, const char* parm) {

    assert(ptr != NULL);
    assert(parm != NULL);
    CmdItem* ci, *crnt = NULL;
    int len = 0, max = 0;

    reset_ci_list(ptr->items);
    while(NULL != (ci = iterate_ci_list(ptr->items))) {
        len = strlen(ci->parm);
        if(!strncmp(ci->parm, parm, len)) {
            if(len >= max) {
                max = len;
                crnt = ci;
            }
        }
    }

    return crnt;
}

static void increment_index(Cmd* cmd) {

    //cmd->index ++;

    if(cmd->index < cmd->max_args)
        cmd->index++;
    else
        show_error(cmd, "expected a command parameter");
}

static void save_cmd(Cmd* cmd, CmdItem* ci, const char* str) {

    if(ci->flag & CMD_SEEN && !(ci->flag & CMD_LIST))
        show_error(cmd, "invalid duplicate flag found: \"%s\"", ci->parm);
    else
        ci->flag |= CMD_SEEN;

    if(ci->flag & CMD_BOOL)
        ci->bval = true;
    else if(ci->flag & (CMD_LIST|CMD_STR|CMD_INT|CMD_FLOAT))
        add_str_list(ci->list, create_string(str));
    else
        show_error(cmd, "invalid flag value: 0x%02X", ci->flag);
}

static void get_cats(Cmd* cmd, CmdItem* ci, int len, char* str) {

    char* tpt = &str[len];
    if(tpt[0] == ':' || tpt[0] == '=')
        tpt++;

    save_cmd(cmd, ci, tpt);
}

static void get_dash(Cmd* cmd, CmdItem* ci, char** argv) {

    char* str = argv[cmd->index];
    size_t len = strlen(ci->parm);

    if(strlen(str) != len)
        get_cats(cmd, ci, (int)len, str);
    else {
        increment_index(cmd);
        if(cmd->index < cmd->max_args) {
            str = argv[cmd->index];
            if(str[0] == '-')
                show_error(cmd, "a command argument cannot start with a '-': \"%s\"", str);
            else
                save_cmd(cmd, ci, str);
        }
        else
            show_error(cmd, "unexpected end of command line");
    }
}

// get one command line argument.
static void get_cmd(Cmd* cmd, char** argv) {

    CmdItem* ci;
    const char* str = argv[cmd->index];
    if(str[0] == '-') {
        // have a defined command
        ci = find_by_parm(cmd, str);
        if(ci != NULL) {
            if(ci->flag & CMD_HELP) {
                show_help(cmd);
                exit(0);
            }
            else
                get_dash(cmd, ci, argv);
        }
        else
            show_error(cmd, "unknown command line argument: %s", str);
    }
    else {
        // have a stand-alone string
        ci = find_by_parm(cmd, "");
        if(ci != NULL) {
            add_str_list(ci->list, create_string(str));
            ci->flag |= CMD_SEEN;
        }
        else
            show_error(cmd, "unexpected stand-alone argument: %s", str);
    }

    increment_index(cmd);
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
    if((!(flags & CMD_BOOL)) && dvalue != NULL)
        push_str_list(ci->list, create_string(dvalue));
    else
        ci->bval = false;

    add_ci_list(cmd->items, ci);
}

Str* get_cmd_str(CmdLine cl, const char* name) {

    assert(cl != NULL);
    assert(name != NULL);
    CmdItem* ci = find_by_name((Cmd*)cl, name);
    assert(ci != NULL);

    return peek_str_list(ci->list);
}

long int get_cmd_int(CmdLine cl, const char* name) {

    Str* s = get_cmd_str(cl, name);

    return strtol(raw_string(s), NULL, 10);
}

unsigned long int get_cmd_unsigned(CmdLine cl, const char* name) {

    Str* s = get_cmd_str(cl, name);

    return strtol(raw_string(s), NULL, 16);
}

double get_cmd_float(CmdLine cl, const char* name) {

    Str* s = get_cmd_str(cl, name);

    return strtod(raw_string(s), NULL);
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

// Read the command line from the system.
void parse_cmd_line(CmdLine cl, int argc, char** argv) {

    Cmd* cmd = (Cmd*)cl;
    cmd->fname = _DUP_STR(argv[0]);
    cmd->max_args = argc;
    cmd->index = 1;

    while(cmd->index < cmd->max_args)
        get_cmd(cmd, argv);

    CmdItem* ci;
    reset_ci_list(cmd->items);
    while(NULL != (ci = iterate_ci_list(cmd->items))) {
        if(ci->flag & CMD_REQD && !(ci->flag & CMD_SEEN)) {
            if(strlen(ci->parm) == 0)
                show_error(cmd, "required file list not seen");
            else
                show_error(cmd, "required parameter not seen: \"%s\"", ci->parm);
        }
    }
}

// example use and use for debugging....
void dump_cmd_line(CmdLine cl) {

    Cmd* cmd = (Cmd*)cl;
    CmdItem* ci;

    reset_ci_list(cmd->items);
    while(NULL != (ci = iterate_ci_list(cmd->items))) {
        printf("%s:\n", ci->name);
        printf("    %s -- %s\n", ci->parm, ci->help);
        printf("    flags: (CMD_NONE");
        if(ci->flag & CMD_REQD) printf("|CMD_REQD");
        if(ci->flag & CMD_LIST) printf("|CMD_LIST");
        if(ci->flag & CMD_STR)  printf("|CMD_STR");
        if(ci->flag & CMD_BOOL) printf("|CMD_BOOL");
        if(ci->flag & CMD_SEEN) printf("|CMD_SEEN");
        if(ci->flag & CMD_HELP) printf("|CMD_HELP");
        if(ci->flag & CMD_FLOAT) printf("|CMD_FLOAT");
        if(ci->flag & CMD_INT) printf("|CMD_INT");
        printf(")\n");
        printf("    values: ");
        if(ci->flag & CMD_BOOL)
            printf("%s\n", (ci->bval)? "true": "false");
        else {
            Str* str;
            reset_str_list(ci->list);
            while(NULL != (str = iterate_str_list(ci->list)))
                printf(" %s", raw_string(str));
        }
        printf("\n");
    }
    printf("\n\n");
}


