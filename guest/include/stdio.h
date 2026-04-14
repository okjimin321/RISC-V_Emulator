#pragma once

#include <stdarg.h>

typedef struct FILE FILE;

extern FILE* stdout;
extern FILE* stderr;

#define EOF (-1)

int printf(const char* fmt, ...);
int snprintf(char* buf, unsigned int size, const char* fmt, ...);
int vsnprintf(char* buf, unsigned int size, const char* fmt, va_list ap);
int fprintf(FILE* stream, const char* fmt, ...);
int vfprintf(FILE* stream, const char* fmt, va_list ap);
int fflush(FILE* stream);
int putchar(int c);
int puts(const char* s);
