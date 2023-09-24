#ifndef _CMD_H
#define _CMD_H

#include <stdbool.h>

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
const char* get_cmd_line(CmdLine cl, const char* name, bool flag);

// Print out the current state of the data structures for debugging.
void dump_cmd_line(CmdLine cl);

#endif /* _CMD_H */