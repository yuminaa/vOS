#include "../memory.h"

/**
 * Fill a region of memory with a repeated byte value
 * @s: Pointer to the destination memory area
 * @c: Byte value to be set
 * @count: Number of bytes to fill
 * @return: A pointer to the memory area s
 */
void *memset(void *s, int c, size_t count)
{
    uint8_t *xs = s;
    uint8_t b = c;
    if (count < 8) 
    {
        while (count--)
            *xs++ = b;
        return s;
    }
    
    // Fill first few bytes to align to word boundary
    size_t align = -(uintptr_t)xs & (sizeof(size_t)-1);
    count -= align;
    while (align--)
        *xs++ = b;
        
    // Create word-sized pattern
    size_t pattern = (b << 24) | (b << 16) | (b << 8) | b;
    pattern = (pattern << 32) | pattern;
    
    size_t *xw = (size_t *)xs;
    
    // Fill by words
    while (count >= sizeof(size_t)) 
    {
        *xw++ = pattern;
        count -= sizeof(size_t);
    }
    
    // Fill the remaining bytes
    xs = (uint8_t *)xw;
    while (count--)
        *xs++ = b;
        
    return s;
}

/**
 * Copy memory area
 * @dest: Pointer to the destination memory area
 * @src: Pointer to the source memory area
 * @count: Number of bytes to copy
 * @return: A pointer to dest
 * Note: The memory areas must not overlap
 */
void *memcpy(void *dest, const void *src, size_t count)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    
    // Handle small copies directly
    if (count < 8) 
    {
        while (count--)
            *d++ = *s++;
        return dest;
    }
    
    // Copy bytes until aligned
    size_t align = -(uintptr_t)d & (sizeof(size_t)-1);
    count -= align;
    while (align--)
        *d++ = *s++;
        
    // Copy by words
    size_t *dw = (size_t *)d;
    const size_t *sw = (const size_t *)s;
    
    while (count >= sizeof(size_t)) 
    {
        *dw++ = *sw++;
        count -= sizeof(size_t);
    }
    
    // Copy remaining bytes
    d = (uint8_t *)dw;
    s = (const uint8_t *)sw;
    while (count--)
        *d++ = *s++;
        
    return dest;
}

/**
 * Compare memory areas
 * @cs: Pointer to the first memory area
 * @ct: Pointer to the second memory area
 * @count: Number of bytes to compare
 * @return: <0 if cs < ct, 0 if cs == ct, >0 if cs > ct
 */
int memcmp(const void *cs, const void *ct, size_t count)
{
    const uint8_t *s1 = cs;
    const uint8_t *s2 = ct;

    if (count < 8) 
    {
        while (count--)
        {
            if (*s1 != *s2)
                return *s1 - *s2;
            s1++;
            s2++;
        }
        return 0;
    }
    
    // Align to word boundary
    size_t align = -(uintptr_t)s1 & (sizeof(size_t)-1);
    count -= align;
    while (align--) 
    {
        if (*s1 != *s2)
            return *s1 - *s2;
        s1++;
        s2++;
    }
    
    const size_t *w1 = (const size_t *)s1;
    const size_t *w2 = (const size_t *)s2; 
    while (count >= sizeof(size_t)) 
    {
        if (*w1 != *w2) 
        {
            s1 = (const uint8_t *)w1;
            s2 = (const uint8_t *)w2;
            for (size_t i = 0; i < sizeof(size_t); i++) 
            {
                if (s1[i] != s2[i])
                    return s1[i] - s2[i];
            }
        }
        w1++;
        w2++;
        count -= sizeof(size_t);
    }
    
    /* Compare remaining bytes */
    s1 = (const uint8_t *)w1;
    s2 = (const uint8_t *)w2;
    while (count--) 
    {
        if (*s1 != *s2)
            return *s1 - *s2;
        s1++;
        s2++;
    }
    
    return 0;
}