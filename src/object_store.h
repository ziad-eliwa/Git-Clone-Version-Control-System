#include "gitobjects.h"
#include <string>

class ObjectStore {
private:
  std::string storePath;

public:
  ObjectStore(std::string storePath);
  blob storeBlob(std::string filePath);
  void store(std::string filePath,tree &t);
};
