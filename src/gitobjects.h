#pragma once
#include "helpers.h"
#include "vector.h"
#include <ctime>
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
    hash = computeHash(serialize());
  }

  void addSubTree(std::string treeName, Tree &t) {
    TreeEntry te("tree", treeName, t.getHash());
    addEntry(te);
    hash = computeHash(serialize());
  }

  const Vector<TreeEntry> &getEntries() const { return entries; }
};

class Commit : public GitObject {
private:
  std::string treeHash;
  std::string author;
  std::string timestamp;
  Vector<std::string> parentHashes;
  std::string message;

public:
  Commit() = default;
  Commit(std::string msg, std::string author, std::string treeHash)
      : message(msg), treeHash(treeHash), author(author) {
    time_t stamp = std::time(nullptr);
    std::tm *tm_ptr = std::localtime(&stamp);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d,%H:%M:%S", tm_ptr);
    timestamp = buf;
    hash = computeHash(serialize());
  }

  std::string serialize() override {
    std::string result =
        "commit " + std::to_string(4 + parentHashes.size()) + '\n';
    result += "author " + author + '\n';
    result += "timestamp " + timestamp + '\n';
    result += "message " + message + '\n';
    result += "tree " + treeHash + '\n';
    for (auto &h : parentHashes)
      result += "parent " + h + '\n';
    return result;
  }

  std::string getTreeHash() const { return treeHash; }
  std::string getMessage() const { return message; }
  std::string getAuthor() const { return author; }
  std::string getTimeStamp() const { return timestamp; }
  Vector<std::string> getParentHashes() { return parentHashes; }

  void addParentHash(std::string &TreeHash) {
    parentHashes.push_back(TreeHash);
    hash = computeHash(serialize());
  }

  void setAuthor(std::string auth) {
    author = auth;
    hash = computeHash(serialize());
  }
  void setTimeStamp(std::string tmstmp) {
    timestamp = tmstmp;
    hash = computeHash(serialize());
  }
  void addMessage(std::string msg) {
    message = msg;
    hash = computeHash(serialize());
  }
  void addTreeHash(std::string TrHash) {
    treeHash = TrHash;
    hash = computeHash(serialize());
  }
};
