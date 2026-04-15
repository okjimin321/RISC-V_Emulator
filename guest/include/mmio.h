#pragma once
#include <stdint.h>

#define UART_ADDR ((volatile uint32_t*)0x10000000)
#define FRAMEBUFFER ((volatile uint32_t*)0x20000000)
#define TIMER_MS (*(volatile uint32_t*)0x30000000)
#define KEY_VALUE (*(volatile uint32_t*)0x40000000)
#define KEY_STATUS (*(volatile uint32_t*)0x40000004)

// TODO
#define WAD_ADDR ((const unsigned char*)0x00800000)
#define WAD_SIZE 0x00500000