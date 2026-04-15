#pragma once

#include <stdarg.h>
#include <stddef.h>

typedef struct FILE FILE;

extern FILE* stdout;
extern FILE* stderr;

#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int printf(const char* fmt, ...);
int snprintf(char* buf, unsigned int size, const char* fmt, ...);
int vsnprintf(char* buf, unsigned int size, const char* fmt, va_list ap);


int putchar(int c);
int puts(const char* s);

// TODO (Make file system)
FILE* fopen(const char* path, const char* mode);
int fclose(FILE* stream);
unsigned int fread(void* ptr, unsigned int size, unsigned int nmemb, FILE* stream);
unsigned int fwrite(const void* ptr, unsigned int size, unsigned int nmemb, FILE* stream);
int fseek(FILE* stream, long offset, int whence);
long ftell(FILE* stream);

int sscanf(const char* str, const char* fmt, ...);

int fprintf(FILE* stream, const char* fmt, ...);
int vfprintf(FILE* stream, const char* fmt, va_list ap);
int fflush(FILE* stream);
