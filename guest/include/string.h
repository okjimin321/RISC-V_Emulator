#pragma once

#include <stddef.h>

unsigned int strlen(const char *s);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, unsigned int n);
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, unsigned int n);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
char* strncpy(char* dst, const char* src, unsigned int n);
char* strdup(const char* s);

void *memset(void *dst, int value, unsigned int n);
void *memcpy(void *dst, const void *src, unsigned int n);
void *memmove(void *dst, const void *src, unsigned int n);
int memcmp(const void *a, const void *b, unsigned int n);
