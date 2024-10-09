/*********************************************************************************************************************/

#include <iomanip>
#include <iostream>

#include "argparse.hxx"

/*********************************************************************************************************************
 * argparse::parse specializations
 *********************************************************************************************************************/

template <> auto argparse::parse(char const *const s) -> int { return atoi(s); }

template <> auto argparse::parse(char const *const s) -> std::string { return s; }

/*********************************************************************************************************************
 * argparse::optional::optional implementation
 *********************************************************************************************************************/

argparse::optional::optional(char _short, std::string_view _long, std::string_view _desc)
    : _short(_short), _long(_long), _desc(_desc) {}

argparse::optional::~optional() = default;

auto argparse::optional::abbr() -> std::tuple<char, std::string_view> { return {_short, _long}; }

auto argparse::optional::parse(char const *const * /*argv*/, int /*argc*/) -> int {
  throw std::runtime_error("Called 'parse' on optional type.");
}

auto argparse::optional::desc() -> std::string_view const & { return _desc; }

argparse::optional_flag::optional_flag(char _short, std::string_view _long, std::string_view _desc)
    : optional(_short, _long, _desc), _cnt(0), _flag(false) {}

auto argparse::optional_flag::takes() -> size_t { return 0; }

auto argparse::optional_flag::parse(char const *const *argv, int len) -> int {
  _flag = true;
  _cnt += 1;
  return 0;
}

auto argparse::optional_flag::is_set() const -> bool { return _flag; }

auto argparse::optional_flag::cnt() const -> size_t { return _cnt; }

/*********************************************************************************************************************
 * argparse::argument implementation
 *********************************************************************************************************************/

argparse::argument::argument(std::string_view _name, std::string_view _desc) : _name(_name), _desc(_desc) {}
argparse::argument::~argument() = default;

auto argparse::argument::desc() -> std::string_view const & { return _desc; }

auto argparse::argument::name() -> std::string_view { return _name; }

auto argparse::argument::parse(char const *const * /*argv*/, int /*len*/) -> int {
  throw std::runtime_error("Called 'parse' on argument type.");
}

/*********************************************************************************************************************
 * argparse::command implementation
 *********************************************************************************************************************/

argparse::command::command(std::string_view _name, std::string_view _desc) : argument(_name, _desc) {}

auto argparse::command::add_command(std::string_view name, std::string_view desc) -> command & {
  if (name.find(' ') != std::string_view::npos) {
    std::cerr << "Command '" << name << "' contains space." << std::endl;
    abort();
  }

  auto arg = std::make_unique<command>(name, desc);
  if (std::ranges::any_of(_commands.begin(), _commands.end(),
                          [name](auto &ptr) -> bool { return name == ptr->name(); })) {
    auto msg = std::string("Duplicated command for ") + name.data();
    throw std::runtime_error(msg);
  }

  auto s = std::string(_name.data()) + " ";
  if (!_base.empty()) {

    s = std::string(_base.data()) + " " + s;
  }

  arg->set_base(s);
  auto cmd = arg.get();
  _commands.push_back(std::move(arg));
  return *cmd;
}

auto argparse::command::takes() -> size_t { return std::numeric_limits<size_t>::max(); }

void argparse::command::set_base(std::string_view base) { _base = base; }

