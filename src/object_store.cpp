#include "object_store.h"
#include <filesystem>
#include <fstream>
#include <iterator>

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
  std::ofstream object(storePath + "/" + b.getHash(), std::ios::binary|std::ios::app);
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
        t.addSubTree(entry.path().filename().string(), subtree);
      } else {
        blob b = storeBlob(entry.path());
        t.addBlob(b);
      }
    }
  }
  std::ofstream object(storePath + "/" + t.getHash(), std::ios::binary|std::ios::app);
  object << t.serialize();
  object.close();
  object.flush();
}
