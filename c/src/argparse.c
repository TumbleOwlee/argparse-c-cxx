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
 * struct flag
 *********************************************************************************************************************/

struct __attribute__((packed)) flag {
    char _short;
    unsigned int _count : 8;
    unsigned int _flags : 8;
    char const *_long;
    char const *_placeholder;
    char const *_desc;
    char const *const *_values;

    int (*takes)();
    int (*parse)(struct flag *ctx, char const *const *, int);
};

static void flag_init(struct flag *ctx, char const flag, char const *const l_flag, char const *const placeholder,
                      char const *const desc, unsigned int flags, int (*takes)(),
                      int (*parse)(struct flag *, char const *const *, int)) {
    ctx->_short = flag;
    ctx->_long = l_flag;
    ctx->_flags = flags;
    ctx->_placeholder = placeholder;
    ctx->_desc = desc;
    ctx->_count = 0;
    ctx->_values = NULL;
    ctx->takes = takes;
    ctx->parse = parse;
}

/*********************************************************************************************************************
 * flag
 *********************************************************************************************************************/

static int flag_takes() { return 0; }

static int flag_parse(struct flag *ctx, char const *const *argv, int argc) {
    ctx->_count += 1;
    return 0;
}

int flag_count(struct flag *flag) {
    if (flag != NULL) {
        return flag->_count;
    } else {
        return -1;
    }
}

int flag_set(struct flag *flag) {
    if (flag != NULL) {
        return flag->_count > 0 ? 1 : 0;
    } else {
        return -1;
    }
}

/*********************************************************************************************************************
 * flag_value
 *********************************************************************************************************************/

static int flag_value_takes() { return 1; }

static int flag_value_parse(struct flag *ctx, char const *const *argv, int argc) {
    if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
        // Fail if option already parsed
        return -1;
    }
    ctx->_values = &argv[0];
    ctx->_count = 1;
    return 1;
}

int flag_value_exists(struct flag *value) {
    if (value != NULL) {
        return value->_values != NULL;
    } else {
        return -1;
    }
}

char const *flag_value_get(struct flag *value) {
    if (value != NULL) {
        return *value->_values;
    } else {
        return NULL;
    }
}

/*********************************************************************************************************************
 * flag_list
 *********************************************************************************************************************/

static int flag_list_takes() { return 1; }

static int flag_list_parse(struct flag *ctx, char const *const *argv, int argc) {
    if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
        // Fail if option already parsed
        return -1;
    }
    ctx->_values = &argv[0];
    ctx->_count = argc;
    return argc;
}

int flag_list_exists(struct flag *list) {
    if (list != NULL) {
        return list->_values != NULL && list->_count > 0 ? 1 : 0;
    } else {
        return -1;
    }
}

int flag_list_count(struct flag *list) {
    if (list != NULL) {
        return list->_count;
    } else {
        return -1;
    }
}

char const *const *flag_list_get(struct flag *list) {
    if (list != NULL) {
        return list->_values;
    } else {
        return NULL;
    }
}

/*********************************************************************************************************************
 * struct arg
 *********************************************************************************************************************/

struct __attribute__((packed)) arg {
    char const *_name;
    char const *_desc;

    unsigned int _count : 8;
    char const *const *_values;

    int (*takes)();
    int (*parse)(struct arg *, char const *const *argv, int argc);
};

static void arg_init(struct arg *ctx, char const *const name, char const *const desc, int (*takes)(),
                     int (*parse)(struct arg *, char const *const *, int)) {
    ctx->_name = name;
    ctx->_desc = desc;
    ctx->_count = 0;
    ctx->_values = NULL;
    ctx->takes = takes;
    ctx->parse = parse;
}

/*********************************************************************************************************************
 * arg_value
 *********************************************************************************************************************/

static int arg_value_takes() { return 1; }

static int arg_value_parse(struct arg *ctx, char const *const *argv, int argc) {
    if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
        // Fail if required already parsed
        return -1;
    }
    ctx->_values = &argv[0];
    ctx->_count = 1;
    return 1;
}

char const *const arg_value_get(struct arg *value) {
    if (value != NULL) {
        return *value->_values;
    } else {
        return NULL;
    }
}

/*********************************************************************************************************************
 * arg_list
 *********************************************************************************************************************/

static int arg_list_takes() { return 2; }

