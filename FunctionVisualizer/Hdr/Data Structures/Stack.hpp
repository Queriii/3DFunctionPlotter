#pragma once

#include "List.hpp"



template <typename Data>
class Stack
{
public:
    unsigned int Size() const;

    void Push(Data D);
    Data Pop();
    Data Peek();

private:
    List<Data> InternalList;
};



//Public
template <typename Data>
unsigned int Stack<Data>::Size() const
{
    return this->InternalList.Size();
}

template <typename Data>
void Stack<Data>::Push(Data D)
{
    this->InternalList.Append(D);
}

template <typename Data>
Data Stack<Data>::Pop()
{
    assert(this->Size() > 0);
    Data D = this->Peek();
    this->InternalList.RemoveLast();
    return D;
}

template <typename Data>
Data Stack<Data>::Peek()
{
    assert(this->Size() > 0);
    return (this->InternalList[this->Size() - 1]);
}