#define ONES     (~0UL/0xFF)
#define HIGHS    (ONES * 0x80)
#define HAS_ZERO(x) (((x) - ONES) & ~(x) & HIGHS)

#include "../string.h"

/**
 * strlen - Find the length of a string
 * @str: The string to be measured
 *
 * The strlen() function calculates the length of the string pointed 
 * to by str, excluding the terminating null byte ('\0').
 *
 * Return: The length of @str
 */
size_t strlen(const char *str) 
{
    const char *s = str;
    size_t *ls;

    while ((size_t)s & (sizeof(long) - 1)) 
    {
        if (!*s) 
            return s - str;
        s++;
    }

    ls = (size_t *)s;
    while (!HAS_ZERO(*ls)) 
        ls++;

    s = (const char *)ls;
    while (*s) 
        s++;

    return s - str;
}

/**
 * strnlen - Find the length of a length-limited string
 * @s: The string to be measured
 * @maxlen: The maximum number of bytes to check
 *
 * The strnlen() function returns the number of bytes in the string 
 * pointed to by @s, excluding the terminating null byte ('\0'), 
 * but at most maxlen bytes.
 *
 * Return: The lesser of maxlen or the length of @s
 */
size_t strnlen(const char *s, size_t maxlen)
{
    const char *es = s;
    // Count until either null or maxlen reached
    while (*es && maxlen) 
    {
        es++;
        maxlen--;
    }
    return es - s;
}

/**
 * strcpy - Copy a string
 * @dest: Destination buffer
 * @src: Source string
 *
 * The strcpy() function copies the string pointed to by src, 
 * including the terminating null byte ('\0'), to the buffer 
 * pointed to by dest.
 *
 * Return: A pointer to the destination string @dest
 */
char *strcpy(char *dest, const char *src) 
{
    char *ret = dest;
    size_t *ldest, *lsrc;
    
    while ((size_t)src & (sizeof(long) - 1)) 
    {
        if (!(*dest = *src)) 
            return ret;
        dest++; src++;
    }
    
    ldest = (size_t *)dest;
    lsrc = (size_t *)src;
    while (!HAS_ZERO(*lsrc)) 
        *ldest++ = *lsrc++;
    
    dest = (char *)ldest;
    src = (char *)lsrc;
    while ((*dest++ = *src++) != '\0');
    
    return ret;
}

/**
 * strncpy - Copy a length-limited string
 * @dest: Destination buffer
 * @src: Source string
 * @n: Maximum number of bytes to copy
 *
 * The strncpy() function copies at most n bytes from the string 
 * pointed to by src to the buffer pointed to by dest. If the length 
 * of src is less than n, strncpy() writes additional null bytes to 
 * dest to ensure that a total of n bytes are written.
 *
 * Return: A pointer to the destination string @dest
 */
char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;

    // First copy up to n bytes from src
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    
    // Then pad with nulls if necessary
    for (; i < n; i++)
        dest[i] = '\0';

    return dest;
}

/**
 * strlcpy - Copy a string with size bound and ensure NUL-termination
 * @dest: Destination buffer
 * @src: Source string
 * @size: Size of destination buffer
 *
 * The strlcpy() function copies at most size-1 bytes from the string 
 * pointed to by src to the buffer pointed to by dest, always 
 * NUL-terminating the result.
 *
 * Return: The total length of the string tried to create
 *         (length of @src)
 */
size_t strlcpy(char *dest, const char *src, size_t size)
{
    const char *s = src;
    size_t n = size;

    // Copy as many bytes as will fit
    if (n != 0 && --n != 0) {
        do {
            if ((*dest++ = *s++) == 0)
                break;
        } while (--n != 0);
    }

    // Not enough room in dest, add NUL and traverse rest of src
    if (n == 0) 
    {
        if (size != 0)
            *dest = '\0';  // NULL-terminate dest
        while (*s++);
    }

    return s - src - 1;  // count does not include NULL
}

/**
 * strcat - Concatenate two strings
 * @dest: Destination buffer
 * @src: Source string
 *
 * The strcat() function appends the string pointed to by src to 
 * the end of the string pointed to by dest. The terminating null 
 * byte ('\0') at the end of dest is overwritten.
 *
 * Return: A pointer to the destination string @dest
 */