static int arg_list_parse(struct arg *ctx, char const *const *argv, int argc) {
    if (argc < 1 || ctx->_values != NULL || ctx->_count != 0) {
        // Fail if required already parsed
        return -1;
    }
    ctx->_values = &argv[0];
    ctx->_count = argc;
    return 1;
}

int arg_list_count(struct arg *list) {
    if (list != NULL) {
        return list->_count;
    } else {
        return -1;
    }
}

char const *const *arg_list_get(struct arg *list) {
    if (list != NULL) {
        return list->_values;
    } else {
        return NULL;
    }
}

/*********************************************************************************************************************
 * command
 *********************************************************************************************************************/

struct flag_item;
struct arg_item;
struct command_item;

struct __attribute__((packed)) command {
    char const *_name;
    char const *_desc;
    char const *_footer;
    unsigned int _set : 1;

    struct command *_parent;
    struct flag_item *_optionals;
    struct arg_item *_requires;
    struct command_item *_commands;
};

static void command_init(struct command *ctx, char const *const name, char const *const desc, char const *const footer,
                         struct command *parent) {
    ctx->_name = name;
    ctx->_desc = desc;
    ctx->_footer = footer;
    ctx->_set = 0;
    ctx->_parent = parent;
    ctx->_optionals = NULL;
    ctx->_requires = NULL;
    ctx->_commands = NULL;
}

int command_is_set(struct command *ctx) { return ctx->_set; }

/*********************************************************************************************************************
 * flag_item
 *********************************************************************************************************************/

struct __attribute__((packed)) flag_item {
    struct flag _optional;
    struct flag_item *_next;
};

static struct flag_item *flag_item_new(char const flag, char const *const l_flag, char const *const placeholder,
                                       char const *const desc, unsigned int flags, int (*takes)(),
                                       int (*parse)(struct flag *, char const *const *, int)) {
    struct flag_item *ctx = malloc(sizeof(struct flag_item));
    if (ctx != NULL) {
        flag_init(&ctx->_optional, flag, l_flag, placeholder, desc, flags, takes, parse);
        ctx->_next = NULL;
    }
    return ctx;
}

