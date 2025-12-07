#pragma once
#include "vector.h"

template <typename T> class Deque {
private:
  Vector<T> a;
  int filled, start, end;

  void grow();

public:
  Deque();

  bool empty();

  T front();
  T back();

  void push_back(T x);
  T pop_back();
  void push_front(T x);
  T pop_front();
};

template <typename T> Deque<T>::Deque() : a(), filled(0), start(0), end(0) {}

template <class T> void Deque<T>::grow() {
  Vector<T> newA(2 * a.capacity());
  for (int i = 0; i < a.size(); i++)
    newA[i] = a[(start + i) % a.size()];
  a = newA;
}

template <class T> bool Deque<T>::empty() { return filled == 0; }
template <class T> T Deque<T>::front() { return a[start]; }
template <class T> T Deque<T>::back() {
  return a[(end - 1 + a.size()) % a.size()];
}

template <typename T> void Deque<T>::push_back(T x) {
  if (filled == a.size())
    grow();

  a[end] = x;
  end = (end + 1) % a.size();
  filled++;
}

template <typename T> T Deque<T>::pop_back() {
  if (!empty()) {
    filled--;
    end = (end - 1 + a.size()) % a.size();
    return a[end];
  }
}

template <typename T> void Deque<T>::push_front(T x) {
  if (filled == a.size())
    grow();

  start = (start - 1 + a.size()) % a.size();
  a[start] = x;
  filled++;
}

template <typename T> T Deque<T>::pop_front() {
  if (!empty()) {
    filled--;
    T tmp = a[start];
    start = (start + 1) % a.size();
    return tmp;
  }
  return nullptr;
}
