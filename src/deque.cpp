#include <deque.h>

template<typename T>
deque<T>::deque(){
    size = 1;
    filled = 0;
    start = 0;
    end = 0;
    a = new T[size];
}

template<typename T>
void deque<T>::push_back(T x){
    if (filled == size) {
        T *b = new T[size * 2];
        for (int i = start; i != end; i++, i %= size) {
            b[i] = a[i];
        }
        delete[] a;
        a = b;
        size *= 2;
    }
    a[end++] = x;
    end %= size;
    filled++;
}

template<typename T>
T deque<T>::pop_back(){
    if (filled > 0) {
        filled--;
        return a[--end];
    }
}

template<typename T>
void deque<T>::push_front(T x){
    if (filled == size) {
        T *b = new T[size * 2];
        for (int i = start; i != end; i++, i %= size) {
            b[i] = a[i];
        }
        delete[] a;
        a = b;
        size *= 2;
    }
    a[--start] = x;
    start %= size;
    filled++;
}

template<typename T>
T deque<T>::pop_front(){
    if (filled > 0) {
        filled--;
        return a[start++];
    }
}