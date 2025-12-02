#include "refs.h"
#include "helpers.h"
#include <filesystem>
#include <fstream>
#include <iostream>

Refs::Refs(std::filesystem::path r, std::filesystem::path h)
    : refsPath(r), headPath(h) {

  if (!std::filesystem::exists(refsPath)) {
    if (!std::filesystem::create_directories(refsPath))
      throw std::runtime_error("failed to create directory");
  }
}

std::string Refs::resolve(std::string ref) {
  if (ref.rfind("HEAD", 0) == 0) {
    // TODO: ^ @{n} ~
    std::string content = readFile(headPath);
    if (content.rfind("ref ", 0) == 0)
      return content.substr(4);
    return content;
  } else if (std::filesystem::exists(refsPath / ref)) {
    std::string content = readFile(refsPath / ref);
    if (content.rfind("ref ", 0) == 0)
      return content.substr(4);
    return content;
  }

  return ref;
}

std::string Refs::head() {
  std::string head = readFile(headPath);
  if (head.rfind("ref ", 0) == 0)
    return head.substr(4);
  return head;
}
void Refs::updateHead(std::string target) {
  std::ofstream r(headPath);
  r << "ref " << target;
}
void Refs::updateRef(std::string ref, std::string target) {
  if (target == "HEAD")
    target = resolve(target);
  std::cout << (refsPath / ref).string() << std::endl;
  std::ofstream r(refsPath / ref);
  if (std::filesystem::exists(refsPath / target))
    r << "ref " << target;
  else
    r << target;
}
