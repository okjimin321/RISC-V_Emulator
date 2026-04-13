#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#define UART_ADDR ((volatile uint32_t*)0x10000000)
#define FRAMEBUFFER ((volatile uint32_t*)0x20000000)
#define TIMER_MS (*(volatile uint32_t*)0x30000000)
#define KEY_VALUE (*(volatile uint32_t*)0x40000000)
#define KEY_STATUS (*(volatile uint32_t*)0x40000004)

int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, unsigned int n);
int strcasecmp(const char* a, const char* b);
int strncasecmp(const char* a, const char* b, unsigned int n);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
char* strncpy(char* dst, const char* src, unsigned int n);
char* strdup(const char* s);

int printf(const char* args, ...);
int putchar(int c);
int puts(const char* s);
int snprintf(char* buf, unsigned int size, const char* fmt, ...);
int vsnprintf(char* buf, unsigned int size, const char* fmt, va_list ap);

void *memset(void *dst, int value, unsigned int n);
void *memcpy(void *dst, const void *src, unsigned int n);

unsigned int strlen(const char *s);

void *malloc(unsigned int size);
void free(void *p);
void* calloc(unsigned int count, unsigned int size);
void* realloc(void* ptr, unsigned int size);

void exit(int code);
int system(const char* command);
int remove(const char* path);
int rename(const char* oldpath, const char* newpath);

int atoi(const char* s);
int abs(int x);
