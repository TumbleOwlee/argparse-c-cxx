/*********************************************************************************************************************/

#include <algorithm>
#include <limits>
#include <memory>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <vector>

namespace argparse {

/*********************************************************************************************************************
 *
 * argparse::parse template/specialization
 *
 * This template and its specializations are used to parse the input
 * arguments into the requested type. Currently only int and std::string
 * are already implemented.
 *
 * FIXME: Make it possible to pass a custom converter into any of the
 *        added arguments.
 *
 *********************************************************************************************************************/

template <typename T> static auto parse(char const *const s) -> T;

template <> auto parse(char const *const s) -> int;
template <> auto parse(char const *const s) -> std::string;

/*********************************************************************************************************************
 *
 * argparse::optional - base class for optional arguments
 *
 * This class specified the necessary interface methods and already
 * stores the common values, such as name and description.
 *
 *********************************************************************************************************************/

class optional {
public:
  optional(char _short, std::string_view _long, std::string_view _desc);
  virtual ~optional();

  optional(optional &&) = delete;
  optional(optional const &) = delete;

  auto operator=(optional &&) -> optional & = delete;
  auto operator=(optional const &) -> optional & = delete;

  auto desc() -> std::string_view const &;
  auto abbr() -> std::tuple<char, std::string_view>;

  virtual auto takes() -> size_t = 0;
  virtual auto parse(char const *const *argv, int argc) -> int;

protected:
  char _short;
  std::string_view _long;
  std::string_view _desc;
};

/*********************************************************************************************************************
 *
 * argparse::optional_flag - specialization of optional for flag arguments
 *
 * An instance of this class specifies a flag parameter. A flag can either
 * be set (true) or unset (false). Additionally a flag can be specified
 * multiple times `-v -v -v`.
 *
 * FIXME: Support multiple occurrances of the same flag using the common
 *        scheme like `-vvv` instead of `-v -v -v`
 *
 *********************************************************************************************************************/

class optional_flag : public optional {
public:
  optional_flag(char _short, std::string_view _long, std::string_view _desc);

  auto cnt() const -> size_t;
  auto is_set() const -> bool;

  auto takes() -> size_t override;
  auto parse(char const *const *argv, int len) -> int override;

private:
  size_t _cnt;
  bool _flag;
};

/*********************************************************************************************************************
 *
 * argparse::optional_value - specialization of optional for value inputs
 *
 * An instance of this class will get an optional value parameter from
 * the commandline and parse it into the template type.
 *
 *********************************************************************************************************************/

template <typename T> class optional_value : public optional {
public:
  optional_value(char _short, std::string_view _long, std::string_view _desc) : optional(_short, _long, _desc) {}

  auto get_value() const -> T const * { return std::get_if<T>(&_value); }

  auto takes() -> size_t override { return 1; }
  auto parse(char const *const *argv, int len) -> int override {
    if (len < 1) {
      return -1;
    }
    _value = argparse::parse<T>(argv[0]);
    return 1;
  }

private:
  std::variant<std::monostate, T> _value;
};

/*********************************************************************************************************************
 *
 * argparse::optional_list - specialization of optional for value lists
 *
 * An instance of this class will get an optional list of values from
 * the commandline and parse each one into the template type.
 *
 *********************************************************************************************************************/

template <typename T> class optional_list : public optional {
public:
  optional_list(char _short, std::string_view _long, std::string_view _desc)
      : optional(_short, _long, _desc), _values() {}

  auto get_values() const -> std::vector<T> const & { return _values; }

  auto takes() -> size_t override { return std::numeric_limits<size_t>::max(); }
  auto parse(char const *const *argv, int len) -> int override {
    if (len < 1) {
      return -1;
    }
    auto cnt = 0;
    for (const auto &v : std::span(argv, len)) {
      _values.push_back(argparse::parse<T>(v));
      cnt += 1;
    }

    return cnt;
  }

private:
  std::vector<T> _values;
};

/*********************************************************************************************************************
 *
 * argparse::argument - base class of required/non-optional parameters
 *
 * This class specified the necessary interface methods and already
 * stores the common values, such as name and description.
 *
 *********************************************************************************************************************/

class argument {
public:
  argument(std::string_view _name, std::string_view _desc);
  virtual ~argument();

  argument(argument &&) = delete;
  argument(argument const &) = delete;

  auto operator=(argument &&) -> argument & = delete;
  auto operator=(argument const &) -> argument & = delete;

  auto name() -> std::string_view;
  auto desc() -> std::string_view const &;

