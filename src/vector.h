#pragma once
#include <initializer_list>
#include <iostream>
#include <ostream>

// Dynamic array
template <class T> class Vector {
private:
  int capacity_;
  int size_;
  T *array;

  void grow();

public:
  Vector();
  Vector(int capacity);
  Vector(std::initializer_list<T> list);
  Vector(const Vector<T> &other);
  ~Vector();

  int size() const;
  int capacity() const;
  bool empty() const;

  T &get(int index);
  T &operator[](int index);
  void set(int index, T item);

  void push_back(T item);
  T pop_back();

  using Iterator = T *;
  Iterator begin();
  Iterator end();

  T &back();
  const T &back() const;
  T &front();
  const T &front() const;

  Vector<T> &operator=(const Vector<T> &other);

  template <class U>
  friend std::ostream &operator<<(std::ostream &os, Vector<U> &v);
};

template <class T> Vector<T>::Vector() : capacity_(1), size_(0) {
  array = new T[capacity_];
}
template <class T> Vector<T>::Vector(int size) : size_(size) {
  capacity_ = 1;
  while (capacity_ < size)
    capacity_ <<= 1;
  array = new T[capacity_];
  for (int i = 0; i < size; i++)
    array[i] = T();
}
template <class T> Vector<T>::Vector(std::initializer_list<T> list) {
  capacity_ = 1;
  while (capacity_ < list.size())
    capacity_ <<= 1;
  array = new T[capacity_];

  int i = 0;
  for (T item : list)
    array[i++] = item;
  size_ = i;
}
template <class T>
Vector<T>::Vector(const Vector<T> &other) : Vector(other.size()) {
  for (int i = 0; i < other.size(); i++)
    array[i] = other[i];
}
template <class T> Vector<T> &Vector<T>::operator=(const Vector<T> &other) {
  if (this == &other)
    return *this;

  delete[] array;
  capacity_ = other.capacity_, size_ = other.size_;
  array = new T[capacity_];
  for (int i = 0; i < other.size(); i++)
    array[i] = other.array[i];
  return *this;
}
template <class T> Vector<T>::~Vector() {
  delete[] array;
  array = nullptr;
  size_ = 0;
  capacity_ = 0;
}

template <class T> int Vector<T>::size() const { return size_; }
template <class T> int Vector<T>::capacity() const { return capacity_; }
template <class T> bool Vector<T>::empty() const { return size_ == 0; }

template <class T> void Vector<T>::grow() {
  capacity_ <<= 1;
  T *newArray = new T[capacity_];
  for (int i = 0; i < size_; i++)
    newArray[i] = array[i];
  delete[] array;
  array = newArray;
}
template <class T> void Vector<T>::push_back(T item) {
  if (size_ + 1 > capacity_)
    grow();
  array[size_++] = item;
}
template <class T> T Vector<T>::pop_back() {
  if (empty())
    throw std::underflow_error("pop_back: cannot pop from empty Vector");
  return array[--size_];
}

template <class T> T &Vector<T>::get(int index) {
  if (!(0 <= index && index < size_))
    throw std::out_of_range("get: index out of range");
  return array[index];
}
template <class T> T &Vector<T>::operator[](int index) { return get(index); }
template <class T> void Vector<T>::set(int index, T item) { get(index) = item; }
template <class T> T *Vector<T>::begin() { return array; }
template <class T> T *Vector<T>::end() { return array + size_; }

template <class T> T &Vector<T>::back() { return array[size_ - 1]; }
template <class T> const T &Vector<T>::back() const { return array[size_ - 1]; }
template <class T> T &Vector<T>::front() { return array[0]; }
template <class T> const T &Vector<T>::front() const { return array[0]; }

template <class T> std::ostream &operator<<(std::ostream &os, Vector<T> &v) {
  os << '[';
  for (int i = 0; i < v.size() - 1; i++)
    os << v[i] << ", ";
  if (!v.empty())
    os << v.back();
  os << ']';
  return os;
};
