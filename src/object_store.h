#pragma once
#include "gitobjects.h"
#include <string>

class ObjectStore {
private:
  std::string storePath;

public:
  ObjectStore() = default;
  ObjectStore(std::string storePath);
  void store(GitObject *obj);
  GitObject *store(std::string filePath);
  GitObject *retrieve(std::string hash);
  std::string retrieveLog(std::string lastHash);
  std::string retrieveHead(std::string headPath);
  void storeHead(std::string hash, std::string headPath);
  void reconstruct(std::string treeHash, std::string path);
};
