#pragma once

#include "List.hpp"

#include <cassert>




template <typename Data>
class Stack
{
public:
    Stack(Stack&) = delete;
    Stack(Stack&&) = delete;

    Stack();

    void Push(Data D);
    Data Pop();
    Data Peek();

    unsigned int Size();

private:
    List<Data> InternalList;
};



template <typename Data>
Stack<Data>::Stack()
{

}


template <typename Data>
void Stack<Data>::Push(Data D)
{
    this->InternalList.Append(static_cast<Data>(D));
}

template <typename Data>
Data Stack<Data>::Pop()
{
    assert(InternalList.Size() > 0);

    Data Output = this->InternalList[this->InternalList.Size() - 1];
    this->InternalList.RemoveLast();

    return Output;
}

template <typename Data>
Data Stack<Data>::Peek()
{
    assert(InternalList.Size() > 0);

    Data Output = this->InternalList[this->InternalList.Size() - 1];

    return Output;
}

template <typename Data>
unsigned int Stack<Data>::Size()
{
    return this->InternalList.Size();
}