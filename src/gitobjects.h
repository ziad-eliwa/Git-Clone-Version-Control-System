#pragma once
#include "hashmap.h"
#include "vector.h"
#include <ctime>
#include <string>

// Structure resource:
// https://www.freecodecamp.org/news/git-internals-objects-branches-create-repo/

class blob {
private:
  std::string mode;
  std::string name;
  std::string content;
  std::string hash;

public:
  blob(std::string &data) : content(data) { hash = computeHash(); }

  std::string computeHash() {
    return std::to_string(Murmur3_32(serialize().c_str()));
  }

  std::string serialize() {
    return "blob " + std::to_string(content.length()) + '\0' + content;
  }

  std::string getContent() { return content; }
  std::string getHash() { return hash; }
  std::string getName() { return name; }
  std::string getMode() { return mode; }
};

class tree {
private:
  struct treeEntry {
    std::string mode;
    std::string type;
    std::string hash;
    std::string name;

    treeEntry(const std::string &m, const std::string &t, const std::string &h,
              const std::string &n)
        : mode(m), type(t), hash(h), name(n) {}
    // For sorting entries
    bool operator<(const treeEntry &other) const { return name < other.name; }
    std::string serialize() const { return mode + " " + name + '\0' + hash; }
  };
  Vector<treeEntry> entries;
  std::string hash;

  void addEntry(const treeEntry &entry) {
    entries.push_back(entry);
    entries.sort(entries.begin(), entries.end());
    hash = computeHash();
  }
  std::string serialize() {
    std::string result;
    for (treeEntry *i = entries.begin(); i != entries.end(); i++) {
      result += (*i).serialize() + "\0";
    }
    return "tree " + std::to_string(result.size()) + result;
  }
  std::string computeHash() { return std::to_string(Murmur3_32(serialize())); }

public:
  tree() = default;

  void addBlob(blob &b) {
    treeEntry te(b.getMode(), "blob", b.computeHash(), b.getName());
    addEntry(te);
  }

  void addSubTree(std::string dirName, tree &t) {
    treeEntry te("040000", "tree", t.hash, dirName);
    addEntry(te);
  }

  std::string getHash() const { return hash; }
  const Vector<treeEntry> &getEntries() const { return entries; }
};