//
// Created by Ali Ahmed  on 18/10/2025.
//

#ifndef STACK_STACK_H
#define STACK_STACK_H
template <class T>

class Stack {
    int top;
    int capacity;
    int *arr;
    public:
    Stack(int);
    ~Stack();
    bool isEmpty();
    bool isFull();
    void push(T);
    int pop();
};


#endif //STACK_STACK_H