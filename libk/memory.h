// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#ifndef __KMEMORY_H__
#define __KMEMORY_H__

#include "kdef.h"

void* memset(void *s, int c, size_t count);
void* memcpy(void *dest, const void *src, size_t count);
void* memmove(void *dest, const char *src, size_t count);
int memcmp(const void *cs, const void *ct, size_t count);

#endif