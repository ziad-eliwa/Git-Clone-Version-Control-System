#include "vector.h"
#include <functional>
#include <string>

class IOption {
public:
  std::string name;
  std::string description;

  virtual ~IOption() = default;
  IOption(std::string name, std::string description);
  virtual int parse(int argc, char *argv[]) = 0;
};

template <class T> class Option : public IOption {
private:
  T &data;

public:
  Option(T &data, std::string name, std::string description);
  int parse(int argc, char *argv[]) override;
};
using Flag = Option<bool>;

class ArgParser {
private:
  std::string name;
  std::string description;

  Vector<ArgParser *> commands;
  Vector<IOption *> options;
  std::function<void()> callback;

public:
  ArgParser();
  ArgParser(std::string appName, std::string description);

  ArgParser &add_command(std::string name, std::string description);
  template <class T>
  ArgParser &add_option(T &data, std::string name, std::string description);
  ArgParser &set_callback(std::function<void()> callback);

  bool parse_args(int argc, char *argv[]);
};