char *strcat(char *dest, const char *src)
{
    char *tmp = dest;

    // Find the end of dest
    while (*tmp)
        tmp++;
    
    // Copy src to end of dest
    while ((*tmp++ = *src++) != '\0')
        ;
        
    return dest;
}

/**
 * strncat - Concatenate a length-limited string
 * @dest: Destination buffer
 * @src: Source string
 * @n: Maximum number of bytes to copy
 *
 * The strncat() function appends at most n bytes from the string 
 * pointed to by src to the end of the string pointed to by dest. 
 * A terminating null byte ('\0') is always appended to the result.
 *
 * Return: A pointer to the destination string @dest
 */
char *strncat(char *dest, const char *src, size_t n)
{
    char *tmp = dest;

    // Find the end of dest
    while (*tmp)
        tmp++;

    // Copy up to n bytes from src
    while (n-- > 0 && (*tmp = *src) != '\0') 
    {
        tmp++;
        src++;
    }

    *tmp = '\0';  // Always null terminate
    return dest;
}

/**
 * strlcat - Concatenate a string with size bound
 * @dest: Destination buffer
 * @src: Source string
 * @size: Size of destination buffer
 *
 * The strlcat() function appends the string pointed to by src to 
 * the end of the string pointed to by dest up to a maximum size 
 * given by size. Always NUL-terminates the result.
 *
 * Return: The total length of the string tried to create
 */
