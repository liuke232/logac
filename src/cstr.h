#ifndef CSTR_H
#define CSTR_H

#include <stddef.h>
#include <stdarg.h>

#ifndef MAX_CSTR_SIZE
#define MAX_CSTR_SIZE 200000
#endif

typedef struct CString {
    size_t size;
    char data[];
} CString;


CString* cstrInit(size_t n);

CString* cstrCreate(const char* str);

CString* cstrCopy(const CString* src);

void cstrDelete(CString* cstr);

const char* cstrToCStr(const CString* cstr);

char* cstrToStr(CString* cstr);

CString* cstrVsprintf(const char* fmt, va_list args);

CString* cstrVsnprintf(size_t len, const char* fmt, va_list args);

CString* cstrSprintf(const char* fmt, ...);

CString* cstrSnprintf(size_t len, const char* fmt, ...);

#endif
