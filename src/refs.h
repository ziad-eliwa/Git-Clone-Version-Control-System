#pragma once
#include <filesystem>
#include <string>

class Refs {
private:
  std::filesystem::path refsPath;
  std::filesystem::path headPath;

public:
  Refs(std::filesystem::path refsPath, std::filesystem::path headPath);
  std::string resolve(std::string ref);

  std::string head();
  void updateHead(std::string ref);
  void updateRef(std::string ref, std::string hash = "HEAD");
  void deleteRef(std::string ref);
};
