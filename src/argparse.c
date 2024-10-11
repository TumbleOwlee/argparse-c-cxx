/*********************************************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argparse.h"

/******************************************************************
 * optional
 ******************************************************************/

struct optional {
  char _short;
  char const *_long;
  char const *_desc;

  int _count;
  char const *_values;

  int (*takes)();
  int (*parse)(struct optional *ctx, char const *const *, int);
};

void optional_init(struct optional *ctx, char const flag, char const *const l_flag, char const *const desc,
                   int (*takes)(), int (*parse)(struct optional *, char const *const *, int)) {
  ctx->_short = flag;
  ctx->_long = l_flag;
  ctx->_desc = desc;
  ctx->_count = 0;
  ctx->_values = NULL;
  ctx->takes = takes;
  ctx->parse = parse;
}

/******************************************************************
 * optional_flag
 ******************************************************************/

int optional_flag_takes() { return 0; }

int optional_flag_parse(struct optional *ctx, char const *const *argv, int argc) {
  ctx->_count += 1;
  return 0;
}

int optional_flag_get_count(struct optional *flag) {
  if (flag != NULL) {
    return flag->_count;
  } else {
    return -1;
  }
}

int optional_flag_is_set(struct optional *flag) {
  if (flag != NULL) {
    return flag->_count > 0 ? 1 : 0;
  } else {
    return -1;
  }
}

/******************************************************************
 * optional_value
 ******************************************************************/

int optional_value_takes() { return 1; }

int optional_value_parse(struct optional *ctx, char const *const *argv, int argc) {
  if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
    return -1;
  }
  ctx->_values = argv[0];
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

char const *const optional_value_get(struct optional *value) {
  if (value != NULL) {
    return value->_values;
  } else {
    return NULL;
  }
}

/******************************************************************
 * optional_list
 ******************************************************************/

int optional_list_takes() { return 1; }

int optional_list_parse(struct optional *ctx, char const *const *argv, int argc) {
  if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
    return -1;
  }
  ctx->_values = argv[0];
  ctx->_count = argc;
  return 1;
}

int optional_list_exists(struct optional *value) {
  if (value != NULL) {
    return value->_values != NULL && value->_count > 0 ? 1 : 0;
  } else {
    return -1;
  }
}

int optional_list_get_count(struct optional *value) {
  if (value != NULL) {
    return value->_count;
  } else {
    return -1;
  }
}

char const *const optional_list_get_values(struct optional *value) {
  if (value != NULL) {
    return value->_values;
  } else {
    return NULL;
  }
}

/******************************************************************
 * required
 ******************************************************************/

struct required {
  char const *_name;
  char const *_desc;

  int _count;
  char const *_values;

  int (*takes)();
  int (*parse)(struct required *, char const *const *argv, int argc);
};

void required_init(struct required *ctx, char const *const name, char const *const desc, int (*takes)(),
                   int (*parse)(struct required *, char const *const *, int)) {
  ctx->_name = name;
  ctx->_desc = desc;
  ctx->_count = 0;
  ctx->_values = NULL;
  ctx->takes = takes;
  ctx->parse = parse;
}

/******************************************************************
 * required_value
 ******************************************************************/

int required_value_takes() { return 1; }

int required_value_parse(struct required *ctx, char const *const *argv, int argc) {
  if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
    return -1;
  }
  ctx->_values = argv[0];
  ctx->_count = 1;
  return 1;
}

int required_value_exists(struct required *value) {
  if (value != NULL) {
    return value->_values != NULL;
  } else {
    return -1;
  }
}

char const *const required_value_get(struct required *value) {
  if (value != NULL) {
    return value->_values;
  } else {
    return NULL;
  }
}

/******************************************************************
 * required_list
 ******************************************************************/

int required_list_takes() { return 2; }

int required_list_parse(struct required *ctx, char const *const *argv, int argc) {
  if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
    return -1;
  }
  ctx->_values = argv[0];
  ctx->_count = argc;
  return 1;
}

int required_list_exists(struct required *value) {
  if (value != NULL) {
    return value->_values != NULL && value->_count > 0 ? 1 : 0;
  } else {
    return -1;
  }
}

int required_list_get_count(struct required *value) {
  if (value != NULL) {
    return value->_count;
  } else {
    return -1;
  }
}

char const *const required_list_get_values(struct required *value) {
  if (value != NULL) {
    return value->_values;
  } else {
    return NULL;
  }
}

/******************************************************************
 * command
 ******************************************************************/

struct optional_item;
struct required_item;
struct command_item;

struct command {
  char const *_name;
  char const *_desc;

  struct command *_parent;
  struct optional_item *_optionals;
  struct required_item *_requires;
  struct command_item *_commands;
};

/*PRIVATE*/ void command_init(struct command *ctx, char const *const name, char const *const desc,
                              struct command *parent) {
  ctx->_name = name;
  ctx->_desc = desc;
  ctx->_parent = parent;
  ctx->_optionals = NULL;
  ctx->_requires = NULL;
  ctx->_commands = NULL;
}

struct optional_item {
  struct optional _optional;
  struct optional_item *_next;
};

/*PRIVATE*/ struct optional_item *optional_item_new(char const flag, char const *const l_flag, char const *const desc,
                                                    int (*takes)(),
                                                    int (*parse)(struct optional *, char const *const *, int)) {
  struct optional_item *ctx = malloc(sizeof(struct optional_item));
  if (ctx != NULL) {
    optional_init(&ctx->_optional, flag, l_flag, desc, takes, parse);
    ctx->_next = NULL;
  }
  return ctx;
}

