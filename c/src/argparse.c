/*********************************************************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 David Loewe
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparse.h"

/*********************************************************************************************************************
 * optional
 *********************************************************************************************************************/

struct optional {
  char _short;
  char const *_long;
  char const *_desc;

  int _count;
  char const *const *_values;

  int (*takes)();
  int (*parse)(struct optional *ctx, char const *const *, int);
};

static void optional_init(struct optional *ctx, char const flag, char const *const l_flag, char const *const desc,
                          int (*takes)(), int (*parse)(struct optional *, char const *const *, int)) {
  ctx->_short = flag;
  ctx->_long = l_flag;
  ctx->_desc = desc;
  ctx->_count = 0;
  ctx->_values = NULL;
  ctx->takes = takes;
  ctx->parse = parse;
}

/*********************************************************************************************************************
 * optional_flag
 *********************************************************************************************************************/

static int optional_flag_takes() { return 0; }

static int optional_flag_parse(struct optional *ctx, char const *const *argv, int argc) {
  ctx->_count += 1;
  return 0;
}

int optional_flag_count(struct optional *flag) {
  if (flag != NULL) {
    return flag->_count;
  } else {
    return -1;
  }
}

int optional_flag_set(struct optional *flag) {
  if (flag != NULL) {
    return flag->_count > 0 ? 1 : 0;
  } else {
    return -1;
  }
}

/*********************************************************************************************************************
 * optional_value
 *********************************************************************************************************************/

static int optional_value_takes() { return 1; }

static int optional_value_parse(struct optional *ctx, char const *const *argv, int argc) {
  if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
    // Fail if option already parsed
    return -1;
  }
  ctx->_values = &argv[0];
  ctx->_count = 1;
  return 1;
}

int optional_value_exists(struct optional *value) {
  if (value != NULL) {
    return value->_values != NULL;
  } else {
    return -1;
  }
}

char const *optional_value_get(struct optional *value) {
  if (value != NULL) {
    return *value->_values;
  } else {
    return NULL;
  }
}

/*********************************************************************************************************************
 * optional_list
 *********************************************************************************************************************/

static int optional_list_takes() { return 1; }

static int optional_list_parse(struct optional *ctx, char const *const *argv, int argc) {
  if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
    // Fail if option already parsed
    return -1;
  }
  ctx->_values = &argv[0];
  ctx->_count = argc;
  return argc;
}

int optional_list_exists(struct optional *list) {
  if (list != NULL) {
    return list->_values != NULL && list->_count > 0 ? 1 : 0;
  } else {
    return -1;
  }
}

int optional_list_count(struct optional *list) {
  if (list != NULL) {
    return list->_count;
  } else {
    return -1;
  }
}

char const *const *optional_list_get(struct optional *list) {
  if (list != NULL) {
    return list->_values;
  } else {
    return NULL;
  }
}

/*********************************************************************************************************************
 * required
 *********************************************************************************************************************/

struct required {
  char const *_name;
  char const *_desc;

  int _count;
  char const *const *_values;

  int (*takes)();
  int (*parse)(struct required *, char const *const *argv, int argc);
};

static void required_init(struct required *ctx, char const *const name, char const *const desc, int (*takes)(),
                          int (*parse)(struct required *, char const *const *, int)) {
  ctx->_name = name;
  ctx->_desc = desc;
  ctx->_count = 0;
  ctx->_values = NULL;
  ctx->takes = takes;
  ctx->parse = parse;
}

/*********************************************************************************************************************
 * required_value
 *********************************************************************************************************************/

static int required_value_takes() { return 1; }

static int required_value_parse(struct required *ctx, char const *const *argv, int argc) {
  if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
    // Fail if required already parsed
    return -1;
  }
  ctx->_values = &argv[0];
  ctx->_count = 1;
  return 1;
}

char const *const required_value_get(struct required *value) {
  if (value != NULL) {
    return *value->_values;
  } else {
    return NULL;
  }
}

/*********************************************************************************************************************
 * required_list
 *********************************************************************************************************************/

static int required_list_takes() { return 2; }

