#include "../../Hdr/Data Structures/String.hpp"

#include <tchar.h>
#include <cassert>




//Cnstrct & dstrct
String::String(PCTSTR pctszString)
{
    assert(pctszString != nullptr);

    size_t ullStringLength = _tcslen(pctszString);
    this->ptszInternalString = new TCHAR[ullStringLength + 1]{};
    if (!this->ptszInternalString)
    {
        //raise
    }

    _tcscpy_s(this->ptszInternalString, ullStringLength + 1, pctszString);
}

String::~String()
{
    if (this->ptszInternalString)
    {
        delete[] this->ptszInternalString;
    }
    else
    {
        //raise
    }
}




//Public
PTSTR String::Get() const
{
    assert(this->ptszInternalString != nullptr);
    return this->ptszInternalString;
}

void String::Append(PCTSTR pctszString)
{
    assert(this->ptszInternalString != nullptr && pctszString != nullptr);

    size_t  ullOriginalStringLength = _tcslen(this->ptszInternalString);
    size_t  ullNewStringLength = ullOriginalStringLength + _tcslen(pctszString);
    PTSTR   ptszNewString = new TCHAR[ullNewStringLength + 1]{};
    _tcscpy_s(ptszNewString, ullNewStringLength + 1, this->ptszInternalString);
    _tcscat_s(ptszNewString, ullNewStringLength + 1, pctszString);

    delete[] this->ptszInternalString;
    this->ptszInternalString = ptszNewString;
}

int String::Find(PCTSTR pctszKeyword) const
{
    assert(this->ptszInternalString != nullptr && pctszKeyword != nullptr);

    for (UINT i = 0, j = 0; i < _tcslen(this->ptszInternalString); i++)
    {
        if (this->ptszInternalString[i] == pctszKeyword[j])
        {
            j++;
            if (j == _tcslen(pctszKeyword))
            {
                return ((i + 1) - j);
            }
        }
        else
        {
            j = 0;
        }
    }

    return -1;
}

bool String::Replace(PCTSTR pctszReplaceMe, PCTSTR pctszNewWord)
{
    assert(this->ptszInternalString != nullptr && pctszReplaceMe != nullptr && pctszNewWord != nullptr);

    int iBase = this->Find(pctszReplaceMe);
    if (iBase == -1)
    {
        return false;
    }

    size_t  ullNewStringLength = _tcslen(this->ptszInternalString) - _tcslen(pctszReplaceMe) + _tcslen(pctszNewWord);
    PTSTR   ptszNewString = new TCHAR[ullNewStringLength + 1]{};

    for (int i = 0; i < iBase; i++)
    {
        ptszNewString[i] = this->ptszInternalString[i];
    }
    _tcscat_s(ptszNewString, ullNewStringLength + 1, pctszNewWord);

    for (int i = iBase + static_cast<int>(_tcslen(pctszNewWord)), j = iBase + static_cast<int>(_tcslen(pctszReplaceMe)); i < ullNewStringLength; i++, j++)
    {
        ptszNewString[i] = this->ptszInternalString[j];
    }
    ptszNewString[ullNewStringLength] = __TEXT('\0');

    delete[] this->ptszInternalString;
    this->ptszInternalString = ptszNewString;

    return true;
}

void String::ReplaceAll(PCTSTR pctszReplaceMe, PCTSTR pctszNewWord)
{
    assert(this->ptszInternalString != nullptr && pctszReplaceMe != nullptr && pctszNewWord != nullptr);

    while (this->Replace(pctszReplaceMe, pctszNewWord));
}

void String::Lower()
{
    for (UINT i = 0; i < _tcslen(this->ptszInternalString); i++)
    {
        if (this->ptszInternalString[i] >= __TEXT('A') && this->ptszInternalString[i] <= __TEXT('Z'))
        {
            this->ptszInternalString[i] += 0x20;
        }
    }
}

void String::Upper()
{
    for (UINT i = 0; i < _tcslen(this->ptszInternalString); i++)
    {
        if (this->ptszInternalString[i] >= __TEXT('a') && this->ptszInternalString[i] <= __TEXT('z'))
        {
            this->ptszInternalString[i] -= 0x20;
        }
    }
}