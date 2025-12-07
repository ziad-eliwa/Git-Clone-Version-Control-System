#include "helpers.h"
#include "hashmap.h"
#include <filesystem>
#include <fstream>
#include <string>

Vector<std::string> split(std::string str, char delim) {
  Vector<std::string> ret;
  int l = 0;
  for (int r = 0; r < str.length(); r++) {
    if (str[r] == delim) {
      ret.push_back(str.substr(l, r - l));
      l = r + 1;
    }
  }
  if (l < str.length())
    ret.push_back(str.substr(l));
  return ret;
}

std::string computeHash(std::string content) {
  return to_hex(Murmur3_32(content));
}

std::filesystem::path repoRoot() {
  std::filesystem::path d = std::filesystem::current_path();
  while (d.parent_path() != d) {
    if (std::filesystem::exists(d / ".jit"))
      return d / ".jit";
    d = d.parent_path();
  }
  if (std::filesystem::exists(d / ".jit"))
    return d / ".jit";
  throw std::runtime_error("no jit repository found.\n  (use 'jit init' to "
                           "initialize a new jit repository.)");
};

std::string pathString(std::filesystem::path path, std::filesystem::path root) {
  if (root.empty())
    root = repoRoot().parent_path();

  if (path.empty())
    return ".";
  std::string p = std::filesystem::relative(path, root).generic_string();
  if ((p.rfind("./", 0) == 0))
    p = p.substr(2);
  return p;
}

std::string readFile(std::filesystem::path path) {
  std::ifstream file(path, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  return content;
}

void DEBUG() { std::cout << "DEBUG\n"; }
