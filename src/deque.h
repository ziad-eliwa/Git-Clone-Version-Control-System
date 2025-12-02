#pragma once
#include "vector.h"

template <typename T> class deque {
private:
  Vector<T> a;
  int filled, start, end;

  void grow();

public:
  deque();

  bool empty();

  T front();
  T back();

  void push_back(T x);
  T pop_back();
  void push_front(T x);
  T pop_front();
};

template <typename T> deque<T>::deque() : a(), filled(0), start(0), end(0) {}

template <class T> void deque<T>::grow() {
  Vector<T> newA(2 * a.capacity());
  for (int i = start; i != end; i++, i %= a.size())
    newA[i] = a[i];
  a = newA;
}

template <class T> bool deque<T>::empty() { return filled == 0; }
template <class T> T deque<T>::front() { return a[start]; }
template <class T> T deque<T>::back() { return a[start]; }

template <typename T> void deque<T>::push_back(T x) {
  if (filled == a.size())
    grow();

  a[end++] = x;
  end %= a.size();
  filled++;
}

template <typename T> T deque<T>::pop_back() {
  if (!empty()) {
    filled--;
    return a[--end];
  }
}

template <typename T> void deque<T>::push_front(T x) {
  if (filled == a.size())
    grow();

  a[--start] = x;
  start %= a.size();
  filled++;
}

template <typename T> T deque<T>::pop_front() {
  if (!empty()) {
    filled--;
    return a[start++];
  }
}
