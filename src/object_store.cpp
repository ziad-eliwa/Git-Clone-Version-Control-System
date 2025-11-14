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
