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

#ifndef __ARGPARSE_C__
#define __ARGPARSE_C__

/*!
 * @brief Optional parameter type, can be either a simple flag, a optional value, or list of optional values
 */
struct optional;

/*!
 * @brief Returns how many times the flag was provided
 *
 * @param flag The optional flags structure
 * @return The number of occurrences of the flag
 */
int optional_flag_count(struct optional *flag);

/*!
 * @brief Returns whether the flag is set (1) or unset (0)
 *
 * @param flag    The optional flags structure
 * @return int    1 if set, else 0
 */
int optional_flag_set(struct optional *flag);

/*!
 * @brief Returns whether a value was provided
 *
 * @param value   The optional value structure
 * @return int    1 if a value is provided, else 0
 */
int optional_value_exists(struct optional *value);

/*!
 * @brief Returns the pointer to the value in argv
 *
 * @param value          The optional value structure
 * @return char const*   Null terminated string value
 */
char const *optional_value_get(struct optional *value);

/*!
 * @brief Returns whether at least one value was provided
 *
 * @param list    The optional list structure
 * @return int    1 if a value is provided, else 0
 */
int optional_list_exists(struct optional *list);

/*!
 * @brief Returns the count of provided values
 *
 * @param list    The optional list structure
 * @return int    The count of available values
 */
int optional_list_count(struct optional *list);

/*!
 * @brief Returns the pointer to the array of values
 *
 * @param list                    The optional list structure
 * @return char const*  const*    The pointer to the array of values
 */
char const *const *optional_list_get(struct optional *list);

/*!
 * @brief Required parameter type, can be either required value, or list of required values
 */
struct required;

/*!
 * @brief Returns the pointer to the value
 *
 * @param value                   The required value structure
 * @return char const*  const*    The pointer to the value
 */
char const *const required_value_get(struct required *value);

/*!
 * @brief Returns the count of provided values
 *
 * @param list    The required list structure
 * @return int    The count of available values
 */
int required_list_count(struct required *list);

/*!
 * @brief Returns the pointer to the array of values
 *
 * @param list                    The required list structure
 * @return char const*  const*    The pointer to the array of values
 */
char const *const *required_list_get(struct required *list);

/*!
 * @brief Command type, utilized for parser and subcommands
 */
struct command;

/*!
 * @brief Return whether the command is used
 *
 * @param ctx    The command structure
 * @return int   1 if command is used, else 0
 */
int command_is_set(struct command *ctx);

/*!
 * @brief Add new command as subcommand
 *
 * @param ctx                 The parent command structure
 * @param name                Name of the subcommand
 * @param desc                Description of the subcommand
 * @return struct command*    Reference to the newly added subcommand
 */
struct command *command_add_subcommand(struct command *ctx, char const *const name, char const *const desc);

/*!
 * @brief Add new optional flag to command
 *
 * @param ctx                 The parent command structure
 * @param flag                The short version of the flag
 * @param l_flag              The long version of the flag
 * @param desc                Description of the flag
 * @return struct optional*   Reference to the newly added optional flag
 */
struct optional *command_add_opt_flag(struct command *ctx, char const flag, char const *const l_flag,
                                      char const *const desc);

/*!
 * @brief Add new optional value to command
 *
 * @param ctx                 The parent command structure
 * @param flag                The short version of the value flag
 * @param l_flag              The long version of the value flag
 * @param desc                Description of the value flag
 * @return struct optional*   Reference to the newly added optional value
 */
struct optional *command_add_opt_value(struct command *ctx, char const flag, char const *const l_flag,
                                       char const *const desc);

/*!
 * @brief Add new optional list of values to command
 *
 * @param ctx                 The parent command structure
 * @param flag                The short version of the list of values flag
 * @param l_flag              The long version of the list of values flag
 * @param desc                Description of the list of values flag
 * @return struct optional*   Reference to the newly added optional list of values
 */
struct optional *command_add_opt_list(struct command *ctx, char const flag, char const *const l_flag,
                                      char const *const desc);

/*!
 * @brief Add new required value to command
 *
 * @param ctx                 The parent command structure
 * @param flag                The short version of the value flag
 * @param l_flag              The long version of the value flag
 * @param desc                Description of the value flag
 * @return struct required*   Reference to the newly added required value
 */
struct required *command_add_req_value(struct command *ctx, char const *const name, char const *const desc);

/*!
 * @brief Add new required list of values to command
 *
 * @param ctx                 The parent command structure
 * @param flag                The short version of the list of values flag
 * @param l_flag              The long version of the list of values flag
 * @param desc                Description of the list of values flag
 * @return struct required*   Reference to the newly added required list of values
 */
struct required *command_add_req_list(struct command *ctx, char const *const name, char const *const desc);

/*!
 * @brief Parser structure holding all optional/required values and commands
 */
struct parser;

/*!
 * @brief Initializes a new parser structure, call parser_deinit(..) to free it
 *
 * @param name               Name of the application (most likely argv[0])
 * @param desc               Description of the application, custom linebreaks supported
 * @return struct parser*    Reference to the newly allocated parser structure
 */
struct parser *parser_init(char const *const name, char const *const desc);

/*!
 * @brief Deinitializes the parser structure, freeing all optional/required parameters and subcommands
 *
 * @param ctx    The parser context
 */
