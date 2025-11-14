#include "hashmap.h"
#include <string>

class ObjectStore {
private:
  std::string storePath;

public:
  ObjectStore(std::string storePath);
  void store(std::string filePath);
};
