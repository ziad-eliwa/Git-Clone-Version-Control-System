#include "argparser.h"
#include "gitobjects.h"
#include "object_store.h"
#include <iostream>

// We store the instructions we have here
// Staging Area: add, commit, log, status
// Branching: reset, merge, rebase, diff
// Networking: push pull, fetch

const std::string STORE_PATH = ".jit/objects/";

int main(int argc, char *argv[]) {
  ObjectStore store(STORE_PATH);

  ArgParser parser(argv[0], "Jit is a version control system.");
  parser.add_command("help", "Show this help message").set_callback([&]() {
    std::cout << parser.help_message() << std::endl;
  });

  std::string filePath;
  parser.add_command("store", "Insert file or directory into the object store")
      .set_callback([&]() {
        tree test;
        std::cout << filePath << std::endl;
        store.store(filePath, test);
      })
      .add_argument(filePath, "file_path", "");

  parser.parse(argc, argv);
  return 0;
}
