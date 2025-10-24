#pragma once
#include "vector.h"
#include <cstdint>

// https://en.wikipedia.org/wiki/MurmurHash
uint32_t Murmur3_32(const uint8_t *data, int length, uint32_t seed = 0);
template <class T> uint32_t Murmur3_32(T data, uint32_t seed = 0);
uint32_t Murmur3_32(const char *data, uint32_t seed = 0);
uint32_t Murmur3_32(const std::string &data, uint32_t seed = 0);

// Open addressing hash table
template <class K, class V> class HashMap {
private:
  Vector<V> vector;
  int capacity_;
  int size_;

public:
  HashMap();

  int size();
  int capcaity();
  bool empty();
};
