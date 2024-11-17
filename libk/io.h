// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#ifndef __KIO_H__
#define __KIO_H__

#include "kdef.h"
#include "memory.h"
#include "string.h"

typedef __builtin_va_list va_list;
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

int printf(const char* format, ...);
int snprintf(char* buf, size_t size, const char* fmt, ...);

void putc(char c);
void puts(const char* str);

void print_hex(uint64_t value);
void print_dec(int64_t value);
void print_ptr(void* ptr);

void vga_putchar(char c);

#endif