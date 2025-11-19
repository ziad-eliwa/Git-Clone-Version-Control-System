#pragma once
#include "gitobjects.h"
#include <string>

class ObjectStore {
private:
  std::string storePath;

public:
  ObjectStore() = default;
  ObjectStore(std::string storePath);
  blob storeBlob(std::string filePath);
  void store(std::string filePath, tree &t);
  tree retrieveTree(std::string &treeHash);
  blob retrieveBlob(std::string &name, std::string &blobHash);
};