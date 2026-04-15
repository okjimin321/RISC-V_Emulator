#pragma once
#include <stddef.h>

void *malloc(unsigned int size);
void free(void *p);
void* calloc(unsigned int count, unsigned int size);
void* realloc(void* ptr, unsigned int size);

void exit(int code);
int system(const char* command);

int atoi(const char* s);
double atof(const char* s);
int abs(int x);

char* getenv(const char* name);

int remove(const char* path);
int rename(const char* oldpath, const char* newpath);
