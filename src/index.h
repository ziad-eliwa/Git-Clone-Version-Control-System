#pragma once
#include "gitobjects.h"
#include "hashmap.h"
#include "helpers.h"
#include "objectstore.h"
#include "vector.h"
#include <filesystem>
#include <fstream>

using IndexEntry = Pair<std::string, std::string>; // path hash

class IndexStore {
private:
  ObjectStore &objectStore;
  std::string indexPath;
  HashMap<std::string, std::string> indexEntries;

public:
  IndexStore(std::string filePath, ObjectStore &objectStore)
      : objectStore(objectStore) {
    this->indexPath = filePath;
    load();
  }
  void add(std::string path) {
    auto p = std::filesystem::path(path);
    if (p.filename() == ".jit")
      return;

    if (std::filesystem::is_directory(p)) {
      for (const auto &entry : std::filesystem::directory_iterator(p))
        add(standardPath(entry));
    } else {
      GitObject *obj = objectStore.store(p.relative_path());
      indexEntries.set(p.relative_path(), obj->getHash());
    }
  }
  void save() {
    Vector<IndexEntry> entries;
    for (auto &[path, hash] : indexEntries)
      entries.push_back({path, hash});
    entries.sort();

    std::ofstream file(indexPath);
    // status needs to be added;
    for (auto [path, hash] : entries)
      file << path << " " << hash << std::endl;
  }
  void load() {
    if (!std::filesystem::exists(indexPath))
      return;
    std::ifstream in(indexPath);
    std::string path, hash;
    while (in >> path >> hash)
      indexEntries.set(standardPath(path), hash);
  }

  Tree _writeTree(HashMap<std::string, Vector<IndexEntry>> &dirMap,
                  std::string &path) {
    Tree ret;

    for (auto &entry : dirMap[path]) {
      std::string p = entry.first;
      int lastSlash = p.rfind('/');
      std::string fileName =
          (lastSlash == std::string::npos) ? p : p.substr(lastSlash + 1);
      ret.addEntry(TreeEntry{"blob", fileName, entry.second});
    }

    for (auto &[p, e] : dirMap) {
      if (p.rfind(path, 0) == 0 &&
          path.size() + 1 < p.length()) { // p.startswith(path)
        std::string remainingPath = p.substr(path.size() + 1);
        int firstSlash = remainingPath.find('/');
        std::string dirName = (firstSlash == std::string::npos)
                                  ? remainingPath
                                  : remainingPath.substr(0, firstSlash);
        if (firstSlash == std::string::npos) { // base case
          Tree subTree = _writeTree(dirMap, p);
          objectStore.store(&subTree);
          ret.addEntry(TreeEntry{"tree", dirName, subTree.getHash()});
        }
      }
    }

    return ret;
  }
  Tree writeTree() {
    Vector<IndexEntry> entries;
    for (auto &[path, hash] : indexEntries)
      entries.push_back({path, hash});
    entries.sort();

    HashMap<std::string, Vector<IndexEntry>> dirMap;

    for (auto &entry : entries) {
      std::string path = entry.first;
      int lastSlash = path.rfind('/');
      std::string dirName =
          (lastSlash == std::string::npos) ? "" : path.substr(0, lastSlash);
      dirMap[dirName].push_back(entry);
    }

    std::string path = ".";
    return _writeTree(dirMap, path);
  }
  void readTree(std::string path, std::string hash) {
    GitObject *obj = objectStore.retrieve(hash);
    if (Blob *b = dynamic_cast<Blob *>(obj)) {
      indexEntries.set(std::filesystem::relative(path), hash);
    } else if (Tree *t = dynamic_cast<Tree *>(obj)) {
      for (auto &e : t->getEntries())
        readTree(path + '/' + e.name, e.getHash());
    }
  }

  void clearIndex() {
    std::ofstream f(indexPath);
    f.close();
  }

  HashMap<std::string, std::string> getEntries() { return indexEntries; }
};