static int required_list_parse(struct required *ctx, char const *const *argv, int argc) {
  if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
    // Fail if required already parsed
    return -1;
  }
  ctx->_values = &argv[0];
  ctx->_count = argc;
  return 1;
}

int required_list_count(struct required *list) {
  if (list != NULL) {
    return list->_count;
  } else {
    return -1;
  }
}

char const *const *required_list_get(struct required *list) {
  if (list != NULL) {
    return list->_values;
  } else {
    return NULL;
  }
}

/*********************************************************************************************************************
 * command
 *********************************************************************************************************************/

struct optional_item;
struct required_item;
struct command_item;

struct command {
  char const *_name;
  char const *_desc;
  int _set;

  struct command *_parent;
  struct optional_item *_optionals;
  struct required_item *_requires;
  struct command_item *_commands;
};

static void command_init(struct command *ctx, char const *const name, char const *const desc, struct command *parent) {
  ctx->_name = name;
  ctx->_desc = desc;
  ctx->_set = 0;
  ctx->_parent = parent;
  ctx->_optionals = NULL;
  ctx->_requires = NULL;
  ctx->_commands = NULL;
}

int command_is_set(struct command *ctx) { return ctx->_set; }

/*********************************************************************************************************************
 * optional_item
 *********************************************************************************************************************/

struct optional_item {
  struct optional _optional;
  struct optional_item *_next;
};

static struct optional_item *optional_item_new(char const flag, char const *const l_flag, char const *const desc,
                                               int (*takes)(),
                                               int (*parse)(struct optional *, char const *const *, int)) {
  struct optional_item *ctx = malloc(sizeof(struct optional_item));
  if (ctx != NULL) {
    optional_init(&ctx->_optional, flag, l_flag, desc, takes, parse);
    ctx->_next = NULL;
  }
  return ctx;
}

static struct optional *command_add_optional_item(struct command *ctx, char const flag, char const *const l_flag,
                                                  char const *const desc, int (*takes)(),
                                                  int (*parse)(struct optional *, char const *const *, int)) {
  if (ctx == NULL) {
    return NULL;
  }

  struct optional_item *item = optional_item_new(flag, l_flag, desc, takes, parse);
  if (item != NULL) {
    if (ctx->_optionals == NULL) {
      ctx->_optionals = item;
    } else {
      struct optional_item *it = ctx->_optionals;
      while (it->_next != NULL) {
        it = it->_next;
      }
      it->_next = item;
    }
    return &item->_optional;
  } else {
    return NULL;
  }
}

/*********************************************************************************************************************
 * required_item
 *********************************************************************************************************************/

struct required_item {
  struct required _required;
  struct required_item *_next;
};

static struct required_item *required_item_new(char const *const name, char const *const desc, int (*takes)(),
                                               int (*parse)(struct required *, char const *const *, int)) {
  struct required_item *ctx = malloc(sizeof(struct required_item));
  if (ctx != NULL) {
    required_init(&ctx->_required, name, desc, takes, parse);
    ctx->_next = NULL;
  }
  return ctx;
}

static struct required *command_add_req_item(struct command *ctx, char const *const name, char const *const desc,
                                             int (*takes)(),
                                             int (*parse)(struct required *, char const *const *, int)) {
  if (ctx == NULL) {
    return NULL;
  }

  struct required_item *item = required_item_new(name, desc, takes, parse);
  if (item != NULL) {
    if (ctx->_requires == NULL) {
      ctx->_requires = item;
    } else {
      struct required_item *it = ctx->_requires;
      while (it->_next != NULL) {
        it = it->_next;
      }
      it->_next = item;
    }
    return &item->_required;
  } else {
    return NULL;
  }
}

/*********************************************************************************************************************
 * command_item
 *********************************************************************************************************************/

struct command_item {
  struct command _command;
  struct command_item *_next;
};

static void command_deinit(struct command *ctx) {
  ctx->_name = NULL;
  ctx->_desc = NULL;
  ctx->_parent = NULL;

  struct command_item *c = ctx->_commands;
  while (c != NULL) {
    ctx->_commands = c->_next;
    command_deinit(&c->_command);
    free(c);
    c = ctx->_commands;
  }

  struct optional_item *o = ctx->_optionals;
  while (o != NULL) {
    ctx->_optionals = o->_next;
    free(o);
    o = ctx->_optionals;
  }

  struct required_item *r = ctx->_requires;
  while (r != NULL) {
    ctx->_requires = r->_next;
    free(r);
    r = ctx->_requires;
  }
}

