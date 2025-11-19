#include "hashmap.h"
#include "object_store.h"
#include "vector.h"
#include <filesystem>
#include <fstream>

class IndexStore {

private:
  ObjectStore &objectStore;
  std::string filePath;
  HashMap<std::string, std::string> fileContents;

public:
  IndexStore(std::string filePath, ObjectStore &objectStore)
      : objectStore(objectStore) {
    this->filePath = (filePath + "/index");
    load();
  }
  void add(std::string path) {
    if (std::filesystem::is_directory(path)) {
      for (const auto &entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_directory(entry.path())) {
            if (entry.path().filename() == ".jit") {
                continue;
            }
            std::cout << entry.path() << "\n";
            add(entry.path());
        } else {
          blob b = objectStore.storeBlob(entry.path());
          fileContents.set(entry.path(), b.getHash());
        }
      }
    } else {
      blob b = objectStore.storeBlob(path);
      fileContents.set(path, b.getHash());
    }
  }
  void save() {
    std::ofstream file;
    file.open(this->filePath);
    // status needs to be added;
    for (auto [path, hash] : fileContents) {
      file << path << " " << hash << std::endl;
    }
  }
  void load() {
    if (!std::filesystem::exists(filePath))
      return;
    std::ifstream in;
    in.open(this->filePath);
    std::string path, hash;
    while (in >> path >> hash) {
      fileContents.set(path, hash);
    }
  }

  tree convertToTree(){
    return tree();
  }

  void clearIndex() {
    std::ofstream f(filePath);
    f.close();
  }

  HashMap<std::string, std::string> &getStagedFiles() { return fileContents; }
};
