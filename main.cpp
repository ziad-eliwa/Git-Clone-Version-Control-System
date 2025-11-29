#include "argparser.h"
#include "gitobjects.h"
#include "index.h"
#include "object_store.h"
#include <filesystem>
#include <iostream>
#include <string>

// We store the instructions we have here
// Staging Area: add, commit, log, status, reset
// Branching: , merge, rebase, diff
// Networking: push pull, fetch
const std::string REPO_ROOT = "./.jit";
const std::string STORE_PATH = REPO_ROOT + "/objects";
const std::string HEAD_PATH = REPO_ROOT + "/HEAD";

int main(int argc, char *argv[]) {
  ObjectStore store;
  IndexStore index(REPO_ROOT, store);

  ArgParser parser(argv[0], "Jit is a version control system.");
  parser.add_command("help", "Show this help message").set_callback([&]() {
    std::cout << parser.help_message() << std::endl;
  });

  // serializing and storing the tree
  // This command is only for testing
  // std::string filePath;
  // parser.add_command("store", "Insert file or directory into the object
  // store")
  //     .set_callback([&]() {
  //       std::cout << filePath << std::endl;
  //       store.store(filePath);
  //     })
  //     .add_argument(filePath, "file_path", "");

  // deserializing the tree
  // This command is only for testing
  // std::string treeHash;
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

  std::string addPath;
  parser.add_command("add", "Add file to the staging area")
      .set_callback([&]() {
        index.add(addPath);
        index.save();
      })
      .add_argument(addPath, "File Path", "");

  std::string commitMessage;
  parser.add_command("commit", "Add file to the staging area")
      .set_callback([&]() {
        std::string current = store.retrieveHead(HEAD_PATH);

        IndexStore index(REPO_ROOT, store);
        Tree commitTree = index.writeTree();
        store.store(&commitTree);
        std::cout << commitTree.serialize() << std::endl;

        Commit *newCommit =
            new Commit(commitMessage, "pharaok", commitTree.getHash());

        if (current != "")
          newCommit->addParentHash(current);
        store.store(newCommit);
        store.storeHead(newCommit->getHash(), HEAD_PATH);
      })
      .add_option(commitMessage, "-m,--message",
                  "Must be between double quotations.");

  parser.add_command("log", "Display the log of the commits")
      .set_callback([&]() {
        std::string LastCommit = store.retrieveHead(HEAD_PATH);
        if (LastCommit != "") {
          std::string result = store.retrieveLog(LastCommit);
          std::cout << result;
        } else {
          std::cout
              << "Jit repository is empty now. \n No commits are found yet.\n";
        }
      });

  // Missing commands.
  // parser.add_command("diff", "")
  //     .set_callback([&]() {
  //
  //     })
  //     .add_argument(filePath, "", "");

  parser.add_command("status", "").set_callback([&]() {
    // Tracked Files
    Vector<std::string> entries;
    for (auto it = std::filesystem::recursive_directory_iterator(".");
         it != std::filesystem::end(it); it++) {
      const auto &entry = *it;
      if (entry.is_directory() && entry.path() == REPO_ROOT) {
        it.disable_recursion_pending();
        continue;
      }
      if (std::filesystem::is_regular_file(entry))
        entries.push_back(entry.path());
    }
    for (auto &s : entries)
      std::cout << s << " ";
    std::cout << std::endl;
    // Untracked Files
  });

  std::string commitHash;
  parser.add_command("checkout", "")
      .set_callback([&]() {
        GitObject *obj = store.retrieve(commitHash);
        if (Commit *c = dynamic_cast<Commit *>(obj)) {
          store.reconstruct(c->getTreeHash(), "./");
          index.readTree(".", c->getTreeHash());
          store.storeHead(commitHash, HEAD_PATH);
          index.save();
        }
      })
      .add_argument(commitHash, "", "");

  // parser.add_command("branch", "").set_callback([&]() {
  //   // Print Branches and Working Branch
  // });

  std::string branchName;
  parser.add_command("branch", "")
      .set_callback([&]() {
        // Create Branches if not exists
      })
      .add_argument(branchName, "", "");

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
