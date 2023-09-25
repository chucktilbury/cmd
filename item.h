#ifndef _ITEM_H
#define _ITEM_H

#include "cmd.h"

// Individual command line item.
typedef struct {
    CmdFlag flag;           // Flags controling the behavior of this item
    const char* parm;       // string recognized from the command line
    const char* name;       // name to access the param by
    const char* help;       // help string for this item
    // status
    bool seen;              // was seen by the command parser
    bool required;          // parameter is required
    // payload
    union {
        StrList* slist;  // parameter is a list of strings.
        const char* str;    // parameter is a string
    } value;
} CmdItem;



#endif /* _ITEM_H */