auto argparse::command::parse(char const *const *argv, int argc) -> int {
  auto next_idx = [argc, argv](int pos) -> int {
    for (auto i = pos; i < argc; ++i) {
      std::string_view sv(argv[i]);
      if (sv.starts_with("-")) {
        return i;
      }
    }
    return argc;
  };

  auto pos = 1;
  while (pos < argc) {
    std::string_view sv(argv[pos]);
    auto end = next_idx(pos + 1);

    if (sv == "--help" || sv == "-h") {
      show_help();
      return -1;
    } else if (sv.starts_with('-') && sv != "--") {
      auto arg = sv.starts_with("--") ? sv.substr(2) : sv.substr(1);

      auto v = std::ranges::filter_view(std::span(_optional.begin(), _optional.end()), [arg](auto &ptr) -> bool {
        auto [s, l] = ptr->abbr();
        return (arg.length() == 1 && s == arg[0]) || l == arg;
      });

      if (v.empty()) {
        return -1;
      }

      auto used = (*v.begin())->parse(&argv[pos + 1], end - pos - 1);
      if (used == -1) {
        show_help();
        return -1;
      }
      pos += used + 1;

    } else if (pos != argc) {
      for (auto &c : _commands) {
        if (c->name() == sv) {
          auto used = c->parse(&argv[pos], argc - pos);
          if (used == -1) {
            return -1;
          }
          pos += used + 1;
          break;
        }
      }
    } else {

      for (auto &r : _required) {
        if (pos >= argc) {
          return -1;
        }
        auto used = r->parse(&argv[pos], argc - pos);
        if (used == -1) {
          return -1;
        }
        pos += used;
      }
      return pos >= argc;
    }
  }

  return _required.empty() ? argc : -1;
}

auto argparse::command::show_help() const -> void {
  std::cout << std::endl << "    Usage: " << _base << _name << " ";

  if (!_optional.empty()) {
    std::cout << "[OPTIONS] ";
  }
  if (!_commands.empty()) {
    std::cout << "[COMMAND] ";
  }

  for (auto &r : _required) {

    std::cout << r->name() << " ";
    if (r->takes() > 1) {
      std::cout << "[" << r->name() << "...] ";
    }
  }
  std::cout << std::endl << std::endl;

  if (!_desc.empty()) {
    size_t start = 0;
    size_t end = 0;
    while (start < _desc.size()) {
      auto pos = _desc.find(' ', end + 1);
      if (pos == std::string_view::npos) {
        std::cout << "    " << _desc.substr(start) << std::endl;
        break;
      } else {
        if ((pos - start) > 80) {
          std::cout << "    " << _desc.substr(start, end - start) << std::endl;

          start = end + 1;

          end = end + 1;
        } else {
          end = pos;
        }
      }
    }
    std::cout << std::endl;
  }

  if (!_optional.empty()) {
    size_t width = 4;
    std::ranges::for_each(_optional.begin(), _optional.end(), [&width](auto &v) {
      auto [s, l] = v->abbr();
      width = std::max<size_t>(width, l.length() + 4);
    });

    std::cout << "    Options:" << std::endl << std::endl;
    for (auto &o : _optional) {
      auto [s, l] = o->abbr();
      std::cout << "        -" << s << ", --" << std::left << std::setw(width) << l << o->desc() << std::endl;
    }
    std::cout << std::endl;
  }

  if (!_commands.empty()) {
    size_t width = 4;
    std::ranges::for_each(_commands.begin(), _commands.end(),
                          [&width](auto &v) { width = std::max<size_t>(width, v->name().length() + 4); });

    std::cout << "    Commands:" << std::endl << std::endl;
    for (auto &r : _commands) {
      std::cout << "        " << std::left << std::setw(width) << r->name() << r->desc() << std::endl;
    }
    std::cout << std::endl << "        See '<command> --help' for additional info." << std::endl << std::endl;
  }

  if (!_required.empty()) {

    size_t width = 4;
    std::ranges::for_each(_required.begin(), _required.end(),
                          [&width](auto &v) { width = std::max<size_t>(width, v->name().length() + 4); });
    std::cout << "    Required:" << std::endl << std::endl;
    for (auto &r : _required) {
      std::cout << "        " << std::left << std::setw(width) << r->name() << r->desc() << std::endl;
    }
    std::cout << std::endl;
  }
}

/*********************************************************************************************************************
 * argparse::parser implementation
 *********************************************************************************************************************/

argparse::parser::parser(std::string_view _name, std::string_view _desc) : command(_name, _desc) {}
argparse::parser::~parser() = default;

auto argparse::parser::parse(int argc, char *argv[]) -> bool { return command::parse(argv, argc) == -1 ? false : true; }

/*********************************************************************************************************************/
