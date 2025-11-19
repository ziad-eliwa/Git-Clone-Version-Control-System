#include "argparser.h"
#include "gitobjects.h"
#include "object_store.h"
#include <iostream>
#include "Index.h"
#include <filesystem>


// We store the instructions we have here
// Staging Area: add, commit, log, status, reset
// Branching: , merge, rebase, diff
// Networking: push pull, fetch

namespace fs = std::filesystem;
const std::string REPO_ROOT = ".jit";
const std::string STORE_PATH = ".jit/objects/";
// Helper function used to stage files
void stage_file(std::string path, ObjectStore& store, Index& index) {

    blob b = store.storeBlob(path);
    index.add(path, b.getHash());
    std::cout << "Added " << path << std::endl;
}
// helper function used to stage directories
void stage_directory(std::string dirPath, ObjectStore& store, Index& index) {
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        std::string path = entry.path().string();

        if (path.find(".jit") != std::string::npos) continue;
        if (!entry.is_directory()) {
            stage_file(path, store, index);
        }
    }
}
int main(int argc, char *argv[]) {
  ObjectStore store(STORE_PATH);

  ArgParser parser(argv[0], "Jit is a version control system.");
  parser.add_command("help", "Show this help message").set_callback([&]() {
    std::cout << parser.help_message() << std::endl;
  });
    std::string fileToAdd;
    parser.add_command("add", "Add file contents to the index")
   .add_argument(fileToAdd,"Path","File to add")
    .set_callback([&]() {

        Index index (REPO_ROOT);
        if (fs::is_directory(fileToAdd)) {
            stage_directory(fileToAdd, store, index);
        }else if (fs::is_regular_file(fileToAdd)) {
            stage_file(fileToAdd, store, index);
        }
        index.save();

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
