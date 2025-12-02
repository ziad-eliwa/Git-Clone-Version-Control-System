#include "refs.h"
#include "helpers.h"
#include "vector.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

Refs::Refs(std::filesystem::path r, std::filesystem::path h)
    : refsPath(r), headPath(h) {

  if (!std::filesystem::exists(refsPath)) {
    if (!std::filesystem::create_directories(refsPath))
      throw std::runtime_error("failed to create directory");
  }
}

bool Refs::isBranch(std::string s) {
  if (s.length() != 8)
    return true;

  for (char c : s) {
    if (std::string("0123456789abcdef").find(c) == std::string::npos)
      return true;
  }

  // HACK: reduce false positives
  if (std::filesystem::exists(refsPath / "../objects" / s))
    return false;
  return true;
}

std::string Refs::resolve(std::string ref) {
  if (ref.rfind("ref ", 0) == 0)
    return resolve(ref.substr(4));

  if (ref.rfind("HEAD", 0) == 0) {
    // TODO: ^ @{n} ~
    return resolve(readFile(headPath));
  } else if (isBranch(ref)) {
    if (std::filesystem::exists(refsPath / ref))
      return resolve(readFile(refsPath / ref));
    return "";
  }

  return ref;
}

bool Refs::isHeadBranch() { return readFile(headPath).rfind("ref ", 0) == 0; }
std::string Refs::getHead() {
  std::string h = readFile(headPath);
  if (h.rfind("ref ", 0) == 0)
    return h.substr(4);
  return h;
}
void Refs::updateHead(std::string target, bool r) {
  r &= isBranch(target);

  std::ofstream h(headPath);
  if (r)
    h << "ref ";
  h << target;
}
void Refs::updateRef(std::string ref, std::string target) {
  if (target == "HEAD")
    target = resolve(target);

  std::ofstream r(refsPath / ref);
  r << target;
}

Vector<std::string> Refs::getRefs() {
  Vector<std::string> refs;
  for (auto &e : std::filesystem::recursive_directory_iterator(refsPath)) {
    if (e.is_regular_file())
      refs.push_back(pathString(e, refsPath));
  }
  return refs;
}
