#include "Index.h"
#include "argparser.h"
#include "gitobjects.h"
#include "object_store.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <pwd.h>
#include <string>
#include <unistd.h>

// We store the instructions we have here
// Staging Area: add, commit, log, status, reset
// Branching: , merge, rebase, diff
// Networking: push pull, fetch
const std::string REPO_ROOT = "./.jit/";
const std::string STORE_PATH = REPO_ROOT + "objects/";
const std::string HEAD_PATH = REPO_ROOT + "HEAD";

int main(int argc, char *argv[]) {
  ObjectStore store;

  ArgParser parser(argv[0], "Jit is a version control system.");
  parser.add_command("help", "Show this help message").set_callback([&]() {
    std::cout << parser.help_message() << std::endl;
  });

  // serializing and storing the tree
  // This command is only for testing
  std::string filePath;
  parser.add_command("store", "Insert file or directory into the object store")
      .set_callback([&]() {
        std::cout << filePath << std::endl;
        store.store(filePath);
      })
      .add_argument(filePath, "file_path", "");

  // deserializing the tree
  // This command is only for testing
  std::string treeHash;
  // parser.add_command("read", "Read hash")
  //     .set_callback([&]() {
  //       Tree retrievedTree = store.(treeHash);
  //       std::cout << retrievedTree.serialize() << "\n";
  //     })
  //     .add_argument(treeHash, "file Hash", "");

  // Staging Area and Commits
  parser.add_command("init", "Initialize a repository").set_callback([&]() {
    std::filesystem::path store(REPO_ROOT);
    if (!std::filesystem::exists(store)) {
      if (!std::filesystem::create_directories(store))
        throw std::runtime_error("failed to create directory");
    }
  });

  parser.add_command("add", "Add file to the staging area")
      .set_callback([&]() {
        IndexStore index(REPO_ROOT, store);
        index.add(filePath);
        index.save();
        std::cout << "Added successfuly\n";
      })
      .add_argument(filePath, "File Path", "");

  std::string commitMessage;
  parser.add_command("commit", "Add file to the staging area")
      .set_callback([&]() {
        std::ifstream head(HEAD_PATH);
        std::string current;
        if (head) {
          head >> current;
        }
        IndexStore index(REPO_ROOT, store);
        Tree commitTree = index.convertToTree();
        store.store(&commitTree);

        Commit *newCommit =
            new Commit(commitMessage, "pharaok", commitTree.getHash());
        std::cout << newCommit->serialize() << std::endl;
        if (current != "")
          newCommit->addParentHash(current);
        store.store(newCommit);

        std::ofstream headWrite(HEAD_PATH);
        headWrite << newCommit->getHash();
        head.close();
        headWrite.close();
      })
      .add_argument(commitMessage, "Commit Message",
                    "Must be between double quotations.");

  parser.add_command("log", "Display the log of the commits")
      .set_callback([&]() {
        std::ifstream last(HEAD_PATH);
        std::string LastCommit;
        last >> LastCommit;
        std::string result = store.retrieveLog(LastCommit);
        std::cout << result;
      });

  // Missing commands.
  parser.add_command("diff", "")
      .set_callback([&]() {

      })
      .add_argument(filePath, "", "");

  parser.add_command("status", "").set_callback([&]() {
    // Tracked Files
    // Untracked Files
  });

  std::string commitHash;
  parser.add_command("reset", "")
      .set_callback([&]() {
        // Reset the working directory to the desired commit.
      })
      .add_argument(commitHash, "", "");

  parser.add_command("branch", "").set_callback([&]() {
    // Print Branches and Working Branch
  });

  std::string branchName;
  parser.add_command("branch", "")
      .set_callback([&]() {
        // Create Branches if not exists
      })
      .add_argument(branchName, "", "");

  std::string checkHash;
  parser.add_command("checkout", "")
      .set_callback([&]() {
        // Switch to branch or a commit
      })
      .add_argument(checkHash, "", "Branch or Commit");

  parser.add_command("merge", "")
      .set_callback([&]() {
        // Merge two branches if possible
      })
      .add_argument(branchName, "", "");

  // Jit repository checking
  if (!std::filesystem::exists(REPO_ROOT)) {
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
