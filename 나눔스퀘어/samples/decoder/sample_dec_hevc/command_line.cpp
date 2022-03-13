#include <stdio.h>
#include "command_line.h"
#include "sample_common_args.h"

// Parsing command line args
bool CommandLine::initialize(int argc, char* argv[])
{
    // Parse the command line
    if (argc == 1 || parse_args(argc - 1, argv + 1, m_params, m_custom_params) < 0) {
        printUsage(argv[0]);
        return false;
    }

    if (height > 16384 || width > 16384) {
        height = 0;
        width = 0;
        fprintf(stderr, "Warning: height and width must be less or equal to 16384. Set default 0.\n");
        return false;
    }

    return true;
}

void CommandLine::printUsage(const char* appname)
{
    fprintf(stderr, "Usage:\n%s -i <path> [options]\n\n", appname);
    fprintf(stderr, "Options:\n");
    print_help(m_params, m_custom_params);
}
