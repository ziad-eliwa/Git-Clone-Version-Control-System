#pragma once
#include "vector.h"
#include <functional>
#include <string>

class IOption {
public:
  std::string name;
  std::string description;
  bool required;

  bool matchName(std::string arg);
  virtual ~IOption() = default;
  IOption(std::string name, std::string description, bool required = false);
  virtual int parse(int argc, char *argv[]) = 0;
};

template <class T> class Option : public IOption {
private:
  T &data;

public:
  Option(T &data, std::string name, std::string description,
         bool required = false);
  int parse(int argc, char *argv[]) override;
};
using Flag = Option<bool>;

class IArgument {
public:
  std::string name;
  std::string description;

  virtual ~IArgument() = default;
  IArgument(std::string name, std::string description);
  virtual void parse(int argc, char *argv[]) = 0;
};
template <class T> class Argument : public IArgument {
private:
  T &data;

public:
  Argument(T &data, std::string name, std::string description);
  void parse(int argc, char *argv[]) override;
};

class ArgParser {
private:
  Vector<ArgParser *> commands;
  Vector<IArgument *> arguments;
  Vector<IOption *> options;
  std::function<void()> callback;

public:
  std::string name;
  std::string description;

  ArgParser();
  ArgParser(std::string appName, std::string description);
  ~ArgParser();

  ArgParser &add_command(std::string name, std::string description);
  template <class T>
  ArgParser &add_argument(T &data, std::string name, std::string description);
  template <class T>
  ArgParser &add_option(T &data, std::string name, std::string description,
                        bool required = false);
  ArgParser &set_callback(std::function<void()> callback);

  std::string help_message() const;

  bool parse(int argc, char *argv[]);
};
