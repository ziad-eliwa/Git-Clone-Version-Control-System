#include "argparser.h"
#include "helpers.h"
#include "vector.h"
#include <stdexcept>

IOption::IOption(std::string name, std::string description, bool required)
    : name(name), description(description), required(required) {}
template <class T>
Option<T>::Option(T &data, std::string name, std::string description,
                  bool required)
    : IOption(name, description, required), data(data) {}
bool IOption::matchName(std::string arg) {
  Vector<std::string> parts = split(name, ',');
  for (auto &p : parts)
    if (p == arg)
      return true;
  return false;
}

// instatiate string and bool options
template <> int Option<std::string>::parse(int argc, char *argv[]) {
  if (argc < 1)
    throw std::runtime_error("not enough arguments");
  if (!matchName(argv[0]))
    return 0;
  if (argc < 2)
    throw std::runtime_error("not enough arguments");
  data = argv[1];
  return 2;
}
template <> int Flag::parse(int argc, char *argv[]) {
  if (argc < 1)
    throw std::runtime_error("not enough arguments");
  if (!matchName(argv[0]))
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
// instantiate add_argument for strings
template ArgParser &
ArgParser::add_argument<std::string>(std::string &data, std::string name,
                                     std::string description);

template <class T>
ArgParser &ArgParser::add_option(T &data, std::string name,
                                 std::string description, bool required) {
  IOption *option = new Option(data, name, description, required);
  options.push_back(option);
  return *this;
}

// instantiate add_option for strings and bools
template ArgParser &ArgParser::add_option<std::string>(std::string &data,
                                                       std::string name,
                                                       std::string description,
                                                       bool required);
template ArgParser &ArgParser::add_option<bool>(bool &data, std::string name,
                                                std::string description,
                                                bool required);

ArgParser &ArgParser::set_callback(std::function<void()> cb) {
  callback = cb;
  return *this;
}

bool ArgParser::parse(int argc, char *argv[]) {
  if (argc < 1)
    return false;
  if (name != argv[0])
    return false;

  int i = 1;
  for (auto cmd : commands) {
    if (cmd->parse(argc - i, (argv + i)))
      return true;
  }

  for (auto &arg : arguments) {
    arg->parse(argc - i, (argv + i));
    i++;
  }

  Vector<bool> seen(options.size(), false);
  while (i < argc) {
    bool anyMatched = false;
    for (int j = 0; j < options.size(); j++) {
      int di = options[j]->parse(argc - i, (argv + i));
      if (di) {
        seen[j] = true;
        anyMatched = true;
        i += di;
        break;
      }
    }
    if (!anyMatched)
      throw std::runtime_error(std::string("unrecognized option: ") + argv[i]);
  }
  for (int i = 0; i < options.size(); i++) {
    if (options[i]->required && !seen[i])
      throw std::runtime_error(
          std::string("missing required option: " + options[i]->name));
  }
  // if (i < argc) // extra unparsed args
  if (callback != nullptr)
    callback();
  return true;
}

std::string ArgParser::help_message() const {
  std::string message = "usage: " + name;

  Vector<std::string> args;
  // args.push_back("[-h | --help]"); // TODO: handle -h|--help
  if (!commands.empty())
    args.push_back("<command>");
  for (auto opt : options) {
    // TODO: move into IOption
    if (opt->required)
      args.push_back(opt->name);
    else
      args.push_back("[" + opt->name + "]");
  }
  args.push_back("[<args>]");

  const int MAX_WIDTH = 80;
  int width = message.length();
  for (auto arg : args) {
    if (width + 1 + arg.size() <= MAX_WIDTH)
      message += " ";
    else
      message += "\n";
    message += arg;
  }

  message += "\n\n";
  message += description;

  if (!commands.empty()) {
    message += "\n\nHere is a list of available commands:\n";
    int longest = 0;
    for (auto cmd : commands)
      longest = std::max(longest, (int)cmd->name.length());
    longest = ((longest + 4) / 4) * 4;
    for (auto cmd : commands) {
      message += "    " + cmd->name;
      message += std::string(longest - cmd->name.length(), ' ');
      message += cmd->description + "\n";
    }
  }

  if (!options.empty()) {
    message += "\n\nHere is a list of available options:\n";
    int longest = 0;
    for (auto opt : options)
      longest = std::max(longest, (int)opt->name.length());
    longest = ((longest + 4) / 4) * 4;
    for (auto opt : options) {
      message += "    " + opt->name;
      message += std::string(longest - opt->name.length(), ' ');
      message += opt->description + "\n";
    }
  }

  return message;
}

ArgParser::~ArgParser() {
  for (int i = 0; i < commands.size(); i++)
    delete commands[i];
  for (int i = 0; i < options.size(); i++)
    delete options[i];
}
