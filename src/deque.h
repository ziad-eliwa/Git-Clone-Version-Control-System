#ifndef DEQUE_H
#define DEQUE_H

template<typename T>
class deque{
private:
    T *a = new T[1];
    int size, filled, start, end;
public:
    deque();
    void push_back(T x);
    T pop_back();
    void push_front(T x);
    T pop_front();
};

#endif DEQUE_H