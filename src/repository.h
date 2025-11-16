#pragma once

#include "hashmap.h"
#include "vector.h"
#include <iostream>
#include <sstream>

class commit {
private:
  std::string hash;
  std::string treeHash;
  std::string author;
  time_t timestamp;
  Vector<std::string> parentHashes;
  std::string message;

public:
  commit(std::string msg, std::string author, std::string treehash)
      : message(msg), treeHash(treeHash), author(author) {
    timestamp = time_t(nullptr);
    hash = computeHash();
  }

  std::string computeHash() { return std::to_string(Murmur3_32(serialize())); }

  std::string serialize() {
    std::stringstream ss;

    ss << "tree " << treeHash << "\n";
    for (std::string *i = parentHashes.begin(); i != parentHashes.end(); i++)
      ss << *i << " ";
    ss << "author " << author << " " << timestamp << "\n";
    ss << "\n" << message;
    std::string content = ss.str();
    return "commit " + std::to_string(content.size()) + '\0' + content;
  }

  std::string getHash() const { return hash; }
  std::string getTreeHash() const { return treeHash; }
  std::string getMessage() const { return message; }
};
