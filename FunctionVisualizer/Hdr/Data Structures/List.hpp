#pragma once

#include <Windows.h>
#include <cassert>




template <typename Data>
using _RefComparisonCallback = bool(*)(Data& A, Data& B);
template <typename Data>
using _ComparisonCallback = bool(*)(Data A, Data B);

template <typename Data>
class List
{
public:
    List(List&) = delete;
    List(List&&) = delete;

    List();
    ~List();


    enum DynamicElementType
    {
        None,
        Single,
        Array
    };
    void SetCleanupDynamicElementsType(DynamicElementType Type);

    unsigned int    Size() const;

    void    Append(Data& D);
    void    Append(Data D);

    void RemoveLast();

    unsigned int Suffices(_RefComparisonCallback<Data> pfnCheck, Data& B);
    unsigned int Suffices(_ComparisonCallback<Data> pfnCheck, Data B);

    Data& operator[](int iIndex);

private:
    Data* pArray;
    unsigned int    uiNumElements;
    unsigned int    uiCurrentElementMax;

    List::DynamicElementType DynamicType;


    bool Resize();

    Data& Get(unsigned int uiIndex) const;
};



//Cnstrct & Dstrct
template <typename Data>
List<Data>::List()
    : uiNumElements(NULL), uiCurrentElementMax(4), DynamicType(List::DynamicElementType::None)
{
    this->pArray = new Data[this->uiCurrentElementMax]{};
    if (!(this->pArray))
    {
        return;
    }
}

template <typename Data>
List<Data>::~List()
{
    if (this->DynamicType != 0)
    {
        for (UINT i = 0; i < this->uiNumElements; i++)
        {
            (this->DynamicType == List::DynamicElementType::Single) ? delete reinterpret_cast<void*>(this->pArray[i]) : delete[] reinterpret_cast<void*>(this->pArray[i]);
        }
    }

    if (this->pArray)
    {
        delete[] this->pArray;
    }
}




//Public
template <typename Data>
void List<Data>::SetCleanupDynamicElementsType(DynamicElementType Type)
{
    this->DynamicType = Type;
}

template <typename Data>
unsigned int List<Data>::Size() const
{
    return this->uiNumElements;
}

template <typename Data>
void List<Data>::Append(Data& D)
{
    if (this->uiNumElements >= this->uiCurrentElementMax)
    {
        if (!(this->Resize()))
        {
            assert(1 == 0);
            return;
        }
    }

    memcpy_s(this->pArray + this->uiNumElements, this->uiCurrentElementMax * sizeof(Data), &D, sizeof(Data));

    this->uiNumElements++;
}

template <typename Data>
void List<Data>::Append(Data D)
{
    if (this->uiNumElements >= this->uiCurrentElementMax)
    {
        if (!(this->Resize()))
        {
            assert(1 == 0);
            return;
        }
    }

    memcpy_s(this->pArray + this->uiNumElements, this->uiCurrentElementMax * sizeof(Data), &D, sizeof(Data));

    this->uiNumElements++;
}

template <typename Data>
void List<Data>::RemoveLast()
{
    assert(this->uiNumElements != 0);
    this->uiNumElements--;
}

template <typename Data>
unsigned int List<Data>::Suffices(_RefComparisonCallback<Data> pfnCheck, Data& B)
{
    if (!pfnCheck)
    {
        return -1;
    }

    for (unsigned int i = 0; i < this->uiNumElements; i++)
    {
        if (pfnCheck(this->Get(i), B))
        {
            return i;
        }
    }

    return -1;
}

template <typename Data>
unsigned int List<Data>::Suffices(_ComparisonCallback<Data> pfnCheck, Data B)
{
    if (!pfnCheck)
    {
        return -1;
    }

    for (unsigned int i = 0; i < this->uiNumElements; i++)
    {
        if (pfnCheck(this->Get(i), B))
        {
            return i;
        }
    }

    return -1;
}


template <typename Data>
Data& List<Data>::operator[](int iIndex)
{
    return (this->Get(iIndex));
}




//Private
template <typename Data>
bool List<Data>::Resize()
{
    assert(this->uiCurrentElementMax != 0);
    this->uiCurrentElementMax *= 2;

    Data* pNewArray = new Data[this->uiCurrentElementMax]{};
    if (!pNewArray)
    {
        return false;
    }
    memcpy_s(pNewArray, this->uiCurrentElementMax * sizeof(Data), this->pArray, this->uiNumElements * sizeof(Data));

    if (this->pArray)
    {
        delete[] this->pArray;
    }
    this->pArray = pNewArray;

    return true;
}


template <typename Data>
Data& List<Data>::Get(unsigned int uiIndex) const
{
    assert(uiIndex < uiNumElements);

    return (this->pArray[uiIndex]);
}