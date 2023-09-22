#ifndef _CMD_H
#define _CMD_H

#include <stdbool.h>

// Opaque handle for command line.
typedef void* CmdLine;

// Flags are a bitmask.
typedef enum {
    // just a place holder
    CMD_NONE = 0x00,
    // if an option is not required, then it is optional.
    CMD_REQD = 0x01,
    // allow but not require multiple items.
    // note that lists and toggles are mutually exclusive.
	CMD_LIST = 0x02,
    // these specify toggles. no argumets are accepted.
    CMD_FALSE = 0x04,
    CMD_TRUE = 0x08,
    // If the parameter has been seen on the command line then this is set.
    CMD_SEEN = 0x10,
} CmdFlags;

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
const char* get_cmd_line(CmdLine cl, const char* name, bool flag);


#endif /* _CMD_H */