static struct command_item *command_item_new(char const *const name, char const *const desc, struct command *parent) {
  struct command_item *ctx = malloc(sizeof(struct command_item));
  if (ctx != NULL) {
    command_init(&ctx->_command, name, desc, parent);
    ctx->_next = NULL;
  }
  return ctx;
}

static struct command *command_add_command_item(struct command *ctx, char const *const name, char const *const desc) {
  if (ctx == NULL) {
    return NULL;
  }

  struct command_item *item = command_item_new(name, desc, ctx);
  if (item != NULL) {
    if (ctx->_commands == NULL) {
      ctx->_commands = item;
    } else {
      struct command_item *it = ctx->_commands;
      while (it->_next != NULL) {
        it = it->_next;
      }
      it->_next = item;
    }
    return &item->_command;
  } else {
    return NULL;
  }
}

/*********************************************************************************************************************
 * public interface for command
 *********************************************************************************************************************/

struct command *command_add_subcommand(struct command *ctx, char const *const name, char const *const desc) {
  return command_add_command_item(ctx, name, desc);
}

struct optional *command_add_opt_flag(struct command *ctx, char const flag, char const *const l_flag,
                                      char const *const desc) {
  return command_add_optional_item(ctx, flag, l_flag, desc, optional_flag_takes, optional_flag_parse);
}

struct optional *command_add_opt_value(struct command *ctx, char const flag, char const *const l_flag,
                                       char const *const desc) {
  return command_add_optional_item(ctx, flag, l_flag, desc, optional_value_takes, optional_value_parse);
}

struct optional *command_add_opt_list(struct command *ctx, char const flag, char const *const l_flag,
                                      char const *const desc) {
  return command_add_optional_item(ctx, flag, l_flag, desc, optional_list_takes, optional_list_parse);
}

struct required *command_add_req_value(struct command *ctx, char const *const name, char const *const desc) {
  return command_add_req_item(ctx, name, desc, required_value_takes, required_value_parse);
}

struct required *command_add_req_list(struct command *ctx, char const *const name, char const *const desc) {
  return command_add_req_item(ctx, name, desc, required_list_takes, required_list_parse);
}

/*********************************************************************************************************************
 * Print help message
 *********************************************************************************************************************/

