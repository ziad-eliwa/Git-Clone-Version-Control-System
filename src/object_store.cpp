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

void ObjectStore::store(std::string filePath) {
  std::ifstream file(filePath, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

  uint32_t hash =
      Murmur3_32(filePath + std::to_string(content.size()) + content);

  std::ofstream object(storePath + "/" + to_hex(hash), std::ios::binary);
  object << content;
};

void ObjectStore::storeTree(std::string filepath, tree &t) {
  for (const auto &entry : std::filesystem::directory_iterator(filepath)) {
    if (entry.is_directory()) {
      tree subtree;
      storeTree(entry.path(), subtree);
      t.addSubTree(entry.path().filename().string(), subtree);
    } else {
      std::ifstream file(entry.path(), std::ios::binary);
      std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());

      blob b(entry.path().filename().string(), content);
      t.addBlob(b);
      std::ofstream object(storePath + "/" + b.getHash(), std::ios::binary);
      object << b.getContent();
    }
  }
  std::ofstream object(storePath + "/" + t.getHash(), std::ios::binary);
  object << t.serialize();
}
