#include "hashmap.h"
#include "gitobjects.h"
#include <string>

class ObjectStore {
private:
  std::string storePath;

public:
  ObjectStore(std::string storePath);
  void store(std::string filePath);
  void storeTree(std::string filePath,tree &t);
};
