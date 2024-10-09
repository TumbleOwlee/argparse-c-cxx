# CLI Argument Parser using C++20

This is a minimalistic and simple argument parser using C++20 features. Because of the used standard, this library may not be suitable for all use-cases.

## Quickstart

```C++
// Create parser instance
auto parser = argparse::parser(argv[0], "Some description of the application. Very long descriptions will be automatically split over multiple lines.");

// Add optional flag argument
parser.add_optional_arg<argparse::optional_flag>('v', "verbose", "Enalbe verbosity level. Allows multiple occurrances.");

// Add subcommand 'run'
auto &run = parser.add_command("run", "Run a given command");

// Add optional flag to sub-command
run.add_optional_arg<argparse::optional_flag>('v', "verbose", "Enalbe verbosity level. Allows multiple occurrances.");

// Or add a sub-sub-command
auto &exec = run.add_command("exec", "Execute something");

// Perform the parsing step
if (!parser.parse(argc, argv)) {
  # Parsing failed
  return 1;
}

// Get the verbosity flag state from the parser
auto &verbosity = parser.get_optional<argparse::optional_flag>("verbose");
std::cerr << "Flag present? " << (verbosity.is_set() ? "Yes" : "No") << std::endl;
std::cerr << "Flag count?   " << verbosity.cnt() << std::endl;

// Or get the verbosity flag state of the sub-command
auto &verbosity = run.get_optional<argparse::optional_flag>("verbose");
std::cerr << "Flag present? " << (verbosity.is_set() ? "Yes" : "No") << std::endl;
std::cerr << "Flag count?   " << verbosity.cnt() << std::endl;
```
