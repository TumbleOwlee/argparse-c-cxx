struct optional;

int optional_flag_get_count(struct optional *flag);
int optional_flag_is_set(struct optional *flag);

int optional_value_exists(struct optional *value);
char const *const optional_value_get_value(struct optional *value);

int optional_list_exists(struct optional *value);
int optional_list_get_count(struct optional *value);
char const *const optional_list_get_values(struct optional *value);

struct required;

int required_value_exists(struct required *value);
char const *const required_value_get(struct required *value);

int required_list_exists(struct required *value);
int required_list_get_count(struct required *value);
char const *const required_list_get_values(struct required *value);

struct command;

struct command *command_add_sub_command(struct command *ctx, char const *const name, char const *const desc);

struct optional *command_add_opt_flag(struct command *ctx, char const flag, char const *const l_flag,
                                      char const *const desc);

struct optional *command_add_opt_value(struct command *ctx, char const flag, char const *const l_flag,
                                       char const *const desc);

struct optional *command_add_opt_list(struct command *ctx, char const flag, char const *const l_flag,
                                      char const *const desc);

struct required *command_add_required_value(struct command *ctx, char const *const name, char const *const desc);

struct required *command_add_required_list(struct command *ctx, char const *const name, char const *const desc);

struct parser;

struct parser *parser_init(char const *const name, char const *const desc);

void parser_deinit(struct parser *ctx);

struct command *parser_add_command(struct parser *ctx, char const *const name, char const *const desc);

struct optional *parser_add_opt_flag(struct parser *ctx, char const flag, char const *const l_flag,
                                     char const *const desc);

struct optional *parser_add_opt_value(struct parser *ctx, char const flag, char const *const l_flag,
                                      char const *const desc);

struct optional *parser_add_opt_list(struct parser *ctx, char const flag, char const *const l_flag,
                                     char const *const desc);

struct required *parser_add_required_value(struct parser *ctx, char const *const name, char const *const desc);

struct required *parser_add_required_list(struct parser *ctx, char const *const name, char const *const desc);

int parser_parse_args(struct parser *ctx, char const *const *argv, int argc);

/*PRIVATE*/ void parser_print_help(struct parser *ctx);
