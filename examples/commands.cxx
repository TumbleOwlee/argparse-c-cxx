#include <iostream>

#include "argparse.hxx"

int main(int argc, char *argv[]) {
  auto parser = argparse::parser(argv[0], "Example application utilizing flag options.");
  auto &run = parser.add_command("run", "Run a given command");
  run.add_optional_arg<argparse::optional_flag>('v', "verbose", "Enalbe verbosity level. Allows multiple occurrances.");

  if (!parser.parse(argc, argv)) {
    return 1;
  }

  // Get argument to access state
  auto &verbosity = run.get_optional<argparse::optional_flag>("verbose");
  // Check if flag is set
  std::cerr << "Flag present? " << (verbosity.is_set() ? "Yes" : "No") << std::endl;
  // Check how often the flag was provided
  std::cerr << "Flag count?   " << verbosity.cnt() << std::endl;

  return 0;
}
