#pragma once
#include "vector.h"
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
