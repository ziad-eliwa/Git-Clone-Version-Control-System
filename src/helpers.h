#pragma once
#include "vector.h"
#include <filesystem>
#include <string>

template <class T, class U> class Pair {
public:
  T first;
  U second;
  bool operator<(const Pair<T, U> &other) {
    return (first < other.first ||
            (first == other.first && second < other.second));
  }
};

Vector<std::string> split(std::string str, char delim);

std::string computeHash(std::string content);

std::filesystem::path repoRoot();

std::string pathString(std::filesystem::path path,
                       std::filesystem::path root = "");

std::string readFile(std::filesystem::path path);
void DEBUG();