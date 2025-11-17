#include "argparser.h"
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
  ObjectStore store(storePath);

  ArgParser parser(argv[0], HELP_MESSAGE);
  std::string filePath;
  parser.add_command("store", "Insert file into the object store")
      .add_argument(filePath, "file_path", "")
      .set_callback([&]() {
        tree test;
        std::cout << filePath << std::endl;
        store.store(filePath, test);
      });

  parser.parse_args(argc, argv);
  return 0;
}
