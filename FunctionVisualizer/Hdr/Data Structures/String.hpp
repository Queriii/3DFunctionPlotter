#pragma once

#include <Windows.h>



class String
{
public:
    String(String&) = delete;
    String(String&&) = delete;

    String(PCTSTR pctszString);
    ~String();


    PTSTR Get() const;

    void Append(PCTSTR pctszString);

    int     Find(PCTSTR pctszKeyword) const;
    bool    Replace(PCTSTR pctszReplaceMe, PCTSTR pctszNewWord);
    void    ReplaceAll(PCTSTR pctszReplaceMe, PCTSTR pctszNewWord);

    void Lower();
    void Upper();

private:
    PTSTR ptszInternalString;
};