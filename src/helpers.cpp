#include "helpers.h"
#include "hashmap.h"

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
