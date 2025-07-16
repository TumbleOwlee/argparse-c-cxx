# CLI Argument Parser for C/C++

This repository provides a CLI argument parser library for plain C and C++. The implementation utilizing plain C is located in `./c/`. The C++ version utilizing some C++20 features is located in `./cxx/`.
Check out the examples in the respective directory for a quickstart.

## Progress

- [x] Implement C library for CLI creation
- [ ] Implement CXX library for CLI creation

## Impressions

#### Auto-generated CLI

```bash
$> ./build/c/argparse-c-flags -h

    Usage: ./build/c/argparse-c-flags [OPTIONS] [COMMAND]

    Short description of the application and its use-case.

    Required flags:

        -o, --output <PATH>    Optional output file path.

    Optional flags:

        -v, --verbose          Verbosity flag enabling more logging.
        -k, --vverbose         Verbosity flag enabling more logging.
        -t, --test             Set testing flag.
        -l, --list <FILE>      List of optional files.

    Commands:

        run    The run subcommand.

    Some additional information, e.g. copyright/license information.
```

#### Auto-generated CLI for subcommand

```bash
$> ./build/c/argparse-c-flags run -h

    Usage: ./build/c/argparse-c-flags run [OPTIONS] [COMMAND]

    The run subcommand.

    Required flags:

        -f, --flag <FLAG>    Activate some flag.

    Commands:

        show    The run subcommand.
```

#### Auto-generated CLI for subcommand of subcommand

```bash
$> ./build/c/argparse-c-flags run show -h

    Usage: ./build/c/argparse-c-flags run show [OPTIONS] INPUT VARS [VARS...]

    The run subcommand.

    Optional flags:

        -w, --what       What to show?

    Required arguments:

        INPUT    Input file path.
        VARS     Some variables.
```
