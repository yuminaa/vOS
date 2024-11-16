// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#ifndef __KSTRING_H__
#define __KSTRING_H__

#include "kdef.h"

size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);

// Copy
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t count);
size_t strlcpy(char *dest, const char *src, size_t size);

// Concatenation 
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t count);
size_t strlcat(char *dest, const char *src, size_t size);  // Safer

// Comparison
int strcmp(const char *cs, const char *ct);
int strncmp(const char *cs, const char *ct, size_t count);
int strcasecmp(const char *s1, const char *s2);  // Case insensitive
int strncasecmp(const char *s1, const char *s2, size_t len);

// Search
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strnchr(const char *s, size_t count, int c);
char *strstr(const char *s1, const char *s2);
char *strnstr(const char *s1, const char *s2, size_t len);

#endif