static void command_show_help(struct command *ctx) {
  fprintf(stdout, "\n    Usage: ");

  // Print parent arguments to provide full commandline
  struct command *processed = NULL;
  while (processed != ctx->_parent) {
    struct command *c = ctx->_parent;
    while (c->_parent != processed) {
      c = c->_parent;
    }
    fprintf(stdout, "%s ", c->_name);
    processed = c;
  }

  fprintf(stdout, "%s ", ctx->_name);

  if (ctx->_optionals != NULL) {
    fprintf(stdout, "[OPTIONS] ");
  }
  if (ctx->_commands != NULL) {
    fprintf(stdout, "[COMMAND] ");
  }

  struct required_item *r = ctx->_requires;
  while (r != NULL) {
    fprintf(stdout, "%s ", r->_required._name);
    if (r->_required.takes() > 1) {
      fprintf(stdout, "[%s...] ", r->_required._name);
    }
    r = r->_next;
  }
  fprintf(stdout, "\n\n");

  // Format description, supports manual linebreaks but also adds linebreaks to keep format
  if (ctx->_desc != NULL) {
    char const *start = ctx->_desc;
    char const *end = ctx->_desc;

    while (*start != '\0') {
      char const *pos = end;
      while (*pos != '\0' && *pos != ' ' && *pos != '\n') {
        ++pos;
      }
      if (*pos == '\0') {
        fprintf(stdout, "    %s\n", start);
        break;
      } else {
        if (*pos == '\n' || (pos - start) > 80) {
          fprintf(stdout, "    %.*s\n", (int)(pos - start), start);
          start = pos + 1;
          end = pos + 1;
        } else {
          end = pos + 1;
        }
      }
    }
    fprintf(stdout, "\n");
  }

  // Display all supported options
  if (ctx->_optionals != NULL) {
    int width = 4;
    struct optional_item *opt = ctx->_optionals;
    while (opt != NULL) {
      int len = strlen(opt->_optional._long);
      if (len + 4 > width) {
        width = len + 4;
      }
      opt = opt->_next;
    }

    opt = ctx->_optionals;
    fprintf(stdout, "    Options:\n\n");
    while (opt != NULL) {
      fprintf(stdout, "        -%c, --%-*s%s\n", opt->_optional._short, width, opt->_optional._long,
              opt->_optional._desc);
      opt = opt->_next;
    }
    fprintf(stdout, "\n");
  }

  // Display all supported commands
  if (ctx->_commands != NULL) {
    int width = 4;
    struct command_item *cmd = ctx->_commands;
    while (cmd != NULL) {
      int len = strlen(cmd->_command._name);
      if (len + 4 > width) {
        width = len + 4;
      }
      cmd = cmd->_next;
    }

    cmd = ctx->_commands;
    fprintf(stdout, "    Commands:\n\n");
    while (cmd != NULL) {
      fprintf(stdout, "        %-*s%s\n", width, cmd->_command._name, cmd->_command._desc);
      cmd = cmd->_next;
    }
    fprintf(stdout, "\n");
  }

  // Display all required arguments
  if (ctx->_requires != NULL) {
    int width = 4;
    struct required_item *req = ctx->_requires;
    while (req != NULL) {
      int len = strlen(req->_required._name);
      if (len + 4 > width) {
        width = len + 4;
      }
      req = req->_next;
    }

    req = ctx->_requires;
    fprintf(stdout, "    Required:\n\n");
    while (req != NULL) {
      fprintf(stdout, "        %-*s%s\n", width, req->_required._name, req->_required._desc);
      req = req->_next;
    }
    fprintf(stdout, "\n");
  }
}

/*********************************************************************************************************************
 * Parsing utility
 *********************************************************************************************************************/

/*!
 * Find the next argument position that is option or command
 */
static int idx_of_next_opt(struct command *ctx, char const *const *argv, int argc, int start) {
  for (int i = start; i < argc; ++i) {
    if (*argv[i] == '-') {
      return i;
    } else {
      int arg_len = strlen(argv[i]);
      struct command_item *c = ctx->_commands;
      while (c != NULL) {
        int c_len = strlen(c->_command._name);
        if (c_len == arg_len && strcmp(argv[i], c->_command._name) == 0) {
          return i;
        }
        c = c->_next;
      }
    }
  }
  return argc;
}

/*!
 * Parses option, supports flag duplicates using `-v -v -v` or `-vvv`
 */
static int optional_parse(struct command *ctx, char const *const *argv, int argc, char const *const arg) {
  int used = -1;
  int is_short = arg[1] == '-' ? 0 : 1;

  if (is_short == 1) {
    // Parse e.g. `-v` and `-vvvv`
    int len = strlen(arg);
    for (int i = 1; i < len; ++i) {
      struct optional_item *opt = ctx->_optionals;
      while (opt != NULL) {
        if (opt->_optional._short == arg[i]) {
          break;
        };
        opt = opt->_next;
      }

      if (opt == NULL) {
        return -1;
      }

      used = (used == -1 ? 0 : used) + opt->_optional.parse(&opt->_optional, argv, argc);
    }
  } else {
    // Parse e.g. `--verbose`
    int len = strlen(&arg[2]);
    struct optional_item *opt = ctx->_optionals;
    while (opt != NULL) {
      int opt_len = strlen(opt->_optional._long);
      if (len == opt_len && strcmp(opt->_optional._long, &arg[2]) == 0) {
        break;
      };
      opt = opt->_next;
    }

    if (opt == NULL) {
      return -1;
    }

    used = opt->_optional.parse(&opt->_optional, argv, argc);
  }

  // Show help if parsing failed
  if (used == -1) {
    command_show_help(ctx);
    return -1;
  }

  return used + 1;
}

/*********************************************************************************************************************
 * Parsing argument for command
 *********************************************************************************************************************/

