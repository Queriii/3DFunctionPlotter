#pragma once

#include <memory.h>
#include <cassert>



template <typename Data>
class List
{
public:
    List();
    ~List();


    Data& operator[](int iIndex);

    unsigned int Size() const;

    void Append(Data);

    bool RemoveLast();
    bool RemoveAll();

private:
    Data* pArray;
    unsigned int    uiNumElements;
    unsigned int    uiNumMaxElements;
    unsigned int    uiNumStartingElements;

    bool Resize();

    Data& Get(unsigned int uiIndex) const;
};




//Cnstrct & Dstrct
template <typename Data>
List<Data>::List()
    : pArray(nullptr), uiNumElements(0), uiNumMaxElements(0), uiNumStartingElements(4)
{
    assert(this->uiNumStartingElements != 0);
}

template <typename Data>
List<Data>::~List()
{
    if (this->pArray)
    {
        delete[] this->pArray;
    }
}




//Public
template <typename Data>
Data& List<Data>::operator[](int iIndex)
{
    assert(iIndex >= 0);
    return this->Get(static_cast<unsigned int>(iIndex));
}

template <typename Data>
unsigned int List<Data>::Size() const
{
    return this->uiNumElements;
}

template <typename Data>
void List<Data>::Append(Data D)
{
    if (this->uiNumElements == this->uiNumMaxElements)
    {
        if (!this->Resize())
        {
            //Throw
        }
    }

    this->pArray[this->uiNumElements] = D;
    this->uiNumElements++;
}

template <typename Data>
bool List<Data>::RemoveLast()
{
    if (!this->uiNumElements)
    {
        return false;
    }

    this->uiNumElements--;

    return true;
}

template <typename Data>
bool List<Data>::RemoveAll()
{
    if (!this->uiNumElements)
    {
        return false;
    }

    while (this->RemoveLast());

    return true;
}



//Private
template <typename Data>
bool List<Data>::Resize()
{
    if (!this->uiNumMaxElements)
    {
        this->uiNumMaxElements = this->uiNumStartingElements;
        this->pArray = new Data[this->uiNumMaxElements];
        if (!this->pArray)
        {
            //Set Exception Info
            return false;
        }
        memset(this->pArray, NULL, uiNumMaxElements * sizeof(Data));

        return true;
    }
    else
    {
        unsigned int uiNewNumMaxElements = this->uiNumMaxElements * 2;
        Data* pNewArray = new Data[uiNewNumMaxElements];
        if (!pNewArray)
        {
            //Set Exception Info
            return false;
        }
        memset(pNewArray, NULL, uiNewNumMaxElements * sizeof(Data));

        memcpy_s(pNewArray, uiNewNumMaxElements * sizeof(Data), this->pArray, this->uiNumElements * sizeof(Data));
        delete[] this->pArray;
        this->pArray = pNewArray;
        this->uiNumMaxElements = uiNewNumMaxElements;

        return true;
    }
}

template <typename Data>
Data& List<Data>::Get(unsigned int uiIndex) const
{
    assert(uiIndex < this->uiNumElements);
    return (this->pArray[uiIndex]);
}