#include "argparse.h"

#include <stdio.h>

int main(int argc, char const *const *argv) {
    parser_new(parser, argv[0], "Short description of the application and its use-case.",
               "Some additional information, e.g. copyright/license information.");

    // Optional parameters
    struct flag *vvv = parser_add_flag(parser, 'v', "verbose", "Verbosity flag enabling more logging.");
    add_flag(parser, verbose, 'k', "vverbose", "Verbosity flag enabling more logging.");
    add_flag(parser, test, 't', "test", "Set testing flag.");
    add_flag_value(parser, output, 'o', "output", "PATH", "Optional output file path.", SET_REQUIRED);
    add_flag_list(parser, files, 'l', "list", "FILE", "List of optional files.", SET_NONE);

    // Required parameters
    // add_req_value(parser, input, "INPUT", "Input file path.");
    // add_req_list(parser, list, "LIST", "List of values.");

    // Add command with its own arguments
    add_command(parser, run, "run", "The run subcommand.", NULL);
    cmd_add_flag_value(run, flag, 'f', "flag", "FLAG", "Activate some flag.", SET_REQUIRED);

    // Add subcommand of command
    cmd_add_subcommand(run, show, "show", "The run subcommand.", NULL);
    cmd_add_flag(show, what, 'w', "what", "What to show?");
    cmd_add_arg_value(show, input, "INPUT", "Input file path.");
    cmd_add_arg_list(show, vars, "VARS", "Some variables.");

    if (0 != parser_parse_args(parser, argv, argc)) {
        return 1;
    }

    fprintf(stdout, "verbose - Count: %d\n", flag_count(verbose));
    fprintf(stdout, "test - Count: %d\n", flag_count(test));
    if (flag_value_exists(output)) {
        fprintf(stdout, "output - Value: %s\n", flag_value_get(output));
    }

    char const *const *values = flag_list_get(files);
    for (int i = 0; i < flag_list_count(files); ++i) {
        fprintf(stdout, "list - Item %d: %s\n", i, values[i]);
    }

    if (command_is_set(run) == 1) {
        fprintf(stdout, "flag - Count: %d\n", flag_count(flag));
        values = arg_list_get(vars);
        for (int i = 0; i < arg_list_count(vars); ++i) {
            fprintf(stdout, "VARS - Item %d: %s\n", i, values[i]);
        }
    }

    parser_deinit(parser);
    return 0;
}