static int command_parse_args(struct command *ctx, char const *const *argv, int argc) {
  // Forbid multiple processing of same command
  if (ctx->_set != 0) {
    return -1;
  }

  ctx->_set = 1;
  int pos = 1;
  while (pos < argc) {
    int len = strlen(argv[pos]);
    int end = idx_of_next_opt(ctx, argv, argc, pos + 1);

    if ((len == 6 && strcmp(argv[pos], "--help") == 0) || (len == 2 && strcmp(argv[pos], "-h") == 0)) {
      // Show help if requested
      command_show_help(ctx);
      return -1;
    } else if (len > 1 && *argv[pos] == '-' && (len != 2 || argv[pos][1] != '-')) {
      // Support `--` to force continuation with required arguments
      int used = optional_parse(ctx, &argv[pos + 1], end - pos - 1, argv[pos]);
      if (used < 0) {
        return -1;
      }
      pos += used;
    } else if (pos < argc) {
      // Check if argument is command and if so, parse command
      struct command_item *c = NULL;
      if (argv[pos][0] != '-') {
        c = ctx->_commands;
        while (c != NULL) {
          int c_len = strlen(c->_command._name);
          if (len == c_len && strcmp(argv[pos], c->_command._name) == 0) {
            int used = command_parse_args(&c->_command, &argv[pos], argc - pos);
            if (used == -1) {
              return -1;
            }
            pos += used + 1;
            break;
          }
          c = c->_next;
        }
      } else if (len == 2 && argv[pos][1] == '-') {
        // Skip '--'
        pos += 1;
      }
      // Check for required arguments if arguments remaining and no subcommand was parsed
      if (pos < argc && c == NULL) {
        struct required_item *r = ctx->_requires;
        while (r != NULL) {
          if (pos >= argc) {
            return -1;
          }
          int used = r->_required.parse(&r->_required, &argv[pos], argc - pos);
          if (used == -1) {
            return -1;
          }
          pos += used;
          r = r->_next;
        }
        return pos;
      }
    } else {
      return -1;
    }
  }

  return ctx->_requires == NULL ? argc : -1;
}

/*********************************************************************************************************************
 * Parser
 *********************************************************************************************************************/

struct parser {
  struct command _internal;
};

struct parser *parser_init(char const *const name, char const *const desc) {
  struct parser *ctx = malloc(sizeof(struct parser));
  if (ctx != NULL) {
    command_init(&ctx->_internal, name, desc, NULL);
  }
  return ctx;
}

void parser_deinit(struct parser *ctx) {
  if (ctx == NULL) {
    return;
  }
  command_deinit(&ctx->_internal);
  free(ctx);
}

struct command *parser_add_command(struct parser *ctx, char const *const name, char const *const desc) {
  return command_add_command_item(&ctx->_internal, name, desc);
}

struct optional *parser_add_opt_flag(struct parser *ctx, char const flag, char const *const l_flag,
                                     char const *const desc) {
  return command_add_optional_item(&ctx->_internal, flag, l_flag, desc, optional_flag_takes, optional_flag_parse);
}

struct optional *parser_add_opt_value(struct parser *ctx, char const flag, char const *const l_flag,
                                      char const *const desc) {
  return command_add_optional_item(&ctx->_internal, flag, l_flag, desc, optional_value_takes, optional_value_parse);
}

struct optional *parser_add_opt_list(struct parser *ctx, char const flag, char const *const l_flag,
                                     char const *const desc) {
  return command_add_optional_item(&ctx->_internal, flag, l_flag, desc, optional_list_takes, optional_list_parse);
}

struct required *parser_add_req_value(struct parser *ctx, char const *const name, char const *const desc) {
  return command_add_req_item(&ctx->_internal, name, desc, required_value_takes, required_value_parse);
}

struct required *parser_add_req_list(struct parser *ctx, char const *const name, char const *const desc) {
  return command_add_req_item(&ctx->_internal, name, desc, required_list_takes, required_list_parse);
}

int parser_parse_args(struct parser *ctx, char const *const *argv, int argc) {
  return command_parse_args(&ctx->_internal, argv, argc) == -1 ? 1 : 0;
}

/*********************************************************************************************************************/
