#include "argparser.h"
#include <stdexcept>

IOption::IOption(std::string name, std::string description)
    : name(name), description(description) {}
template <class T>
Option<T>::Option(T &data, std::string name, std::string description)
    : IOption(name, description), data(data) {}

// instatiate string and bool options
template <> int Option<std::string>::parse(int argc, char *argv[]) {
  if (argc < 1)
    throw std::runtime_error("not enough arguments");
  if (argv[0] != name)
    return 0;
  if (argc < 2)
    throw std::runtime_error("not enough arguments");
  data = argv[1];
  return 2;
}
template <> int Flag::parse(int argc, char *argv[]) {
  if (argc < 1)
    throw std::runtime_error("not enough arguments");
  if (argv[0] != name)
    return 0;
  data = true;
  return 1;
}

IArgument::IArgument(std::string name, std::string description)
    : name(name), description(description) {}
template <class T>
Argument<T>::Argument(T &data, std::string name, std::string description)
    : IArgument(name, description), data(data) {}

template <> void Argument<std::string>::parse(int argc, char *argv[]) {
  if (argc < 1)
    throw std::runtime_error("not enough arguments");
  data = argv[0];
}

ArgParser::ArgParser()
    : name(""), description(""), commands(), options(), callback(nullptr) {}
ArgParser::ArgParser(std::string n, std::string d) : ArgParser() {
  name = n;
  description = d;
}

ArgParser &ArgParser::add_command(std::string name, std::string description) {
  ArgParser *ap = new ArgParser(name, description);
  commands.push_back(ap);
  return *ap;
}
template <class T>
ArgParser &ArgParser::add_argument(T &data, std::string name,
                                   std::string description) {
  IArgument *argument = new Argument(data, name, description);
  arguments.push_back(argument);
  return *this;
}
// instantiate add_argument for strings and bools
template ArgParser &
ArgParser::add_argument<std::string>(std::string &data, std::string name,
                                     std::string description);
template <class T>
ArgParser &ArgParser::add_option(T &data, std::string name,
                                 std::string description) {
  IOption *option = new Option(data, name, description);
  options.push_back(option);
  return *this;
}

// instantiate add_option for strings and bools
template ArgParser &ArgParser::add_option<std::string>(std::string &data,
                                                       std::string name,
                                                       std::string description);
template ArgParser &ArgParser::add_option<bool>(bool &data, std::string name,
                                                std::string description);

ArgParser &ArgParser::set_callback(std::function<void()> cb) {
  callback = cb;
  return *this;
}

bool ArgParser::parse_args(int argc, char *argv[]) {
  if (argc < 0)
    return false;
  if (name != argv[0])
    return false;

  for (auto cmd : commands) {
    if (cmd->parse_args(argc - 1, (argv + 1)))
      return true;
  }

  int i = 1;
  for (auto &arg : arguments) {
    arg->parse(argc - i, (argv + i));
    i++;
  }

  while (i < argc) {
    bool anyMatched = false;
    for (auto opt : options) {
      int di = opt->parse(argc - i, (argv + i));
      if (di) {
        anyMatched = true;
        i += di;
        break;
      }
    }
    if (!anyMatched)
      throw std::runtime_error(std::string("unrecognized option: ") + argv[i]);
  }
  // if (i < argc) // extra unparsed args
  if (callback != nullptr)
    callback();
  return true;
}

ArgParser::~ArgParser() {
  for (int i = 0; i < commands.size(); i++)
    delete commands[i];
  for (int i = 0; i < options.size(); i++)
    delete options[i];
}
