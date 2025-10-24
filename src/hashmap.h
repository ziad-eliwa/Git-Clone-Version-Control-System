#pragma once
#include "vector.h"
#include <cstdint>
#include <iostream>
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

  class Iterator {
  public:
    using InnerIt = typename Vector<KeyValuePair<K, V>>::Iterator;
    using OuterIt = typename Vector<Vector<KeyValuePair<K, V>>>::Iterator;
    OuterIt outer_it;
    OuterIt outer_end;
    InnerIt inner_it;

    Iterator(OuterIt outer_it_, OuterIt outer_end_);

    KeyValuePair<K, V> &operator*();
    Iterator &operator++();
    void next_valid();
    bool operator!=(const Iterator &other) const;
  };
  HashMap<K, V>::Iterator begin();
  HashMap<K, V>::Iterator end();

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
  for (auto kvp : *this)
    newVector[Murmur3_32(kvp.key) & (capacity_ - 1)].push_back(kvp);

  // delete vector; // TODO: create deconstructor
  vector = newVector;
}

template <class K, class V> V &HashMap<K, V>::get(K key) {
  Vector<KeyValuePair<K, V>> *chain =
      &vector[Murmur3_32(key) & (capacity_ - 1)];
  for (int i = 0; i < chain->size(); i++) {
    if (chain->get(i).key == key)
      return chain->get(i).value;
  }
  size_++;
  if (load_factor() > MAX_LOAD_FACTOR) {
    grow();
    chain = &vector[Murmur3_32(key) & (capacity_ - 1)];
  }

  chain->push_back({key, V()});
  return chain->back().value;
}
template <class K, class V> V &HashMap<K, V>::operator[](K key) {
  return get(key);
}
template <class K, class V> void HashMap<K, V>::set(K key, V value) {
  get(key) = value;
}

template <class K, class V>
HashMap<K, V>::Iterator::Iterator(OuterIt o_it, OuterIt o_end)
    : outer_it(o_it), outer_end(o_end), inner_it(o_it->begin()) {
  next_valid();
}
template <class K, class V>
KeyValuePair<K, V> &HashMap<K, V>::Iterator::operator*() {
  return *inner_it;
}
template <class K, class V> void HashMap<K, V>::Iterator::next_valid() {
  while (outer_it != outer_end && inner_it == outer_it->end()) {
    outer_it++;
    if (outer_it != outer_end)
      inner_it = outer_it->begin();
  }
}
template <class K, class V>
typename HashMap<K, V>::Iterator &HashMap<K, V>::Iterator::operator++() {
  inner_it++;
  next_valid();
  return *this;
}
template <class K, class V>
bool HashMap<K, V>::Iterator::operator!=(const Iterator &other) const {
  return outer_it != other.outer_it ||
         (outer_it != outer_end && inner_it != other.inner_it);
}

template <class K, class V>
typename HashMap<K, V>::Iterator HashMap<K, V>::begin() {
  return HashMap<K, V>::Iterator(vector.begin(), vector.end());
}
template <class K, class V>
typename HashMap<K, V>::Iterator HashMap<K, V>::end() {
  return HashMap<K, V>::Iterator(vector.end(), vector.end());
};

template <class K, class V>
std::ostream &operator<<(std::ostream &os, HashMap<K, V> &hm) {
  os << '{';
  KeyValuePair<K, V> *prevKvp = nullptr;
  for (auto kvp : hm) {
    if (prevKvp != nullptr)
      os << prevKvp->key << ": " << prevKvp->value << ", ";
    prevKvp = &kvp;
  }
  if (prevKvp != nullptr)
    os << prevKvp->key << ": " << prevKvp->value;
  os << '}';
  return os;
}