  virtual auto takes() -> size_t = 0;
  virtual auto parse(char const *const *argv, int len) -> int;

protected:
  std::string_view _name;
  std::string_view _desc;
};

/*********************************************************************************************************************
 *
 * argparse::required_value - specialization of argument for a required value
 *
 * An instance of this class will get an required value parameter from
 * the commandline and parse it into the template type.
 *
 *********************************************************************************************************************/

template <typename T> class required_value : public argument {
public:
  required_value(std::string_view _name, std::string_view _desc) : argument(_name, _desc) {}

  auto get_value() const -> T const * { return std::get_if<T>(&_value); }

  auto takes() -> size_t override { return 1; }
  auto parse(char const *const *argv, int len) -> int override {
    if (len < 1) {
      return -1;
    }
    _value = argparse::parse<T>(argv[0]);
    return 1;
  }

private:
  std::string_view _name;
  std::variant<std::monostate, T> _value;
};

/*********************************************************************************************************************
 *
 * argparse::required_list - specialization of argument for value lists
 *
 * An instance of this class will get a required list of values from
 * the commandline and parse each one into the template type.
 *
 *********************************************************************************************************************/

template <typename T> class required_list : public argument {
public:
  required_list(std::string_view _name, std::string_view _desc) : argument(_name, _desc), _values() {}

  auto get_values() const -> std::vector<T> const & { return _values; }

  auto takes() -> size_t override { return std::numeric_limits<size_t>::max(); }
  auto parse(char const *const *argv, int len) -> int override {
    if (len < 1) {
      return -1;
    }

    auto cnt = 0;
    for (const auto &v : std::span(argv, len)) {

      _values.push_back(argparse::parse<T>(v));
      cnt += 1;
    }
    return cnt;
  }

private:
  std::vector<T> _values;
};

/*********************************************************************************************************************
 *
 * argparse::command - Commands and subcommands
 *
 * An instance of this class describes a command/subcommand with its own
 * optional and/or required parameters. Each command can also contain
 * additional subcommands.
 *
 *********************************************************************************************************************/

class command : public argument {

  friend class argparse;

public:
  command(std::string_view _name, std::string_view _desc);

  template <typename Opt> auto add_optional_arg(char const _short, std::string_view _long, std::string_view _desc) {
    auto opt = std::make_unique<Opt>(_short, _long, _desc);
    if (std::ranges::any_of(_optional.begin(), _optional.end(), [_short, _long](auto &ptr) -> bool {
          auto [s, l] = ptr->abbr();
          return s == _short || l == _long;
        })) {
      auto msg = std::string("Duplicated optional argument for ") + _short + "/" + _long.data();
      throw std::runtime_error(msg);
    }
    _optional.push_back(std::move(opt));
  }

  template <typename Arg> auto add_required_arg(std::string_view _name, std::string_view _desc) {
    auto arg = std::make_unique<Arg>(_name, _desc);
    if (std::ranges::any_of(_required.begin(), _required.end(),
                            [_name](auto &ptr) -> bool { return _name == ptr->name(); })) {
      auto msg = std::string("Duplicated required argument for ") + _name.data();
      throw std::runtime_error(msg);
    }
    _required.push_back(std::move(arg));
  }

  template <typename T> auto get_optional(std::string_view _long) -> T const & {
    auto v = std::ranges::filter_view(std::span(_optional.begin(), _optional.end()), [_long](auto &ptr) -> bool {
      auto [s, l] = ptr->abbr();
      return l == _long;
    });
    if (v.empty()) {
      abort();
    }
    return *reinterpret_cast<T const *>(v.begin()->get());
  }

  template <typename T> auto get_required(std::string_view _name) -> T const & {
    auto v = std::ranges::filter_view(std::span(_required.begin(), _required.end()),
                                      [_name](auto &ptr) -> bool { return ptr->name() == _name; });
    if (v.empty()) {
      abort();
    }
    return *reinterpret_cast<T const *>(v.begin()->get());
  }

  auto takes() -> size_t override;

  auto add_command(std::string_view name, std::string_view desc) -> command &;

protected:
  std::string _base;
  std::vector<std::unique_ptr<optional>> _optional;
  std::vector<std::unique_ptr<argument>> _required;
  std::vector<std::unique_ptr<command>> _commands;

  auto show_help() const -> void;

  void set_base(std::string_view base);

  auto parse(char const *const *argv, int argc) -> int override;
};

/*********************************************************************************************************************
 *
 * argparse::parser - CLI parser class
 *
 * An instance of parser is the root of a comamnd/subcommand tree. It works
 * like the argparse::command with the only difference that it will return
 * a simple true/false as parsing result instead of custom integer values
 * that are for internal use to propagate how many arguments were taken
 * by the given argument.
 *
 *********************************************************************************************************************/

class parser : public command {
public:
  parser(std::string_view _name, std::string_view _desc);
  ~parser();

  // Prevent unnecessary copy or move
  parser(parser &&) = delete;
  parser(parser const &) = delete;

  auto operator=(parser &&) -> parser & = delete;
  auto operator=(parser const &) -> parser & = delete;

  auto parse(int argc, char *argv[]) -> bool;
};

} // namespace argparse

/*********************************************************************************************************************/
