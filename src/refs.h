#pragma once
#include "vector.h"
#include <filesystem>
#include <string>

class Refs {
private:
  std::filesystem::path refsPath;
  std::filesystem::path headPath;
  std::filesystem::path mergeHeadPath;

public:
  Refs(std::filesystem::path refsPath, std::filesystem::path headPath,
       std::filesystem::path mergeHead = "");
  std::string resolve(std::string ref);

  bool isBranch(std::string target);
  std::string getHead();
  std::string getMergeHead();
  bool isHeadBranch();
  void updateHead(std::string target, bool isRef = true);
  void updateMergeHead(std::string target, bool isRef = true);
  void updateRef(std::string ref, std::string target = "HEAD");
  void deleteRef(std::string ref);
  Vector<std::string> getRefs();
};
