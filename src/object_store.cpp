#include "gitobjects.h"
#include "object_store.h"
#include "vector.h"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

ObjectStore::ObjectStore(std::string sp) {
  std::filesystem::path store(sp);
  storePath = store.string();
  if (!std::filesystem::exists(store)) {
    if (!std::filesystem::create_directories(store))
      throw std::runtime_error("failed to create directory");
  }
};

blob ObjectStore::storeBlob(std::string filePath) {
  std::ifstream file(filePath, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

  blob b(filePath, content);
  std::ofstream object(storePath + "/" + b.getHash(),
                       std::ios::binary | std::ios::app);
  object << b.getContent();
  object.close();
  object.flush();
  return b;
};

void ObjectStore::store(std::string filepath, tree &t) {
  if (!std::filesystem::is_directory(filepath)) {
    blob b = storeBlob(filepath);
    t.addBlob(b);
  } else {
    for (const auto &entry : std::filesystem::directory_iterator(filepath)) {
      if (entry.is_directory()) {
        tree subtree;
        store(entry.path(), subtree);
        t.addSubTree(entry.path(), subtree);
      } else {
        blob b = storeBlob(entry.path());
        t.addBlob(b);
      }
    }
  }
  std::ofstream object(storePath + "/" + t.getHash(),
                       std::ios::binary | std::ios::app);
  object << t.serialize();
  object.close();
  object.flush();
}

blob ObjectStore::retrieveBlob(std::string &name, std::string &blobHash) {
  std::string filePath(".jit/objects/" + blobHash);
  std::ifstream file(filePath, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

  return blob(name,content);
}

tree ObjectStore::retrieveTree(std::string &treeHash) {
  std::string filePath(".jit/objects/" + treeHash);
  std::ifstream file(filePath, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  Vector<std::string> result;
  tree t;
  int start = 0;
  for (int i = 0; i < content.length(); ++i) {
    if (content[i] == '\0' || i == content.length() - 1) {
      result.push_back(content.substr(start, i - start));
      start = i + 1;
    }
  }

  for (int i = 1; i < result.size(); i += 2) {
    if (std::filesystem::is_directory(result[i])) {
      tree subtree = retrieveTree(result[i + 1]);
      t.addSubTree(result[i], subtree);
    } else {
      blob blb = retrieveBlob(result[i],result[i + 1]);
      t.addBlob(blb);
    }
  }
  return t;
}
