#include "Index.h"
#include "argparser.h"
#include "commit.h"
#include "gitobjects.h"
#include "object_store.h"
#include <filesystem>
#include <iostream>
#include <pwd.h>
#include <unistd.h>

// We store the instructions we have here
// Staging Area: add, commit, log, status, reset
// Branching: , merge, rebase, diff
// Networking: push pull, fetch
const std::string STORE_PATH = ".jit/objects/";
const std::string REPO_ROOT = ".jit";

// namespace fs = std::filesystem;
// // Helper function used to stage files
// void stage_file(std::string path, Index& index) {
//     if (!fs::exists(path)) return;
//     index.add(path);

//     std::cout << "Added " << path << std::endl;
// }
// // helper function used to stage directories
// void stage_directory(std::string dirPath, Index& index) {
//     if (!fs::exists(dirPath)) return;

//     for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
//         std::string path = entry.path().string();
//         if (path.find(REPO_ROOT) != std::string::npos) continue;
//         if (!entry.is_directory()) {
//             stage_file(path, index);
//         }
//     }
// }
int main(int argc, char *argv[]) {
  ObjectStore store;

  ArgParser parser(argv[0], "Jit is a version control system.");
  parser.add_command("help", "Show this help message").set_callback([&]() {
    std::cout << parser.help_message() << std::endl;
  });
  // std::string fileToAdd;
  // parser.add_command("add", "Add file contents to the index")
  //     .add_argument(fileToAdd, "Path", "File to add")
  //     .set_callback([&]() {
  //       Index index(REPO_ROOT, store);

  //       if (fs::is_directory(fileToAdd)) {
  //         stage_directory(fileToAdd, index);
  //       } else if (fs::exists(fileToAdd)) {
  //         stage_file(fileToAdd, index);
  //       } else {
  //         std::cerr << "fatal: pathspec '" << fileToAdd
  //                   << "' did not match any files\n";
  //         return;
  //       }
  //     });
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

  // Staging Area and Commits
  parser.add_command("init", "Initialize a repository").set_callback([&]() {
    std::filesystem::path store("./.jit");
    if (!std::filesystem::exists(store)) {
      if (!std::filesystem::create_directories(store))
        throw std::runtime_error("failed to create directory");
    }
  });

  parser.add_command("add", "Add file to the staging area")
      .set_callback([&]() {
        IndexStore index(STORE_PATH, store);
        index.add(filePath);
        index.save();
        std::cout << "Added successfuly\n";
      })
      .add_argument(filePath, "File Path", "");

  std::string commitMessage;
  parser.add_command("commit", "Add file to the staging area")
      .set_callback([&]() {
        // Not functioning yet.
        IndexStore index(STORE_PATH, store);

        // Needed: build tree from indexes and store the tree in .jit/objects/
        tree CommitTree = index.convertToTree();

        uid_t uid = getuid();
        passwd *pw = getpwuid(uid);
        commit newCommit(commitMessage, pw->pw_name,CommitTree.getHash());
        index.clearIndex();
      })
      .add_argument(commitMessage, "Commit Message",
                    "Must be between double quotations.");

  // Jit repository checking
  if (!std::filesystem::exists(".jit")) {
    std::string cmd = argv[1];
    if (cmd == "init") {
      store = ObjectStore(STORE_PATH);
      parser.parse(argc, argv);
    } else {
      std::cout << "A jit repository is not initialized yet.\n \t\t Use\t jit "
                   "init\t to initialize a repo\n";
    }
  } else {
    store = ObjectStore(STORE_PATH);
    parser.parse(argc, argv);
  }

  return 0;
}
