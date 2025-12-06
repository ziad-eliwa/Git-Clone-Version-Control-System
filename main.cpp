#include "argparser.h"
#include "deque.h"
#include "gitobjects.h"
#include "hashmap.h"
#include "helpers.h"
#include "index.h"
#include "objectstore.h"
#include "refs.h"
#include "vector.h"
#include <exception>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <diff.cpp>
// We store the instructions we have here
// Staging Area: add, commit, log, status, reset
// Branching: , merge, rebase, diff
// Networking: push pull, fetch

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
      throw std::runtime_error(".jit directory already exists");
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

        if (current != "")
          newCommit->addParentHash(current);

        store.store(newCommit);
        if (refs.isHeadBranch())
          refs.updateRef(refs.getHead(), newCommit->getHash());
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
  
  std::string filePath1, filePath2;
  parser.add_command("diff", "")
      .set_callback([&]() {
          std::ifstream file1(filePath1), file2(filePath2);
          Vector<std::string> lines1, lines2;
          std::string line;

          if (!file1.is_open()) {
              throw std::runtime_error("Cannot open file: " + filePath1);
          }
          while (std::getline(file1, line)) {
              lines1.push_back(line);
          }
          if (!file2.is_open()) {
              throw std::runtime_error("Cannot open file: " + filePath2);
          }
          while (std::getline(file2, line)) {
              lines2.push_back(line);
          }
          Vector<std::string> result = diff(lines1, lines2);
          for (const auto &line : result) std::cout << line << "\n";
      })
      .add_argument(filePath1, "", "")
      .add_argument(filePath2, "", "");

  parser.add_command("status", "").set_callback([&]() {
    std::filesystem::path repo = repoRoot();
    std::filesystem::path wd = repo.parent_path();
    ObjectStore store(repo / "objects");
    IndexStore index(repo / "index", store);

    enum class Status { Clean, NewFile, Modified, Deleted };

    // TODO: improve array search
    HashMap<std::string, Status> status;

    Vector<std::string> untracked;
    for (auto it = std::filesystem::recursive_directory_iterator(wd);
         it != std::filesystem::end(it); it++) {
      const auto &entry = *it;
      if (entry.path().filename().string().rfind(".jit", 0) == 0) {
        it.disable_recursion_pending();
        continue;
      }

      if (std::filesystem::is_regular_file(entry))
        untracked.push_back(pathString(entry));
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
        Blob untrackedBlob = Blob(readFile(wd / path));
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

  std::string target;
  parser.add_command("checkout", "")
      .set_callback([&]() {
        std::filesystem::path repo = repoRoot();
        std::filesystem::path wd = repo.parent_path();
        ObjectStore store(repo / "objects");
        IndexStore index(repo / "index", store);
        Refs refs(repo / "refs", repo / "HEAD");

        std::string hash =
            refs.isBranch(target) ? refs.resolve(target) : target;
        if (Commit *c = dynamic_cast<Commit *>(store.retrieve(hash))) {
          store.reconstruct(c->getTreeHash(), wd);
          index.readTree(wd, c->getTreeHash());
          refs.updateHead(target);
          index.save();
        } else {
          std::cout << "no such branch or commit '" << target << "'"
                    << std::endl;
        }
      })
      .add_argument(target, "Commit hash", "");

  // parser.add_command("branch", "").set_callback([&]() {
  //   // Print Branches and Working Branch
  // });

  std::string branchName;
  parser.add_command("branch", "")
      .set_callback([&]() {
        std::filesystem::path repo = repoRoot();
        Refs refs(repo / "refs", repo / "HEAD");
        if (branchName.empty()) {
          Vector<std::string> branches = refs.getRefs();
          for (auto &b : branches)
            std::cout << (b == refs.getHead() ? "+" : " ") << b << std::endl;
          return;
        }
        refs.updateRef(branchName, "HEAD");
      })
      .add_argument(branchName, "", "", false);

  parser.add_command("merge", "")
      .set_callback([&]() {
        std::filesystem::path repo = repoRoot();
        std::filesystem::path wd = repo.parent_path();
        ObjectStore store(repo / "objects");
        IndexStore index(repo / "index", store);
        Refs refs(repo / "refs", repo / "HEAD");

        auto resolveCommit = [&](std::string s) {
          std::string commitHash = refs.resolve(s);

          Commit *commit = nullptr;
          if (Commit *c = dynamic_cast<Commit *>(store.retrieve(commitHash)))
            commit = c;
          return commit;
        };

        Commit *ourHead = resolveCommit(refs.getHead());
        Commit *otherHead = resolveCommit(branchName);

        if (ourHead == nullptr) {
          std::cout << "head is detached" << std::endl;
          return;
        }

        if (otherHead == nullptr) {
          std::cout << "unknown branch '" << branchName << "'" << std::endl;
          return;
        }

        // fast-forward merge
        deque<Commit *> dq;
        dq.push_back(otherHead);
        while (!dq.empty()) {
          Commit *cur = dq.front();
          dq.pop_front();
          if (cur->getHash() == ourHead->getHash()) {
            // otherHead->getHash() is different for some reason
            refs.updateRef(refs.getHead(), refs.resolve(branchName));
            store.reconstruct(otherHead->getTreeHash(), wd);
            std::cout << "performed fast-forward merge" << std::endl;
            return;
          }
          for (auto &h : cur->getParentHashes()) {
            Commit *next = resolveCommit(h);

            if (next != nullptr)
              dq.push_back(next);
          }
        }

        // divergent branches
      })
      .add_argument(branchName, "", "");

  try {
    parser.parse(argc, argv);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
