#include <iostream>

#include "argparse.hxx"

int main(int argc, char *argv[]) {
  auto parser = argparse::parser(argv[0], "Example application utilizing flag options.");
  auto &verbosity = parser.add_opt_flag('v', "verbose", "Enalbe verbosity level. Allows multiple occurrances.");

  if (!parser.parse(argc, argv)) {
    return 1;
  }

  // Check if flag is set
  std::cerr << "Flag present? " << (verbosity.is_set() ? "Yes" : "No") << std::endl;
  // Check how often the flag was provided
  std::cerr << "Flag count?   " << verbosity.cnt() << std::endl;

  return 0;
}
