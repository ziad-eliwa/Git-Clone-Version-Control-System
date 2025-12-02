#include "argparser.h"
#include "gitobjects.h"
#include "hashmap.h"
#include "helpers.h"
#include "index.h"
#include "objectstore.h"
#include "refs.h"
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

// We store the instructions we have here
// Staging Area: add, commit, log, status, reset
// Branching: , merge, rebase, diff
// Networking: push pull, fetch
const std::filesystem::path REPO_ROOT = "./.jit";

std::filesystem::path repoRoot() {
  std::filesystem::path d = std::filesystem::current_path();
  while (d.parent_path() != d) {
    if (std::filesystem::exists(d / ".jit"))
      return d / ".jit";
    d = d.parent_path();
  }
  if (std::filesystem::exists(d / ".jit"))
    return d / ".jit";
  throw std::runtime_error("no jit repository found.\n  (use 'jit init' to "
                           "initialize a new jit repository.)");
};

int main(int argc, char *argv[]) {
  ArgParser parser(argv[0], "Jit Version Control System.");
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
    std::filesystem::path repo = "./.jit";
    if (std::filesystem::exists(repo))
      throw std::runtime_error(".jit repository already exists");
    Refs refs(repo / "refs", repo / "HEAD");
    refs.updateHead("main");
  });

  std::string addPath;
  parser.add_command("add", "Add file to the staging area")
      .set_callback([&]() {
        std::filesystem::path repo = repoRoot();
        ObjectStore store(repo / "objects");
        IndexStore index(repo / "index", store);
        index.add(addPath);
        index.save();
      })
      .add_argument(addPath, "File Path", "");

  std::string commitMessage;
  parser.add_command("commit", "Add file to the staging area")
      .set_callback([&]() {
        std::filesystem::path repo = repoRoot();
        ObjectStore store(repo / "objects");
        IndexStore index(repo / "index", store);
        Refs refs(repo / "refs", repo / "HEAD");

        std::string current = refs.resolve("HEAD");

        Tree commitTree = index.writeTree();
        store.store(&commitTree);

        Commit *newCommit =
            new Commit(commitMessage, "pharaok", commitTree.getHash());

        std::cout << current << std::endl;
        if (current != "")
          newCommit->addParentHash(current);

        store.store(newCommit);
        if (refs.head().rfind("ref ", 0) == 0)
          refs.updateRef(refs.head().substr(4), newCommit->getHash());
      })
      .add_option(commitMessage, "-m,--message",
                  "Must be between double quotations.");

  parser.add_command("log", "Display the log of the commits")
      .set_callback([&]() {
        std::filesystem::path repo = repoRoot();
        ObjectStore store(repo / "objects");
        Refs refs(repo / "refs", repo / "HEAD");

        std::string lastCommit = refs.resolve("HEAD");
        if (lastCommit == "") {
          std::cout << "your current branch does not have any commits yet."
                    << std::endl;
          return;
        }

        std::cout << store.retrieveLog(lastCommit);
      });

  // Missing commands.
  // parser.add_command("diff", "")
  //     .set_callback([&]() {
  //
  //     })
  //     .add_argument(filePath, "", "");

  parser.add_command("status", "").set_callback([&]() {
    std::filesystem::path repo = repoRoot();
    ObjectStore store(repo / "objects");
    IndexStore index(repo / "index", store);

    enum class Status { Clean, NewFile, Modified, Deleted };

    // TODO: improve array search
    HashMap<std::string, Status> status;

    Vector<std::string> untracked;
    for (auto it = std::filesystem::recursive_directory_iterator(".");
         it != std::filesystem::end(it); it++) {
      const auto &entry = *it;
      if (entry.is_directory() && entry.path() == REPO_ROOT) {
        it.disable_recursion_pending();
        continue;
      }

      if (std::filesystem::is_regular_file(entry))
        untracked.push_back(standardPath(entry));
    }

    for (auto &[path, hash] : index.getEntries()) {
      bool found = false;
      for (auto &p : untracked) {
        if (p == path) {
          found = true;
          break;
        }
      }
      if (found) {
        Blob untrackedBlob = Blob(readFile(path));
        std::string untrackedHash = untrackedBlob.getHash();
        if (untrackedHash != hash)
          status[path] = Status::Modified;
      } else {
        status[path] = Status::Deleted;
      }
    }

    for (auto &path : untracked) {
      bool found = false;
      for (auto &[p, _] : index.getEntries()) {
        if (p == path) {
          found = true;
          break;
        }
      }
      if (!found)
        status[path] = Status::NewFile;
    }

    bool clean = true;
    // TODO: sort by status
    for (auto [path, status] : status) {
      switch (status) {
      case Status::NewFile:
        std::cout << "new file: " << path << std::endl;
        clean = false;
        break;
      case Status::Modified:
        std::cout << "modified: " << path << std::endl;
        clean = false;
        break;
      case Status::Deleted:
        std::cout << "deleted: " << path << std::endl;
        clean = false;
        break;
      }
    }
    if (clean) {
      std::cout << "Working tree clean." << std::endl;
    }
  });

  std::string commitHash;
  parser.add_command("checkout", "")
      .set_callback([&]() {
        std::filesystem::path repo = repoRoot();
        ObjectStore store(repo / "objects");
        IndexStore index(repo / "index", store);
        Refs refs(repo / "refs", repo / "HEAD");

        GitObject *obj = store.retrieve(commitHash);
        if (Commit *c = dynamic_cast<Commit *>(obj)) {
          store.reconstruct(c->getTreeHash(), "./");
          index.readTree(".", c->getTreeHash());
          // TODO: orphaned commits
          refs.updateHead(commitHash);
          index.save();
        }
      })
      .add_argument(commitHash, "Commit hash", "");

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

  try {
    parser.parse(argc, argv);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
