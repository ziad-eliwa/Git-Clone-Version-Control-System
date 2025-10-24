#pragma once
#include "vector.h"
#include <cstdint>
#include <ostream>
#include <type_traits>

// See https://en.wikipedia.org/wiki/MurmurHash
uint32_t Murmur3_32(const uint8_t *data, int length, uint32_t seed);
template <class T>
std::enable_if_t<std::is_integral_v<T>, uint32_t>
Murmur3_32(T data, uint32_t seed = 0) {
  return Murmur3_32(reinterpret_cast<const uint8_t *>(&data), sizeof(data),
                    seed);
}
uint32_t Murmur3_32(const char *data, uint32_t seed = 0);
uint32_t Murmur3_32(const std::string &data, uint32_t seed = 0);

template <class K, class V> class KeyValuePair {
public:
  K key;
  V value;
};

// Hash table using chaining
template <class K, class V> class HashMap {
private:
  int capacity_;
  int size_;
  Vector<Vector<KeyValuePair<K, V>>> vector;

  void grow();

public:
  const double MAX_LOAD_FACTOR = 1;

  HashMap();

  int size() const;
  int capacity() const;
  double load_factor() const;
  bool empty() const;

  V &get(K key);
  V &operator[](K key);
  void set(K key, V value);

  template <class KK, class VV>
  friend std::ostream &operator<<(std::ostream &os, HashMap<KK, VV> &hm);
};

template <class K, class V>
HashMap<K, V>::HashMap() : capacity_(1), size_(0), vector(capacity_) {}

template <class K, class V> int HashMap<K, V>::size() const { return size_; }
template <class K, class V> int HashMap<K, V>::capacity() const {
  return capacity_;
}
template <class K, class V> double HashMap<K, V>::load_factor() const {
  return static_cast<double>(size_) / capacity_;
}

template <class K, class V> void HashMap<K, V>::grow() {
  capacity_ <<= 1;
  Vector<Vector<KeyValuePair<K, V>>> newVector(capacity_);
  for (int i = 0; i < vector.size(); i++) {
    for (int j = 0; j < vector[i].size(); j++) {
      KeyValuePair<K, V> kvp = vector[i][j];
      newVector[Murmur3_32(kvp.key) & (capacity_ - 1)].push_back(kvp);
    }
  }
  // delete vector; // TODO: create deconstructor
  vector = newVector;
}

template <class K, class V> V &HashMap<K, V>::get(K key) {
  Vector<KeyValuePair<K, V>> &chain = vector[Murmur3_32(key) & (capacity_ - 1)];
  for (int i = 0; i < chain.size(); i++) {
    if (chain[i].key == key)
      return chain[i].value;
  }
  size_++;
  if (load_factor() > MAX_LOAD_FACTOR)
    grow();

  Vector<KeyValuePair<K, V>> &newChain =
      vector[Murmur3_32(key) & (capacity_ - 1)];
  newChain.push_back({key, V()});
  return newChain.back().value;
}
template <class K, class V> V &HashMap<K, V>::operator[](K key) {
  return get(key);
}
template <class K, class V> void HashMap<K, V>::set(K key, V value) {
  get(key) = value;
}

template <class K, class V>
std::ostream &operator<<(std::ostream &os, HashMap<K, V> &hm) {
  os << '{';
  KeyValuePair<K, V> *prevKvp = nullptr;
  for (int i = 0; i < hm.vector.size(); i++) {
    for (int j = 0; j < hm.vector[i].size(); j++) {
      if (prevKvp != nullptr)
        os << prevKvp->key << ": " << prevKvp->value << ", ";
      prevKvp = &hm.vector[i][j];
    }
  }
  if (prevKvp != nullptr)
    os << prevKvp->key << ": " << prevKvp->value;
  os << '}';
  return os;
}
