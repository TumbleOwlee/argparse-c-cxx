#include "argparse.h"

#include <stdio.h>

int main(int argc, char const *const *argv) {
  struct parser *parser = parser_init(argv[0], "Some description with very long line, so it is forced to split the "
                                               "description over multiple lines. It should be possible and look good.");
  struct optional *verbose = parser_add_opt_flag(parser, 'v', "verbose", "Verbosity level of runtime information.");
  struct optional *special = parser_add_opt_flag(parser, 's', "special", "Verbosity level of runtime information.");
  struct command *run = parser_add_command(parser, "run", "Run something without exception.");
  struct command *exit = parser_add_command(parser, "exit", "Run something without exception.");
  struct required *files = parser_add_required_list(parser, "FILES", "List of files to execute.");
  struct required *custom = parser_add_required_list(parser, "CUSTOM", "List of files to execute.");

  if (0 != parser_parse_args(parser, argv, argc)) {
    fprintf(stderr, "Failed to parse arguments!\n");
  }

  parser_print_help(parser);

  fprintf(stderr, "Verbosity: %d\n", optional_flag_is_set(verbose));
  fprintf(stderr, "Count: %d\n", optional_flag_get_count(verbose));
  parser_deinit(parser);
  return 0;
}
