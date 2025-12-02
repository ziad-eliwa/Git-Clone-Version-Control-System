#pragma once
#include "gitobjects.h"
#include <string>

class ObjectStore {
private:
  std::string storePath;

public:
  ObjectStore(std::string storePath);
  void store(GitObject *obj);
  GitObject *store(std::string filePath);
  GitObject *retrieve(std::string hash);
  // std::string retrieveLog(std::string lastHash);
  void reconstruct(std::string treeHash, std::string path);
};
