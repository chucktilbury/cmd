# UTIL

## MEM

This is a memory allocation interface. Its main purpose is to put a wrapper around memory allocation to detect allocation errors and abort the program if it runs out of memory. This particular library is able to switch to using the Boehm-Demers-Weiser Garbage Collector, which can be had here: https://github.com/ivmai/bdwgc. If you want to use the garbage collection then install the library and build this library with the ``USE_GC`` define set and the interface to it will be used instead of the standard library.

### API

* _ALLOC(s)
* _ALLOC_T(t)
* _ALLOC_ARRAY(t, n)
* _REALLOC(p, s)
* _REALLOC_T(p, t)
* _REALLOC_ARRAY(p, t, n)
* _DUP_MEM(p, s)
* _DUP_STR(p)
* _FREE(p)

## PTRLST

## STR

## HASH

## CMD

Simple command line parser for C.

This is a simple library that is intended to be used with a language compiler. It does not attempt to be compatible with POSIX or Windows, but it does attempt to be easy to use and complete. Both ease of use for the developer who is writing the program that uses the command line, and for the end-user that is using the program is considered to be a priority.

### Supported features

* A parameter name is the string on the command line that causes the parser to perform some action or that introduces an argument. Parameter names always start with one or more dashes (-) and it can have one or more alphabetical characters or dashes. No digits or other punctuation characters are allowed.
* The general format of a command parameter is ``-name value``. Also ``-name=value``, ``-name:value`` are valid and equivalent. Any punctuation character except ```-``` is recognized as a name/value separator.  A name may have a ```-``` embedded in it, such as ``--some-name`` and be recognized as a valid single parameter name.
  * A parameter name is matched by its length, as well as its content. For example, a parameter named ``-a`` is different from a parameter named ``-abc``. When this is done, the longest match is used.
  * A value can be any valid string that can be passed on the command line. For example, ``-x="a description string"`` is valid for most operating systems and will be passed as a single string to the command line parser. However, the line ``-x=a description string`` will be passed to the program as three separate strings by most operating systems and there is no way to guess what the true intent of the user was.
* All values are handled as strings. If the developer wants to treat a value as, for example a number, then they are responsible for doing the conversion according to their needs.
* Required and optional parameters. The default is optional. It must be marked as required if that is intended.
* Parameter arguments in the form of ``-x=123``, ``-x:123``, ``-x123`` and ``-x 123`` are equivalent. However, something like ``-xabc`` can create an ambiguity. The parser resolves this by accepting the longest possible match. So if the parser knows about ``-x`` and ``-xa``, then the command line argument will be equivalent to ``-xa=bc``, rather than ``-x=abc``. But a command argument like ``-xbcd`` will parse as ``-x=bcd``.
* An option list can have one of 2 forms.
  * In the first form, the list is created in the application without a parameter name. In this case, any stray string found on the command line that does not have a ``-`` in front of it will be added to the list. For example, a list of files, such as ``gcc`` or ``clang`` accepts.
  * In the second form, the list is connected with a parameter name and separated by a comma. For example, ``-x:opt1,opt2,opt3``. When this format is used, then the parameter cannot contain spaces adjacent to the comma character, but spaces can be embedded in an option using quotes, according to the command line rules of the operating system.
* The parameters ``-h``, ``--help``, and ``-?`` are reserved and print the help screen and exit. Note that the parameter ``-hq`` or ``-hx`` are valid stand-alone parameters like any other.

### Unsupported features

* Grouped parameters. For example, ``-abc`` is a single parameter name, and not ``-a -b -c``.
* Mutually required parameters.
* Mutually exclusive parameters.
* Duplicate parameters

### API

```C
// Flags are a bitmask. For example, OR them together, such as
// (CMD_LIST|CMD_REQD) to specify a required list parameter.
typedef enum {
    CMD_NONE = 0x00,
    CMD_REQD = 0x01,
    CMD_LIST = 0x02,
    CMD_BOOL = 0x04,
} CmdFlag;

// Use an opaque handle.
typedef void* CmdLine;

// Create the command line data structure.
CmdLine create_cmd(const char* description);

// Destroy the command line data structure.
void destroy_cmd(CmdLine cmd);

// Add a command line parameter to the parser.
//    parm = the name to recognize on the command line.
//    name = the name to use when retrieving a parameter.
//    dvalue = the default value of the parameter.
//    flags = controls the specify the behavior of the
//            parameter on the command line.
// If this is called with a NULL param and CMD_LIST as a
// flag, then random strings (like file names) will be
// stored under the name.
void add_cmd_item(CmdLine cmd,
                 const char* parm,
                 const char* name,
                 const char* dvalue,
                 unsigned char flags);

// Read the actual command line into the data structure and abort
// the program if there is an error.
void parse_cmd(CmdLine cmd, int argc, char** argv);

// If the name was defined as a list parameter, then reset the iterator.
// Otherwise, simply return without doing anything.
void reset_cmd(CmdLine cmd, const char* name);

// Get the command line string. If the value is defined as a boolean,
// then return NULL for false or a pointer to a blank string for true.
// Otherwise, if the value is defined as a string, then return the
// string if it is defined, or NULL if it is not. If the command was
// defined as a list then return the list iteration.

// return the parameter as a "cooked" string
Str* get_cmd_str(CmdLine cl, const char* name);
// return the parameter as a long int. Note that the user is responsible for
// checking the validity of the number.
long int get_cmd_int(CmdLine cl, const char* name);
// return the parameter as an unsigned long int. Note that the user is
// responsible for checking the validity of the number.
unsigned long int get_cmd_unsigned(CmdLine cl, const char* name);
// return the parameter as a double float
double get_cmd_float(CmdLine cl, const char* name);
// return an iterable list of strings
StrList* get_cmd_list(CmdLine cl, const char* name);
// return the boolean value. The default is always false.
bool get_cmd_bool(CmdLine cl, const char* name);
// return the flag so you can see what the type is.
CmdFlag get_cmd_flag(CmdLine cl, const char* name);
```