static struct flag *command_add_flag_item(struct command *ctx, char const flag, char const *const l_flag,
                                          char const *const placeholder, char const *const desc, unsigned int flags,
                                          int (*takes)(), int (*parse)(struct flag *, char const *const *, int)) {
    if (ctx == NULL) {
        return NULL;
    }

    struct flag_item *item = flag_item_new(flag, l_flag, placeholder, desc, flags, takes, parse);
    if (item != NULL) {
        if (ctx->_optionals == NULL) {
            ctx->_optionals = item;
        } else {
            struct flag_item *it = ctx->_optionals;
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
 * arg_item
 *********************************************************************************************************************/

struct __attribute__((packed)) arg_item {
    struct arg _required;
    struct arg_item *_next;
};

static struct arg_item *arg_item_new(char const *const name, char const *const desc, int (*takes)(),
                                     int (*parse)(struct arg *, char const *const *, int)) {
    struct arg_item *ctx = malloc(sizeof(struct arg_item));
    if (ctx != NULL) {
        arg_init(&ctx->_required, name, desc, takes, parse);
        ctx->_next = NULL;
    }
    return ctx;
}

static struct arg *command_add_arg_item(struct command *ctx, char const *const name, char const *const desc,
                                        int (*takes)(), int (*parse)(struct arg *, char const *const *, int)) {
    if (ctx == NULL) {
        return NULL;
    }

    struct arg_item *item = arg_item_new(name, desc, takes, parse);
    if (item != NULL) {
        if (ctx->_requires == NULL) {
            ctx->_requires = item;
        } else {
            struct arg_item *it = ctx->_requires;
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

struct __attribute__((packed)) command_item {
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

    struct flag_item *o = ctx->_optionals;
    while (o != NULL) {
        ctx->_optionals = o->_next;
        free(o);
        o = ctx->_optionals;
    }

    struct arg_item *r = ctx->_requires;
    while (r != NULL) {
        ctx->_requires = r->_next;
        free(r);
        r = ctx->_requires;
    }
}

static struct command_item *command_item_new(char const *const name, char const *const desc, char const *const footer,
                                             struct command *parent) {
    struct command_item *ctx = malloc(sizeof(struct command_item));
    if (ctx != NULL) {
        command_init(&ctx->_command, name, desc, footer, parent);
        ctx->_next = NULL;
    }
    return ctx;
}

static struct command *command_add_command_item(struct command *ctx, char const *const name, char const *const desc,
                                                char const *const footer) {
    if (ctx == NULL) {
        return NULL;
    }

    struct command_item *item = command_item_new(name, desc, footer, ctx);
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

struct command *command_add_subcommand(struct command *ctx, char const *const name, char const *const desc,
                                       char const *const footer) {
    return command_add_command_item(ctx, name, desc, footer);
}

struct flag *command_add_flag(struct command *ctx, char const flag, char const *const l_flag, char const *const desc,
                              unsigned int flags) {
    return command_add_flag_item(ctx, flag, l_flag, NULL, desc, flags, flag_takes, flag_parse);
}

struct flag *command_add_flag_value(struct command *ctx, char const flag, char const *const l_flag,
                                    char const *const placeholder, char const *const desc, unsigned int flags) {
    return command_add_flag_item(ctx, flag, l_flag, placeholder, desc, flags, flag_value_takes, flag_value_parse);
}

struct flag *command_add_flag_list(struct command *ctx, char const flag, char const *const l_flag,
                                   char const *const placeholder, char const *const desc, unsigned int flags) {
    return command_add_flag_item(ctx, flag, l_flag, placeholder, desc, flags, flag_list_takes, flag_list_parse);
}

struct arg *command_add_arg_value(struct command *ctx, char const *const name, char const *const desc) {
    return command_add_arg_item(ctx, name, desc, arg_value_takes, arg_value_parse);
}

struct arg *command_add_arg_list(struct command *ctx, char const *const name, char const *const desc) {
    return command_add_arg_item(ctx, name, desc, arg_list_takes, arg_list_parse);
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

    struct arg_item *r = ctx->_requires;
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
        struct flag_item *opt = ctx->_optionals;
        while (opt != NULL) {
            int len = strlen(opt->_optional._long);
            if (opt->_optional._placeholder != NULL) {
                len += strlen(opt->_optional._placeholder);
            }
            if (len + 7 > width) {
                width = len + 7;
            }
            opt = opt->_next;
        }

        int printed = 0;
        opt = ctx->_optionals;
        while (opt != NULL) {
            if ((opt->_optional._flags & SET_REQUIRED) == SET_REQUIRED) {
                if (printed == 0) {
                    fprintf(stdout, "    Required flags:\n\n");
                    printed = 1;
                }
                if (opt->_optional._placeholder == NULL) {
                    fprintf(stdout, "        -%c, --%-*s%s\n", opt->_optional._short, width, opt->_optional._long,
                            opt->_optional._desc);
                } else {
                    fprintf(stdout, "        -%c, --%s <%s>%-*s%s \n", opt->_optional._short, opt->_optional._long,
                            opt->_optional._placeholder,
                            (int)(width - strlen(opt->_optional._long) - strlen(opt->_optional._placeholder)) - 3, "",
                            opt->_optional._desc);
                }
            }
            opt = opt->_next;
        }
        if (printed == 1) {
            fprintf(stdout, "\n");
        }

        printed = 0;
        opt = ctx->_optionals;
        while (opt != NULL) {
            if ((opt->_optional._flags & SET_REQUIRED) != SET_REQUIRED) {
                if (printed == 0) {
                    fprintf(stdout, "    Optional flags:\n\n");
                    printed = 1;
                }
                if (opt->_optional._placeholder == NULL) {
                    fprintf(stdout, "        -%c, --%-*s%s\n", opt->_optional._short, width, opt->_optional._long,
                            opt->_optional._desc);
                } else {
                    fprintf(stdout, "        -%c, --%s <%s>%-*s%s \n", opt->_optional._short, opt->_optional._long,
                            opt->_optional._placeholder,
                            (int)(width - strlen(opt->_optional._long) - strlen(opt->_optional._placeholder)) - 3, "",
                            opt->_optional._desc);
                }
            }
            opt = opt->_next;
        }
        if (printed == 1) {
            fprintf(stdout, "\n");
        }
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
        struct arg_item *req = ctx->_requires;
        while (req != NULL) {
            int len = strlen(req->_required._name);
            if (len + 4 > width) {
                width = len + 4;
            }
            req = req->_next;
        }

        req = ctx->_requires;
        fprintf(stdout, "    Required arguments:\n\n");
        while (req != NULL) {
            fprintf(stdout, "        %-*s%s\n", width, req->_required._name, req->_required._desc);
            req = req->_next;
        }
        fprintf(stdout, "\n");
    }

    // Format footer
    if (ctx->_footer != NULL) {
        char const *start = ctx->_footer;
        char const *end = ctx->_footer;

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
static int parse_flag(struct command *ctx, char const *const *argv, int argc, char const *const arg) {
    int used = -1;
    int is_short = arg[1] == '-' ? 0 : 1;

    if (is_short == 1) {
        // Parse e.g. `-v` and `-vvvv`
        int len = strlen(arg);
        for (int i = 1; i < len; ++i) {
            struct flag_item *opt = ctx->_optionals;
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
        struct flag_item *opt = ctx->_optionals;
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

static int command_check_if_required(struct flag_item *o) {
    while (o != NULL) {
        if ((o->_optional._flags & SET_REQUIRED) == SET_REQUIRED) {
            int takes = o->_optional.takes();
            int _count = o->_optional._count;
            if (takes > _count) {
                if (o->_optional.takes() > 1) {
                    fprintf(stderr, "Missing option: -%c, --%s <%s...>\n", o->_optional._short, o->_optional._long,
                            o->_optional._placeholder);
                } else {
                    fprintf(stderr, "Missing option: -%c, --%s <%s> \n", o->_optional._short, o->_optional._long,
                            o->_optional._placeholder);
                }
                return -1;
            }
        }
        o = o->_next;
    }
    return 0;
}

static int command_parse_args(struct command *ctx, char const *const *argv, int argc) {
    // Forbid multiple processing of same command
    if (ctx->_set != 0) {
        return -1;
    }
    struct arg_item *r = NULL;
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
            int used = parse_flag(ctx, &argv[pos + 1], end - pos - 1, argv[pos]);
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
                        pos += used;
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
                r = ctx->_requires;
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

                if (command_check_if_required(ctx->_optionals) != 0) {
                    return -1;
                } else {
                    return pos;
                }
            }
        } else {
            return -1;
        }
    }

    if (command_check_if_required(ctx->_optionals) != 0) {
        return -1;
    } else {
        return ctx->_requires == NULL ? pos : -1;
    }
}

/*********************************************************************************************************************
 * Parser
 *********************************************************************************************************************/

struct parser {
    struct command _internal;
};

struct parser *parser_init(char const *const name, char const *const desc, char const *const footer) {
    struct parser *ctx = malloc(sizeof(struct parser));
    if (ctx != NULL) {
        command_init(&ctx->_internal, name, desc, footer, NULL);
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

struct command *parser_add_command(struct parser *ctx, char const *const name, char const *const desc,
                                   char const *const footer) {
    return command_add_command_item(&ctx->_internal, name, desc, footer);
}

struct flag *parser_add_flag(struct parser *ctx, char const flag, char const *const l_flag, char const *const desc) {
    return command_add_flag_item(&ctx->_internal, flag, l_flag, NULL, desc, SET_NONE, flag_takes, flag_parse);
}

struct flag *parser_add_flag_value(struct parser *ctx, char const flag, char const *const l_flag,
                                   const char *const placeholder, char const *const desc, unsigned int flags) {
    return command_add_flag_item(&ctx->_internal, flag, l_flag, placeholder, desc, flags, flag_value_takes,
                                 flag_value_parse);
}

struct flag *parser_add_flag_list(struct parser *ctx, char const flag, char const *const l_flag,
                                  const char *const placeholder, char const *const desc, unsigned int flags) {
    return command_add_flag_item(&ctx->_internal, flag, l_flag, placeholder, desc, flags, flag_list_takes,
                                 flag_list_parse);
}

struct arg *parser_add_arg_value(struct parser *ctx, char const *const name, char const *const desc) {
    return command_add_arg_item(&ctx->_internal, name, desc, arg_value_takes, arg_value_parse);
}

struct arg *parser_add_arg_list(struct parser *ctx, char const *const name, char const *const desc) {
    return command_add_arg_item(&ctx->_internal, name, desc, arg_list_takes, arg_list_parse);
}

int parser_parse_args(struct parser *ctx, char const *const *argv, int argc) {
    return command_parse_args(&ctx->_internal, argv, argc) == argc ? 0 : 1;
}

/*********************************************************************************************************************/