size_t strlcat(char *dest, const char *src, size_t size)
{
    const char *s = src;
    char *d = dest;
    size_t n = size;
    size_t dlen;

    // Find the end of dest and adjust n
    while (n-- != 0 && *d != '\0')
        d++;
    dlen = d - dest;
    n = size - dlen;

    if (n == 0)
        return dlen + strlen(s);

    while (*s != '\0') 
    {
        if (n != 1) 
        {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';

    return dlen + (s - src);
}

/**
 * strcmp - Compare two strings
 * @s1: First string
 * @s2: Second string
 *
 * The strcmp() function compares the two strings s1 and s2.
 *
 * Return: negative if s1 < s2, 0 if equal, positive if s1 > s2
 */
int strcmp(const char *s1, const char *s2) 
{
    size_t l1;
    size_t l2;
    while ((size_t)s1 & (sizeof(long) - 1)) 
    {
        if (*s1 != *s2)
            return (unsigned char)*s1 - (unsigned char)*s2;
        if (!*s1)
            return 0;
        s1++; s2++;
    }
    
    while (1) 
    {
        l1 = *(size_t *)s1;
        l2 = *(size_t *)s2;
        
        if (HAS_ZERO(l1) || l1 != l2) 
        {
            break;
        }
        s1 += sizeof(long);
        s2 += sizeof(long);
    }
    
    while (1) 
    {
        if (*s1 != *s2)
            return (unsigned char)*s1 - (unsigned char)*s2;
        if (!*s1)
            return 0;
        s1++; 
        s2++;
    }
}

/**
 * strncmp - Compare a length-limited string
 * @s1: First string
 * @s2: Second string
 * @n: Maximum bytes to compare
 *
 * The strncmp() function compares at most the first n bytes of 
 * the two strings s1 and s2.
 *
 * Return: negative if s1 < s2, 0 if equal, positive if s1 > s2
 */
int strncmp(const char *s1, const char *s2, size_t n)
{
    unsigned char c1;
    unsigned char c2;
    while (n)
    {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 != c2)
            return c1 - c2;
        if (!c1)
            break;
        n--;
    }
    return 0;
}

/**
 * strcasecmp - Compare two strings ignoring case
 * @s1: First string
 * @s2: Second string
 *
 * The strcasecmp() function compares the two strings s1 and s2, 
 * ignoring case differences.
 *
 * Return: negative if s1 < s2, 0 if equal, positive if s1 > s2
 */
int strcasecmp(const char *s1, const char *s2)
{
    unsigned char c1, c2;

    do 
    {
        c1 = (*s1++ | 0x20);
        c2 = (*s2++ | 0x20);
        if (c1 == '\0')
            return c1 - c2;
    }
    while (c1 == c2);

    return c1 - c2;
}

/**
 * strncasecmp - Compare a length-limited string ignoring case
 * @s1: First string
 * @s2: Second string
 * @len: Maximum bytes to compare
 *
 * The strncasecmp() function compares the two strings s1 and s2, 
 * ignoring case differences, comparing at most len bytes.
 *
 * Return: negative if s1 < s2, 0 if equal, positive if s1 > s2
 */
int strncasecmp(const char *s1, const char *s2, size_t len)
{
    unsigned char c1, c2;

    if (!len)
        return 0;

    do 
    {
        c1 = (*s1++ | 0x20);
        c2 = (*s2++ | 0x20);
        if (!--len)
            return c1 - c2;
        if (c1 == '\0')
            return c1 - c2;
    } 
    while (c1 == c2);

    return c1 - c2;
}

/**
 * strchr - Locate character in string
 * @s: String to search
 * @c: Character to find
 *
 * The strchr() function returns a pointer to the first occurrence 
 * of the character c in the string s.
 *
 * Return: pointer to the matched character or NULL if not found
 */
char *strchr(const char *s, int c)
{
    for (; *s != (char)c; s++)
        if (*s == '\0')
            return NULL;
    return (char *)s;
}

/**
 * strrchr - Locate character in string from the end
 * @s: String to search
 * @c: Character to find
 *
 * The strrchr() function returns a pointer to the last occurrence 
 * of the character c in the string s.
 *
 * Return: pointer to the matched character or NULL if not found
 */
char *strrchr(const char *s, int c)
{
    const char *last = NULL;

    do 
    {
        if (*s == (char)c)
            last = s;
    } 
    while (*s++);

    return (char *)last;
}

/**
 * strnchr - Locate character in length-limited string
 * @s: String to search
 * @count: Maximum bytes to search
 * @c: Character to find
 *
 * The strnchr() function returns a pointer to the first occurrence 
 * of the character c in the string s, where not more than count 
 * bytes are searched.
 *
 * Return: pointer to the matched character or NULL if not found
 */
char *strnchr(const char *s, size_t count, int c)
{
    while (count--) 
    {
        if (*s == (char)c)
            return (char *)s;
        if (*s++ == '\0')
            break;
    }
    return NULL;
}

/**
 * strnstr - Locate a substring in a length-limited string
 * @s1: String to search in (haystack)
 * @s2: String to search for (needle)
 * @len: Maximum bytes to search
 *
 * The strnstr() function locates the first occurrence of the 
 * null-terminated string s2 in the string s1, where not more 
 * than len bytes are searched.
 *
 * Return: pointer to the located substring or NULL if not found
 */
char *strnstr(const char *s1, const char *s2, size_t len)
{
    size_t l2 = strlen(s2);
    
    if (!l2)
        return (char *)s1;

    while (len >= l2) 
    {
        size_t i;
        bool match = true;
        
        for (i = 0; i < l2; i++)
        {
            if (s1[i] != s2[i]) 
            {
                match = false;
                break;
            }
        }
        
        if (match)
            return (char *)s1;
            
        s1++;
        len--;
    }
    return NULL;
}

/**
 * strstr - Locate a substring
 * @haystack: String to search in
 * @needle: String to search for
 *
 * The strstr() function finds the first occurrence of the substring 
 * needle in the string haystack. The terminating null bytes ('\0') 
 * are not compared.
 *
 * Return: pointer to the beginning of substring or NULL if not found
 */
char *strstr(const char *haystack, const char *needle)
{
    const char first = *needle;
    if (first == '\0')
        return (char *)haystack;

    while (*haystack) 
    {
        if (*haystack == first) 
        {
            const char *h = haystack + 1;
            const char *n = needle + 1;
            
            while (*n && *h == *n) 
            {
                h++;
                n++;
            }
            
            if (*n == '\0')
                return (char *)haystack;
        }
        haystack++;
    }
    
    return NULL;
}