#include <iostream>

const std::string HELP_MESSAGE = "usage: jit <command> [<args>]\n\n"
                                 "Here is a list of the available commands:\n"
                                 "  store   Insert file into the object store";

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << HELP_MESSAGE << std::endl;
    return 0;
  }

  std::string command = argv[1];
  if (command == "store") {
    if (argc < 3) {
      std::cout << "invalid usage: missing file name\n\n"
                   "usage: jit store <file_name>"
                << std::endl;
      return 0;
    }
    std::string file_name = argv[2];
  } else {
    std::cout << "unknown command: " << command << "\n\n"
              << HELP_MESSAGE << std::endl;
    return 0;
  }
  return 0;
}
