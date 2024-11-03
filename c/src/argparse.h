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

#ifdef __cplusplus
extern C {
#endif

    enum settings { SET_NONE = 0, SET_REQUIRED = 1 };

    /*!
     * @brief Optional parameter type, can be either a simple flag, a optional value, or list of optional values
     */
    struct flag;

    /*!
     * @brief Returns how many times the flag was provided
     *
     * @param flag The optional flags structure
     * @return The number of occurrences of the flag
     */
    int flag_count(struct flag * flag);

    /*!
     * @brief Returns whether the flag is set (1) or unset (0)
     *
     * @param flag    The optional flags structure
     * @return int    1 if set, else 0
     */
    int flag_set(struct flag * flag);

    /*!
     * @brief Returns whether a value was provided
     *
     * @param value   The optional value structure
     * @return int    1 if a value is provided, else 0
     */
    int flag_value_exists(struct flag * value);

    /*!
     * @brief Returns the pointer to the value in argv
     *
     * @param value          The optional value structure
     * @return char const*   Null terminated string value
     */
    char const *flag_value_get(struct flag * value);

    /*!
     * @brief Returns whether at least one value was provided
     *
     * @param list    The optional list structure
     * @return int    1 if a value is provided, else 0
     */
    int flag_list_exists(struct flag * list);

    /*!
     * @brief Returns the count of provided values
     *
     * @param list    The optional list structure
     * @return int    The count of available values
     */
    int flag_list_count(struct flag * list);

    /*!
     * @brief Returns the pointer to the array of values
     *
     * @param list                    The optional list structure
     * @return char const*  const*    The pointer to the array of values
     */
    char const *const *flag_list_get(struct flag * list);

    /*!
     * @brief arg parameter type, can be either arg value, or list of arg values
     */
    struct arg;

    /*!
     * @brief Returns the pointer to the value
     *
     * @param value                   The arg value structure
     * @return char const*  const*    The pointer to the value
     */
    char const *const arg_value_get(struct arg * value);

    /*!
     * @brief Returns the count of provided values
     *
     * @param list    The arg list structure
     * @return int    The count of available values
     */
    int arg_list_count(struct arg * list);

    /*!
     * @brief Returns the pointer to the array of values
     *
     * @param list                    The arg list structure
     * @return char const*  const*    The pointer to the array of values
     */
    char const *const *arg_list_get(struct arg * list);

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
    int command_is_set(struct command * ctx);

    /*!
     * @brief Add new command as subcommand
     *
     * @param ctx                 The parent command structure
     * @param name                Name of the subcommand
     * @param desc                Description of the subcommand
     * @param footer              Optional footer text
     * @return struct command*    Reference to the newly added subcommand
     */
    struct command *command_add_subcommand(struct command * ctx, char const *const name, char const *const desc,
                                           char const *const footer);

    /*!
     * @brief Add new optional flag to command
     *
     * @param ctx                 The parent command structure
     * @param flag                The short version of the flag
     * @param l_flag              The long version of the flag
     * @param desc                Description of the flag
     * @param flags                1 if flag is arg, else 0
     * @return struct flag*   Reference to the newly added optional flag
     */
    struct flag *command_add_flag(struct command * ctx, char const flag, char const *const l_flag,
                                  char const *const desc, unsigned int flags);

    /*!
     * @brief Add new optional value to command
     *
     * @param ctx                 The parent command structure
     * @param flag                The short version of the value flag
     * @param l_flag              The long version of the value flag
     * @param placeholder         Text placeholder for value.
     * @param desc                Description of the value flag
     * @param flags                1 if flag is arg, else 0
     * @return struct flag*   Reference to the newly added optional value
     */
    struct flag *command_add_flag_value(struct command * ctx, char const flag, char const *const l_flag,
                                        const char *const placeholder, char const *const desc, unsigned int flags);

    /*!
     * @brief Add new optional list of values to command
     *
     * @param ctx                 The parent command structure
     * @param flag                The short version of the list of values flag
     * @param l_flag              The long version of the list of values flag
     * @param placeholder         Text placeholder for value.
     * @param desc                Description of the list of values flag
     * @param flags                1 if flag is arg, else 0
     * @return struct flag*   Reference to the newly added optional list of values
     */
    struct flag *command_add_flag_list(struct command * ctx, char const flag, char const *const l_flag,
                                       char const *const placeholder, char const *const desc, unsigned int flags);

    /*!
     * @brief Add new arg value to command
     *
     * @param ctx                 The parent command structure
     * @param flag                The short version of the value flag
     * @param l_flag              The long version of the value flag
     * @param desc                Description of the value flag
     * @return struct arg*   Reference to the newly added arg value
     */
    struct arg *command_add_arg_value(struct command * ctx, char const *const name, char const *const desc);

    /*!
     * @brief Add new arg list of values to command
     *
     * @param ctx                 The parent command structure
     * @param flag                The short version of the list of values flag
     * @param l_flag              The long version of the list of values flag
     * @param desc                Description of the list of values flag
     * @return struct arg*   Reference to the newly added arg list of values
     */
    struct arg *command_add_arg_list(struct command * ctx, char const *const name, char const *const desc);

    /*!
     * @brief Parser structure holding all optional/arg values and commands
     */
    struct parser;

    /*!
     * @brief Initializes a new parser structure, call parser_deinit(..) to free it
     *
     * @param name               Name of the application (most likely argv[0])
     * @param desc               Description of the application, custom linebreaks supported
     * @param footer             Optional footer text
     * @return struct parser*    Reference to the newly allocated parser structure
     */
    struct parser *parser_init(char const *const name, char const *const desc, char const *const footer);

    /*!
     * @brief Deinitializes the parser structure, freeing all optional/arg parameters and subcommands
     *
     * @param ctx    The parser context
     */
    void parser_deinit(struct parser * ctx);

    /*!
     * @brief Adds a new command to the parser
     *
     * @param ctx                  The parser context
     * @param name                 Name of the command
     * @param desc                 Description of the command
     * @param footer             Optional footer text
     * @return struct command*     Reference to the created command
     */
    struct command *parser_add_command(struct parser * ctx, char const *const name, char const *const desc,
                                       char const *const footer);

    /*!
     * @brief Adds a new optional flag to the parser
     *
     * @param ctx                  The parser context
     * @param flag                 Short flag of the optional
     * @param l_flag               Long flag of the optional
     * @param desc                 Description of the optional flag
     * @return struct flag*    Reference to the created optional flag
     */
    struct flag *parser_add_flag(struct parser * ctx, char const flag, char const *const l_flag,
                                 char const *const desc);

    /*!
     * @brief Adds a new optional value to the parser
     *
     * @param ctx                  The parser context
     * @param flag                 Short flag of the optional value
     * @param l_flag               Long flag of the optional value
     * @param placeholder          Text placeholder for value
     * @param desc                 Description of the optional value
     * @param flags                1 if flag is arg, else 0
     * @return struct flag*    Reference to the created optional value
     */
    struct flag *parser_add_flag_value(struct parser * ctx, char const flag, char const *const l_flag,
                                       const char *const placeholder, char const *const desc, unsigned int flags);

    /*!
     * @brief Adds a new optional value list to the parser
     *
     * @param ctx                  The parser context
     * @param flag                 Short flag of the optional value list
     * @param l_flag               Long flag of the optional value list
     * @param placeholder          Text placeholder for value
     * @param desc                 Description of the optional value list
     * @param flags                1 if flag is arg, else 0
     * @return struct flag*    Reference to the created optional value list
     */
    struct flag *parser_add_flag_list(struct parser * ctx, char const flag, char const *const l_flag,
                                      const char *const placeholder, char const *const desc, unsigned int flags);

    /*!
     * @brief Adds a new arg value to the parser
     *
     * @param ctx                  The parser context
     * @param name                 Name of the arg value
     * @param desc                 Description of the arg value
     * @return struct arg*    Reference to the created optional value
     */
    struct arg *parser_add_arg_value(struct parser * ctx, char const *const name, char const *const desc);

    /*!
     * @brief Adds a new arg value list to the parser
     *
     * @param ctx                  The parser context
     * @param name                 Name of the arg value list
     * @param desc                 Description of the arg value list
     * @return struct arg*    Reference to the created optional value list
     */
    struct arg *parser_add_arg_list(struct parser * ctx, char const *const name, char const *const desc);

    /*!
     * @brief Parsing of the given arguments
     *
     * @param ctx    The context containing the supported argument definitions
     * @param argv   The array of commandline arguments
     * @param argc   Number of commandline arguments provided
     * @return int   1 on success, 0 on failure.
     */
    int parser_parse_args(struct parser * ctx, char const *const *argv, int argc);

/*!
 * @brief See parser_init(..)
 */
#define parser_new(var, name, desc, footer) struct parser *var = parser_init(name, desc, footer)

/*!
 * @brief See parser_add_flag(..)
 */
#define add_flag(parser, var, s_flag, l_flag, desc) struct flag *var = parser_add_flag(parser, s_flag, l_flag, desc)

/*!
 * @brief See parser_add_flag_value(..)
 */
#define add_flag_value(parser, var, s_flag, l_flag, placeholder, desc, flags)                                          \
    struct flag *var = parser_add_flag_value(parser, s_flag, l_flag, placeholder, desc, flags)

/*!
 * @brief See parser_add_flag_list(..)
 */
#define add_flag_list(parser, var, s_flag, l_flag, placeholder, desc, flags)                                           \
    struct flag *var = parser_add_flag_list(parser, s_flag, l_flag, placeholder, desc, flags)

/*!
 * @brief See parser_add_arg_value(..)
 */
#define add_arg_value(parser, var, name, desc) struct arg *var = parser_add_arg_value(parser, name, desc)

/*!
 * @brief See parser_add_arg_list(..)
 */
#define add_arg_list(parser, var, name, desc) struct arg *var = parser_add_arg_list(parser, name, desc)

/*!
 * @brief See parser_add_command(..)
 */
#define add_command(parser, var, name, desc, footer)                                                                   \
    struct command *var = parser_add_command(parser, name, desc, footer)

/*!
 * @brief See command_add_flag(..)
 */
#define cmd_add_flag(cmd, var, s_flag, l_flag, desc)                                                                   \
    struct flag *var = command_add_flag(cmd, s_flag, l_flag, desc, SET_NONE)

/*!
 * @brief See command_add_flag_value(..)
 */
#define cmd_add_flag_value(cmd, var, s_flag, l_flag, placeholder, desc, flags)                                         \
    struct flag *var = command_add_flag_value(cmd, s_flag, l_flag, placeholder, desc, flags)

/*!
 * @brief See command_add_flag_list(..)
 */
#define cmd_add_flag_list(cmd, var, s_flag, l_flag, placeholder, desc, flags)                                          \
    struct flag *var = command_add_flag_list(cmd, s_flag, l_flag, placeholder, desc, flags)

/*!
 * @brief See command_add_arg_value(..)
 */
#define cmd_add_arg_value(cmd, var, name, desc) struct arg *var = command_add_arg_value(cmd, name, desc)

/*!
 * @brief See command_add_arg_list(..)
 */
#define cmd_add_arg_list(cmd, var, name, desc) struct arg *var = command_add_arg_list(cmd, name, desc)

/*!
 * @brief See command_add_command(..)
 */
#define cmd_add_subcommand(cmd, var, name, desc, footer)                                                               \
    struct command *var = command_add_subcommand(cmd, name, desc, footer)

#ifdef __cplusplus
}
#endif

#endif // __ARGPARSE_C__
