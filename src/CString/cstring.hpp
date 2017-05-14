#ifndef CSTRING_HPP_INCLUDED
#define CSTRING_HPP_INCLUDED

#include <cstdio>
#include <cstring>
#include <stdarg.h>

class CString
{
public:
    CString();
    CString(const char *DefaultString);
    CString(const CString &DefaultString);

    ~CString();

    operator char* ();

    CString& operator = (const char *NewString);
    CString& operator = (const CString &NewString);
    CString& operator += (const char *NewString);
    CString& operator += (const CString &NewString);

    friend CString operator + (const CString &String1, const char *String2);
    friend CString operator + (const char *String1, const CString &String2);
    friend CString operator + (const CString &String1, const CString &String2);

    void Append(const char *Format, ...);
    void Set(const char *Format, ...);
    void Empty();

protected:
    char *String;
};

#endif /* End of 'cstring.hpp' file */