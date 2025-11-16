#include "stack.h"
using namespace std;
template<class T>
Stack <T>::Stack(int cap) {
    capacity = cap;
    arr = new int[cap];
    top = -1;
}
template<class T>
Stack <T>::~Stack() {

}
template<class T>
bool Stack <T>::isEmpty() {
    if (top==-1) return true;
    else return false;
}
template<class T>
bool Stack <T>::isFull() {
    if (top==capacity-1) return true;
    else return false;
}
template<class T>
void Stack <T>::push(T x) {
    arr[++top] = x;

}
template<class T>
int Stack <T>::pop() {
    return arr[top--];
}