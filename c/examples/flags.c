#include "argparse.h"

#include <stdio.h>

int main(int argc, char const *const *argv) {
  parser_new(parser, argv[0], "Short description of the application and its use-case.");

  // Optional parameters
  add_opt_flag(parser, verbose, 'v', "verbose", "Verbosity flag enabling more logging.");
  add_opt_flag(parser, test, 't', "test", "Set testing flag.");
  add_opt_value(parser, output, 'o', "output", "Optional output file path.");
  add_opt_list(parser, files, 'l', "list", "List of optional files.");

  // Required parameters
  // add_req_value(parser, input, "INPUT", "Input file path.");
  // add_req_list(parser, list, "LIST", "List of values.");

  // Add command with its own arguments
  add_command(parser, run, "run", "The run subcommand.");
  cmd_add_opt_flag(run, flag, 'f', "flag", "Activate some flag.");

  // Add subcommand of command
  cmd_add_subcommand(run, show, "show", "The run subcommand.");
  cmd_add_opt_flag(show, what, 'w', "what", "What to show?");
  cmd_add_req_list(show, vars, "VARS", "Some variables.");

  if (0 != parser_parse_args(parser, argv, argc)) {
    return 1;
  }

  fprintf(stdout, "verbose - Count: %d\n", optional_flag_count(verbose));
  fprintf(stdout, "test - Count: %d\n", optional_flag_count(test));
  if (optional_value_exists(output)) {
    fprintf(stdout, "output - Value: %s\n", optional_value_get(output));
  }

  char const *const *values = optional_list_get(files);
  for (int i = 0; i < optional_list_count(files); ++i) {
    fprintf(stdout, "list - Item %d: %s\n", i, values[i]);
  }

  if (command_is_set(run) == 1) {
    fprintf(stdout, "flag - Count: %d\n", optional_flag_count(flag));
    values = required_list_get(vars);
    for (int i = 0; i < required_list_count(vars); ++i) {
      fprintf(stdout, "VARS - Item %d: %s\n", i, values[i]);
    }
  }

  parser_deinit(parser);
  return 0;
}
