#pragma once
#include <filesystem>
#include <string>

class Refs {
private:
  std::filesystem::path refsPath;
  std::filesystem::path headPath;

  bool isRef(std::string target);

public:
  Refs(std::filesystem::path refsPath, std::filesystem::path headPath);
  std::string resolve(std::string ref);

  std::string head();
  bool isOrphaned();
  void updateHead(std::string target, bool isRef = true);
  void updateRef(std::string ref, std::string target = "HEAD");
  void deleteRef(std::string ref);
};
