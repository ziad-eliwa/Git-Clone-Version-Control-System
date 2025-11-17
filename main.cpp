#include "gitobjects.h"
#include "object_store.h"
#include <iostream>

// We store the instructions we have here
// Staging Area: add, commit, log, status
// Branching: reset, merge, rebase, diff
// Networking: push pull, fetch

const std::string storePath = ".jit/objects/";
const std::string HELP_MESSAGE =
    "usage: jit <command> [<args>]\n\n"
    "Here is a list of the available commands:\n"
    "  store   Insert file into the object store"; // test

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << HELP_MESSAGE << std::endl;
    return 0;
  }

  ObjectStore store(storePath);

  std::string command = argv[1];
  if (command == "store") {
    if (argc < 3) {
      std::cout << "invalid usage: missing file name\n\n"
                   "usage: jit store <file_path>"
                << std::endl;
      return 0;
    }

    tree test;
    store.storeTree(argv[2], test);
  } else {
    std::cout << "unknown command: " << command << "\n\n"
              << HELP_MESSAGE << std::endl;
    return 0;
  }

  return 0;
}