void parser_deinit(struct parser *ctx);

/*!
 * @brief Adds a new command to the parser
 *
 * @param ctx                  The parser context
 * @param name                 Name of the command
 * @param desc                 Description of the command
 * @return struct command*     Reference to the created command
 */
struct command *parser_add_command(struct parser *ctx, char const *const name, char const *const desc);

/*!
 * @brief Adds a new optional flag to the parser
 *
 * @param ctx                  The parser context
 * @param flag                 Short flag of the optional
 * @param l_flag               Long flag of the optional
 * @param desc                 Description of the optional flag
 * @return struct optional*    Reference to the created optional flag
 */
struct optional *parser_add_opt_flag(struct parser *ctx, char const flag, char const *const l_flag,
                                     char const *const desc);

/*!
 * @brief Adds a new optional value to the parser
 *
 * @param ctx                  The parser context
 * @param flag                 Short flag of the optional value
 * @param l_flag               Long flag of the optional value
 * @param desc                 Description of the optional value
 * @return struct optional*    Reference to the created optional value
 */
struct optional *parser_add_opt_value(struct parser *ctx, char const flag, char const *const l_flag,
                                      char const *const desc);

/*!
 * @brief Adds a new optional value list to the parser
 *
 * @param ctx                  The parser context
 * @param flag                 Short flag of the optional value list
 * @param l_flag               Long flag of the optional value list
 * @param desc                 Description of the optional value list
 * @return struct optional*    Reference to the created optional value list
 */
struct optional *parser_add_opt_list(struct parser *ctx, char const flag, char const *const l_flag,
                                     char const *const desc);

/*!
 * @brief Adds a new required value to the parser
 *
 * @param ctx                  The parser context
 * @param name                 Name of the required value
 * @param desc                 Description of the required value
 * @return struct required*    Reference to the created optional value
 */
struct required *parser_add_req_value(struct parser *ctx, char const *const name, char const *const desc);

/*!
 * @brief Adds a new required value list to the parser
 *
 * @param ctx                  The parser context
 * @param name                 Name of the required value list
 * @param desc                 Description of the required value list
 * @return struct required*    Reference to the created optional value list
 */
struct required *parser_add_req_list(struct parser *ctx, char const *const name, char const *const desc);

/*!
 * @brief Parsing of the given arguments
 *
 * @param ctx    The context containing the supported argument definitions
 * @param argv   The array of commandline arguments
 * @param argc   Number of commandline arguments provided
 * @return int   1 on success, 0 on failure.
 */
int parser_parse_args(struct parser *ctx, char const *const *argv, int argc);

/*!
 * @brief See parser_init(..)
 */
#define parser_new(var, name, desc) struct parser *var = parser_init(name, desc)

/*!
 * @brief See parser_add_opt_flag(..)
 */
#define add_opt_flag(parser, var, flag, l_flag, desc)                                                                  \
  struct optional *var = parser_add_opt_flag(parser, flag, l_flag, desc)

/*!
 * @brief See parser_add_opt_value(..)
 */
#define add_opt_value(parser, var, flag, l_flag, desc)                                                                 \
  struct optional *var = parser_add_opt_value(parser, flag, l_flag, desc)

/*!
 * @brief See parser_add_opt_list(..)
 */
#define add_opt_list(parser, var, flag, l_flag, desc)                                                                  \
  struct optional *var = parser_add_opt_list(parser, flag, l_flag, desc)

/*!
 * @brief See parser_add_req_value(..)
 */
#define add_req_value(parser, var, name, desc) struct required *var = parser_add_req_value(parser, name, desc)

/*!
 * @brief See parser_add_req_list(..)
 */
#define add_req_list(parser, var, name, desc) struct required *var = parser_add_req_list(parser, name, desc)

/*!
 * @brief See parser_add_command(..)
 */
#define add_command(parser, var, name, desc) struct command *var = parser_add_command(parser, name, desc)

/*!
 * @brief See command_add_opt_flag(..)
 */
#define cmd_add_opt_flag(cmd, var, flag, l_flag, desc)                                                                 \
  struct optional *var = command_add_opt_flag(cmd, flag, l_flag, desc)

/*!
 * @brief See command_add_opt_value(..)
 */
#define cmd_add_opt_value(cmd, var, flag, l_flag, desc)                                                                \
  struct optional *var = cmd_add_opt_value(cmd, flag, l_flag, desc)

/*!
 * @brief See command_add_opt_list(..)
 */
#define cmd_add_opt_list(cmd, var, flag, l_flag, desc)                                                                 \
  struct optional *var = command_add_opt_list(cmd, flag, l_flag, desc)

/*!
 * @brief See command_add_req_value(..)
 */
#define cmd_add_req_value(cmd, var, name, desc) struct required *var = command_add_req_value(cmd, name, desc)

/*!
 * @brief See command_add_req_list(..)
 */
#define cmd_add_req_list(cmd, var, name, desc) struct required *var = command_add_req_list(cmd, name, desc)

/*!
 * @brief See command_add_command(..)
 */
#define cmd_add_subcommand(cmd, var, name, desc) struct command *var = command_add_subcommand(cmd, name, desc)

#endif // __ARGPARSE_C__
