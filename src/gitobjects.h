#pragma once
#include "hashmap.h"
#include "vector.h"
#include <ctime>
#include <string>

// Structure resource:
// https://www.freecodecamp.org/news/git-internals-objects-branches-create-repo/

class blob {
private:
  std::string name;
  std::string content;
  std::string hash;

public:
  blob() = default;
  blob(std::string name, std::string &content) : name(name), content(content) {
    hash = computeHash();
  }

  std::string computeHash() {
    return to_hex(Murmur3_32(serialize()));
  }

  std::string serialize() {
    return "blob\0" + name + '\0'+ std::to_string(content.length()) + '\0' + content;
  }

  std::string getContent() { return content; }
  std::string getHash() { return hash; }
  std::string getName() { return name; }
};

class tree {
private:
  struct treeEntry {
    std::string type;
    std::string hash;
    std::string name;
    treeEntry() = default;
    treeEntry(std::string t, std::string h, std::string n)
        : type(t), hash(h), name(n) {}
    // For sorting entries
    bool operator<(const treeEntry &other) const { return name < other.name; }
    std::string serialize() const { return name + '\0' + hash; }
  };
  Vector<treeEntry> entries;
  std::string hash;
  std::string name;

  void addEntry(const treeEntry &entry) {
    entries.push_back(entry);
    entries.sort(entries.begin(), entries.end());
    hash = computeHash();
  }
  std::string computeHash() { return to_hex(Murmur3_32(serialize())); }

public:
  std::string serialize() {
    std::string result = "";
    for (int i = 0; i < entries.size(); i++) {
      result += entries[i].serialize() + '\0';
    }
    return "tree\0" + std::to_string(entries.size()) + '\0' + result;
  }
  tree() = default;

  void addBlob(blob &b) {
    treeEntry te("blob", b.computeHash(), b.getName());
    addEntry(te);
  }

  void addSubTree(std::string dirname, tree &t) {
    t.name = dirname;
    treeEntry te("tree", t.hash, t.name);
    addEntry(te);
  }

  std::string getHash() const { return hash; }
  const Vector<treeEntry> &getEntries() const { return entries; }
};