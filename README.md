# cmd
Simple command line parser for C.

This is a simple library that is intended to be used with a language compiler.

## Supported features

* A parameter name is the string on the command line that causes the parser to perform some action or that introduces an argument. Parameter names always start with one or more dashes (-) and it can have one or more alphabetical characters or dashes. No digits or other punctuation characters are allowed. Names can be any valid C string.
* If the parameter type is a list, then this creates a list of strings that can be iterated.
* Required and optional parameters. The default is required. It must be marked as optional if that is intended.
* Parameters are all strings. For example, "-x" and "--extra-stuff" may or may not access the same internal object. Whether they do or not is determined by the name. If the type is not a list, then an error is generated if it is found more than once in the command line, but more than one parameter can be assigned to the same name. If more than one parameter is assigned to the same name, then they appear as OR'ed in the generated document.
* Parameter arguments in the form of "-x=123", "-x:123", "-x123" and "-x 123" are equivalent. However, something like "-xabc" can create an ambiguity. The parser resolves this by accepting the longest possible match. So if the parser knows about "-x" and "-xa", then the command line argument will be equivalent to "-xa=bc", rather than "-x=abc". But a command argument like "-xbcd" will parse as "-x=bcd".
* An optional list can be retrieved. This list can be in the middle of other parameters. If a name does not start with a "-" and it is not taken to be an argument to a parameter, then it is part of the list. If a list is not specified then arguments that are not part of a parameter will generate an error.
* All parameters are strings. The user of the parameter is required to convert it, if needed.
* The parameters "-h", "--help", and "-?" are reserved and print the help screen and exit. Note that the parameter "-hq" or "-hx" are valid stand-alone parameters like any other.

## Unsupported features

* Grouped parameters. For example, "-abc" is a single parameter name, and not "-a -b -c".
* Mutually required parameters.
* Mutually exclusive parameters.
* Duplicate parameters

## API

```C

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
} CmdFlags;

// Create the command line data structure.
CmdLine* create_cmd_line(const char* description);

// Destroy the command line data structure.
void destroy_cmd_line(CmdLine* cl);

// Add a command line parameter to the parser.
//    parm = the neme to recognize on the command line.
//    name = the name to use when retrieving a parameter.
//    dvalue = the default value of the parameter.
//    flags = controls the specify the behavior of the
//            parameter on the command line.
// If this is called with a NULL param and CMD_LIST as a
// flag, then random strings (like file names) will be
// stored under the name.
void add_cmd_line(const char* parm,
                 const char* name,
                 const char* dvalue,
                 unsigned char flags);

// Read the actual command line into the data structure and abort
// the program if there is an error.
void parse_cmd_line(CmdLine* cl, int argc, char** argv);

// Iterate the named command parameter. If it is not a list, then
// return the same value upon multiple calls. Otherwise, iterate the
// list and return NULL after the last item. If the flag is set and
// the item is a list, then the iterator is reset. If the parameter
// is not a list, then it's ignored.
const char* get_cmd_line(CmdLine* cl, const char* name, bool flag);

```

