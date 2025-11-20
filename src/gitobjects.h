#pragma once
#include "hashmap.h"
#include "helpers.h"
#include "vector.h"
#include <ctime>
#include <sstream>
#include <string>

// Structure resource:
// https://www.freecodecamp.org/news/git-internals-objects-branches-create-repo/

class GitObject {
public:
  std::string hash;
  virtual std::string getHash() const { return hash; }
  virtual std::string serialize() = 0;
  virtual ~GitObject() = default;
};

class Blob : public GitObject {
private:
  std::string content;

public:
  Blob() = default;
  Blob(std::string content) : content(content) {
    hash = computeHash(serialize());
  }

  std::string serialize() override {
    return "blob " + std::to_string(content.length()) + '\n' + content;
  }

  std::string getContent() { return content; }
};

class TreeEntry : public GitObject {
public:
  std::string type;
  std::string name;
  TreeEntry() = default;
  TreeEntry(std::string t, std::string n, std::string h) : type(t), name(n) {
    hash = h;
  }
  // For sorting entries
  bool operator<(const TreeEntry &other) const { return name < other.name; }
  std::string serialize() override { return type + ' ' + name + ' ' + hash; }
};

class Tree : public GitObject {
private:
  Vector<TreeEntry> entries;

public:
  void addEntry(const TreeEntry &entry) {
    entries.push_back(entry);
    entries.sort();
    hash = computeHash(serialize());
  }

  std::string serialize() override {
    std::string result = "tree " + std::to_string(entries.size()) + '\n';
    for (int i = 0; i < entries.size(); i++)
      result += entries[i].serialize() + '\n';
    return result;
  }
  Tree() = default;

  void addBlob(std::string blobName, Blob &b) {
    TreeEntry te("blob", blobName, b.getHash());
    addEntry(te);
  }

  void addSubTree(std::string treeName, Tree &t) {
    TreeEntry te("tree", treeName, t.getHash());
    addEntry(te);
  }

  const Vector<TreeEntry> &getEntries() const { return entries; }
};

class Commit : public GitObject {
private:
  std::string treeHash;
  std::string author;
  time_t timestamp;
  Vector<std::string> parentHashes;
  std::string message;

public:
  Commit() = default;
  Commit(std::string msg, std::string author, std::string treeHash)
      : message(msg), treeHash(treeHash), author(author) {
    timestamp = std::time(nullptr);
    hash = computeHash(serialize());
  }

  std::string serialize() override {
    std::string result =
        "commit " + std::to_string(4 + parentHashes.size()) + '\n';
    result += "author " + author + '\n';
    result += "timestamp " + std::to_string(timestamp) + '\n';
    result += "message " + message + '\n';
    result += "tree " + treeHash + '\n';
    for (auto &h : parentHashes)
      result += "parent " + h + '\n';
    return result;
  }

  void addParentHash(std::string &TreeHash) {
    parentHashes.push_back(TreeHash);
  }

  std::string getTreeHash() const { return treeHash; }
  std::string getMessage() const { return message; }

  void setAuthor(std::string auth) { author = auth; }
  void setTimeStamp(std::string tmstmp) {
    std::tm tm{};
    strptime(tmstmp.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    timestamp = mktime(&tm);
  }
  void addMessage(std::string msg) { message = msg;}
  void addTreeHash(std::string TrHash) {treeHash = TrHash;}
};
