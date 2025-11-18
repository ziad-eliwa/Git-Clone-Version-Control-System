#include "argparser.h"
#include "gitobjects.h"
#include "object_store.h"
#include <iostream>

// We store the instructions we have here
// Staging Area: add, commit, log, status, reset
// Branching: , merge, rebase, diff
// Networking: push pull, fetch

const std::string STORE_PATH = ".jit/objects/";

int main(int argc, char *argv[]) {
  ObjectStore store(STORE_PATH);

  ArgParser parser(argv[0], "Jit is a version control system.");
  parser.add_command("help", "Show this help message").set_callback([&]() {
    std::cout << parser.help_message() << std::endl;
  });

  // serializing and storing the tree
  // This command is only for testing
  std::string filePath;
  parser.add_command("store", "Insert file or directory into the object store")
      .set_callback([&]() {
        tree storedTree;
        std::cout << filePath << std::endl;
        store.store(filePath, storedTree);
      })
      .add_argument(filePath, "file_path", "");

  // deserializing the tree
  // This command is only for testing
  std::string treeHash;
  parser.add_command("read", "Read hash")
      .set_callback([&]() {
          tree retrievedTree = store.retrieveTree(treeHash);
          std::cout << retrievedTree.serialize() << "\n";
      })
      .add_argument(treeHash, "file Hash", "");

  parser.parse(argc, argv);
  return 0;
}
