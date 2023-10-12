
#include <stdarg.h>
#include "util.h"

static int errors = 0;
static int warnings = 0;

void generic_error_msg(const char* preamble, const char* fmt, ...) {

    va_list args;

    fprintf(stderr, "%s", preamble);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void error(const char* fmt, ...) {

    va_list args;

    va_start(args, fmt);
    generic_error_msg("Error:", fmt, args);
    va_end(args);
    errors++;
}

void warning(const char* fmt, ...) {

    va_list args;

    va_start(args, fmt);
    generic_error_msg("Warning:", fmt, args);
    va_end(args);
    warnings++;
}

void fatal(const char* fmt, ...) {

    va_list args;

    va_start(args, fmt);
    generic_error_msg("Fatal Error:", fmt, args);
    va_end(args);
    errors++;
    exit(1);
}

int get_errors() {

    return errors;
}

int get_warnings() {

    return warnings;
}
