#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#define UART_ADDR ((volatile uint32_t*)0x10000000)
#define FRAMEBUFFER ((volatile uint32_t*)0x20000000)
#define TIMER_MS (*(volatile uint32_t*)0x30000000)
#define KEY_VALUE (*(volatile uint32_t*)0x40000000)
#define KEY_STATUS (*(volatile uint32_t*)0x40000004)

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
