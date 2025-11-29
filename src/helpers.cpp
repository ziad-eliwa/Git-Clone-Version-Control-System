#include "helpers.h"
#include "hashmap.h"
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

std::string standardPath(std::filesystem::path path) {
  if (path.empty())
    return ".";
  std::string p = path.generic_string();
  if (!(p.rfind("./", 0) == 0))
    p = "./" + p;
  return p;
}

std::string readFile(std::filesystem::path path) {
  std::ifstream file(path, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  return content;
}
