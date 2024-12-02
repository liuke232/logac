#include "cstr.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

thread_local static char cstr_buffer[MAX_CSTR_SIZE + 1] = {0};

CString* cstrInit(size_t n)
{
    CString* ret = (CString*)malloc(sizeof(size_t) + n + 1);
    ret->size = n;
    memset(ret->data, 0, n + 1);
    return ret;
}

CString* cstrCreate(const char* str)
{
    int str_len = strlen(str);
    int size = sizeof(size_t) + str_len + 1;
    CString* ret = (CString*)malloc(size);
    memcpy(ret->data, str, str_len + 1);
    return ret;
}

CString* cstrCopy(const CString* src)
{
    size_t size = sizeof(size_t) + src->size + 1;
    CString* ret = (CString*)malloc(size);
    memcpy(ret, src, size);
    return ret;
}

void cstrDelete(CString* cstr)
{
    free(cstr);
}

const char* cstrToCStr(const CString* cstr)
{
    return cstr->data;
}

char* cstrToStr(CString* cstr)
{
    return cstr->data;
}

CString* cstrVsprintf(const char* fmt, va_list args)
{
    int size = vsnprintf(cstr_buffer, MAX_CSTR_SIZE, fmt, args);
    CString* ret = cstrInit(size);
    memcpy(ret->data, cstr_buffer, size);
    return ret;
}

CString* cstrVsnprintf(size_t len, const char* fmt, va_list args)
{
    int size = vsnprintf(cstr_buffer, len, fmt, args);
    CString* ret = cstrInit(size);
    memcpy(ret->data, cstr_buffer, size);
    return ret;
}

CString* cstrSprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    CString* ret = cstrVsprintf(fmt, args);
    va_end(args);
    return ret;
}

CString* cstrSnprintf(size_t len, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    CString* ret = cstrVsnprintf(len, fmt, args);
    va_end(args);
    return ret;
}

