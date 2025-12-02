#pragma once

template<typename T>
class deque{
private:
    T *a = new T[1];
    int size, filled, start, end;
public:
    deque();

    bool empty();

    void push_back(T x);
    T pop_back();
    void push_front(T x);
    T pop_front();
};

template<typename T>
deque<T>::deque(){
    size = 1;
    filled = 0;
    start = 0;
    end = 0;
    a = new T[size];
}

template <class T> bool deque<T>::empty() { return filled == 0; }

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
    if (!empty()) {
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
    if (!empty()) {
        filled--;
        return a[start++];
    }
}
