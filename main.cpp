#include "object_store.h"
#include <iostream>

// We store the instructions we have here
const std::string HELP_MESSAGE = "usage: jit <command> [<args>]\n\n"
                                 "Here is a list of the available commands:\n"
                                 "  store   Insert file into the object store";  // test

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << HELP_MESSAGE << std::endl;
    return 0;
  }

  ObjectStore store(".jit/objects/");

  std::string command = argv[1];
  if (command == "store") {
    if (argc < 3) {
      std::cout << "invalid usage: missing file name\n\n"
                   "usage: jit store <file_path>"
                << std::endl;
      return 0;
    }
    for (int j = 2; j < argc; j++) {
      std::string filePath = argv[j];
      store.store(filePath);
    }

  } else {
    std::cout << "unknown command: " << command << "\n\n"
              << HELP_MESSAGE << std::endl;
    return 0;
  }

  return 0;
}
