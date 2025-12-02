#pragma once
#include "gitobjects.h"
#include <string>

class ObjectStore {
private:
  std::filesystem::path storePath;

public:
  ObjectStore(std::filesystem::path storePath);
  void store(GitObject *obj);
  GitObject *store(std::string filePath);
  bool exists(std::string hash);
  GitObject *retrieve(std::string hash);
  std::string retrieveLog(std::string lastHash);
  void reconstruct(std::string treeHash, std::filesystem::path path);
};
