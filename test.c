
#include <stdio.h>

#include "cmd.h"

int main(int argc, const char** argv) {

    CmdLine cmd = create_cmd_line("This is a test command line.");
    parse_cmd_line(cmd, argc, argv);

    return 0;
}