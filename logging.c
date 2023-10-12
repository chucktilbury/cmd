
#include <stdarg.h>
#include "util.h"


Logger* init_logger(FILE* stream, int vlevel, const char* preamble, ...) {

    va_list args;

    Logger* lgr = _ALLOC_T(Logger);

    if(stream != NULL)
        lgr->stream = stream;
    else
        lgr->stream = stderr;

    lgr->level = vlevel;

    va_start(args, preamble);
    int len = vsnprintf(NULL, 0, preamble, args);
    va_end(args);

    lgr->preamble = _ALLOC(len);
    va_start(args, preamble);
    vsnprintf((char*)lgr->preamble, len, preamble, args);
    va_end(args);

    return lgr;
}

void destroy_logger(Logger* lgr) {

    if(lgr != NULL) {
        if(fileno(lgr->stream) >= 3)
            fclose(lgr->stream); // it's a file

        if(lgr->preamble != NULL)
            _FREE(lgr->preamble);
        _FREE(lgr);
    }
}

void logger(Logger* lgr, int level, const char* fmt, ...) {

    assert(lgr != NULL);

    va_list args;

    if(level >= lgr->level) {
        fprintf(lgr->stream, "%s", lgr->preamble);
        va_start(args, fmt);
        vfprintf(lgr->stream, fmt, args);
        va_end(args);
    }
}