/*PRIVATE*/ struct optional *command_add_optional_item(struct command *ctx, char const flag, char const *const l_flag,
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

struct required_item {
  struct required _required;
  struct required_item *_next;
};

/*PRIVATE*/ struct required_item *required_item_new(char const *const name, char const *const desc, int (*takes)(),
                                                    int (*parse)(struct required *, char const *const *, int)) {
  struct required_item *ctx = malloc(sizeof(struct required_item));
  if (ctx != NULL) {
    required_init(&ctx->_required, name, desc, takes, parse);
    ctx->_next = NULL;
  }
  return ctx;
}

/*PRIVATE*/ struct required *command_add_required_item(struct command *ctx, char const *const name,
                                                       char const *const desc, int (*takes)(),
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

struct command_item {
  struct command _command;
  struct command_item *_next;
};

/*PRIVATE*/ void command_deinit(struct command *ctx) {
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

/*PRIVATE*/ struct command_item *command_item_new(char const *const name, char const *const desc,
                                                  struct command *parent) {
  struct command_item *ctx = malloc(sizeof(struct command_item));
  if (ctx != NULL) {
    command_init(&ctx->_command, name, desc, parent);
    ctx->_next = NULL;
  }
  return ctx;
}

/*PRIVATE*/ struct command *command_add_command_item(struct command *ctx, char const *const name,
                                                     char const *const desc) {
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

/*PUBLIC*/ struct command *command_add_sub_command(struct command *ctx, char const *const name,
                                                   char const *const desc) {
  return command_add_command_item(ctx, name, desc);
}

/*PUBLIC*/ struct optional *command_add_opt_flag(struct command *ctx, char const flag, char const *const l_flag,
                                                 char const *const desc) {
  return command_add_optional_item(ctx, flag, l_flag, desc, optional_flag_takes, optional_flag_parse);
}

/*PUBLIC*/ struct optional *command_add_opt_value(struct command *ctx, char const flag, char const *const l_flag,
                                                  char const *const desc) {
  return command_add_optional_item(ctx, flag, l_flag, desc, optional_value_takes, optional_value_parse);
}

/*PUBLIC*/ struct optional *command_add_opt_list(struct command *ctx, char const flag, char const *const l_flag,
                                                 char const *const desc) {
  return command_add_optional_item(ctx, flag, l_flag, desc, optional_list_takes, optional_list_parse);
}

/*PUBLIC*/ struct required *command_add_required_value(struct command *ctx, char const *const name,
                                                       char const *const desc) {
  return command_add_required_item(ctx, name, desc, required_value_takes, required_value_parse);
}

/*PUBLIC*/ struct required *command_add_required_list(struct command *ctx, char const *const name,
                                                      char const *const desc) {
  return command_add_required_item(ctx, name, desc, required_list_takes, required_list_parse);
}

/*PRIVATE*/ void command_print_help(struct command *ctx) {
  fprintf(stdout, "\n    Usage: %s ", ctx->_name);

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

/*PRIVATE*/ int command_parse_args(struct command *ctx, char const *const *argv, int argc) { return 0; }

struct parser {
  struct command _internal;
};

/*PUBLIC*/ struct parser *parser_init(char const *const name, char const *const desc) {
  struct parser *ctx = malloc(sizeof(struct parser));
  if (ctx != NULL) {
    command_init(&ctx->_internal, name, desc, NULL);
  }
  return ctx;
}

/*PUBLIC*/ void parser_deinit(struct parser *ctx) {
  if (ctx == NULL) {
    return;
  }
  command_deinit(&ctx->_internal);
  free(ctx);
}

/*PUBLIC*/ struct command *parser_add_command(struct parser *ctx, char const *const name, char const *const desc) {
  return command_add_command_item(&ctx->_internal, name, desc);
}

/*PUBLIC*/ struct optional *parser_add_opt_flag(struct parser *ctx, char const flag, char const *const l_flag,
                                                char const *const desc) {
  return command_add_optional_item(&ctx->_internal, flag, l_flag, desc, optional_flag_takes, optional_flag_parse);
}

/*PUBLIC*/ struct optional *parser_add_opt_value(struct parser *ctx, char const flag, char const *const l_flag,
                                                 char const *const desc) {
  return command_add_optional_item(&ctx->_internal, flag, l_flag, desc, optional_value_takes, optional_value_parse);
}

/*PUBLIC*/ struct optional *parser_add_opt_list(struct parser *ctx, char const flag, char const *const l_flag,
                                                char const *const desc) {
  return command_add_optional_item(&ctx->_internal, flag, l_flag, desc, optional_list_takes, optional_list_parse);
}

/*PUBLIC*/ struct required *parser_add_required_value(struct parser *ctx, char const *const name,
                                                      char const *const desc) {
  return command_add_required_item(&ctx->_internal, name, desc, required_value_takes, required_value_parse);
}

/*PUBLIC*/ struct required *parser_add_required_list(struct parser *ctx, char const *const name,
                                                     char const *const desc) {
  return command_add_required_item(&ctx->_internal, name, desc, required_list_takes, required_list_parse);
}

/*PUBLIC*/ int parser_parse_args(struct parser *ctx, char const *const *argv, int argc) {
  return command_parse_args(&ctx->_internal, argv, argc) == -1 ? 1 : 0;
}

/*PRIVATE*/ void parser_print_help(struct parser *ctx) { command_print_help(&ctx->_internal); }

/*********************************************************************************************************************/
