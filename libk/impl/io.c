// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#include "../io.h"
#include "../memory.h"
#include "../string.h"

#define VGA_MEMORY ((uint16_t*)0xFFFFFFFF800B8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_BLACK 0
#define VGA_LIGHT_GREY 7
#define VGA_WHITE 15

#define VGA_COLOR(fg, bg) ((bg << 4) | fg)
#define VGA_ENTRY(c, color) ((uint16_t)(c) | ((uint16_t)(color) << 8))

#define MAX_NUMBER_LENGTH 32
#define PRINT_BUF_SIZE 1024

static const char hex_chars[] = "0123456789ABCDEF";
static const char hex_chars_lower[] = "0123456789abcdef";

static size_t vga_row = 0;
static size_t vga_col = 0;
static uint8_t vga_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
static uint16_t* vga_buffer = VGA_MEMORY;

static void vga_scroll() 
{
    memcpy(vga_buffer, 
           vga_buffer + VGA_WIDTH,
           (VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(uint16_t));
    
    for (size_t x = 0; x < VGA_WIDTH; x++)
        vga_buffer[(VGA_HEIGHT-1) * VGA_WIDTH + x] = VGA_ENTRY(' ', vga_color);
    
    vga_row--;
}

void vga_putchar(const char c)
{
    switch (c) 
    {
        case '\n':
            vga_col = 0;
            vga_row++;
            break;
        case '\r':
            vga_col = 0;
            break;
        case '\t':
            vga_col = (vga_col + 8) & ~7;
            break;
        case '\b':
            if (vga_col > 0) 
                vga_col--;
            break;
        default:
        {
            size_t index = vga_row * VGA_WIDTH + vga_col;
            vga_buffer[index] = VGA_ENTRY(c, vga_color);
            vga_col++;
            break;
        }
    }

    if (vga_col >= VGA_WIDTH)
    {
        vga_col = 0;
        vga_row++;
    }

    if (vga_row >= VGA_HEIGHT)
        vga_scroll();
}

void putc(char c)
{
    vga_putchar(c);
}

void puts(const char* str)
{
    while (*str)
        putc(*str++);
    putc('\n');
}

static void utoa(uint64_t value, char* buf, int base, bool uppercase)
{
    char tmp[MAX_NUMBER_LENGTH];
    char* ptr = tmp;
    const char* chars = uppercase ? hex_chars : hex_chars_lower;

    if (value == 0) 
    {
        *buf++ = '0';
        *buf = '\0';
        return;
    }

    do 
    {
        *ptr++ = chars[value % base];
        value /= base;
    } 
    while (value);

    while (ptr > tmp)
        *buf++ = *--ptr;
    *buf = '\0';
}

static void itoa(int64_t value, char* buf, int base)
{
    if (value < 0) 
    {
        *buf++ = '-';
        utoa(-value, buf, base, true);
    } 
    else 
    {
        utoa(value, buf, base, true);
    }
}

int printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = 0;
    char num_buf[MAX_NUMBER_LENGTH];

    while (*format)
    {
        if (*format != '%') 
        {
            putc(*format++);
            ret++;
            continue;
        }

        format++;
        bool is_long_long = false;
        if (*format == 'l') 
        {
            format++;
            if (*format == 'l') 
            {
                is_long_long = true;
                format++;
            }
        }
       
        switch (*format) 
        {
            case 'c': 
            {
                char c = va_arg(args, int);
                putc(c);
                ret++;
                break;
            }
            case 's':
            {
                char* s = va_arg(args, char*);
                if (!s) s = "(null)";
                while (*s) 
                {
                    putc(*s++);
                    ret++;
                }
                break;
            }
            case 'd':
            case 'i': 
            {
                int64_t num;
                if (is_long_long) 
                {
                    num = va_arg(args, int64_t);
                } 
                else 
                {
                    num = va_arg(args, int);
                }
                itoa(num, num_buf, 10);
                char* s = num_buf;
                while (*s) 
                {
                    putc(*s++);
                    ret++;
                }
                break;
            }
            case 'u': 
            {
                uint64_t num;
                if (is_long_long) 
                {
                    num = va_arg(args, uint64_t);
                } 
                else 
                {
                    num = va_arg(args, unsigned int);
                }
                utoa(num, num_buf, 10, false);
                char* s = num_buf;
                while (*s) 
                {
                    putc(*s++);
                    ret++;
                }
                break;
            }
            case 'x':
            case 'X': 
            {
                uint64_t num;
                if (is_long_long) 
                {
                    num = va_arg(args, uint64_t);
                } 
                else 
                {
                    num = va_arg(args, unsigned int);
                }
                putc('0');
                putc('x');
                ret += 2;
                
                utoa(num, num_buf, 16, *format == 'X');
                if (is_long_long) 
                {
                    size_t len = strlen(num_buf);
                    while (len < 16)
                    {
                        putc('0');
                        ret++;
                        len++;
                    }
                }
                
                char* s = num_buf;
                while (*s) 
                {
                    putc(*s++);
                    ret++;
                }
                break;
            }
            case 'p': 
            {
                void* ptr = va_arg(args, void*);
                putc('0');
                putc('x');
                ret += 2;
                utoa((uint64_t)ptr, num_buf, 16, true);
                size_t len = strlen(num_buf);
                while (len < 16) 
                {
                    putc('0');
                    ret++;
                    len++;
                }
                char* s = num_buf;
                while (*s) 
                {
                    putc(*s++);
                    ret++;
                }
                break;
            }
            case '%':
                putc('%');
                ret++;
                break;
            default:
                putc('%');
                putc(*format);
                ret += 2;
                break;
        }
        format++;
    }

    va_end(args);
    return ret;
}

int snprintf(char* buf, size_t size, const char* fmt, ...)
{
    if (!buf || size == 0)
        return 0;
    
    va_list args;
    va_start(args, fmt);
    size_t pos = 0;
    char num_buf[MAX_NUMBER_LENGTH];

    while (*fmt && pos < size - 1)
    {
        if (*fmt != '%') 
        {
            buf[pos++] = *fmt++;
            continue;
        }

        fmt++; // Skip '%'
        switch (*fmt) 
        {
            case 'c': 
            {
                char c = va_arg(args, int);
                if (pos < size - 1)
                    buf[pos++] = c;
                break;
            }
            case 's': 
            {
                char* s = va_arg(args, char*);
                if (!s) 
                    s = "(null)";
                while (*s && pos < size - 1)
                    buf[pos++] = *s++;
                break;
            }
            case 'd':
            case 'i': 
            {
                int64_t num = va_arg(args, int64_t);
                itoa(num, num_buf, 10);
                char* s = num_buf;
                while (*s && pos < size - 1)
                    buf[pos++] = *s++;
                break;
            }
            case 'u': 
            {
                uint64_t num = va_arg(args, uint64_t);
                utoa(num, num_buf, 10, false);
                char* s = num_buf;
                while (*s && pos < size - 1)
                    buf[pos++] = *s++;
                break;
            }
            case 'x':
            case 'X': 
            {
                uint64_t num = va_arg(args, uint64_t);
                utoa(num, num_buf, 16, *fmt == 'X');
                char* s = num_buf;
                while (*s && pos < size - 1)
                    buf[pos++] = *s++;
                break;
            }
            case 'p': 
            {
                void* ptr = va_arg(args, void*);
                if (pos < size - 2) 
                {
                    buf[pos++] = '0';
                    buf[pos++] = 'x';
                    utoa((uint64_t)ptr, num_buf, 16, true);
                    // Pad with 0s
                    size_t len = strlen(num_buf);
                    while (len < 16 && pos < size - 1) 
                    {
                        buf[pos++] = '0';
                        len++;
                    }
                    char* s = num_buf;
                    while (*s && pos < size - 1)
                        buf[pos++] = *s++;
                }
                break;
            }
            case '%':
                if (pos < size - 1)
                    buf[pos++] = '%';
                break;
            default:
                if (pos < size - 2) 
                {
                    buf[pos++] = '%';
                    buf[pos++] = *fmt;
                }
                break;
        }
        fmt++;
    }
    
    buf[pos] = '\0';
    va_end(args);
    return pos;
}

void print_hex(uint64_t value)
{
    char buf[MAX_NUMBER_LENGTH];
    utoa(value, buf, 16, true);
    puts(buf);
}

void print_dec(int64_t value)
{
    char buf[MAX_NUMBER_LENGTH];
    itoa(value, buf, 10);
    puts(buf);
}

void print_ptr(void* ptr)
{
    char buf[MAX_NUMBER_LENGTH];
    putc('0');
    putc('x');
    utoa((uint64_t)ptr, buf, 16, true);
    // Pad with 0s
    // Check if its actually 16 characters
    size_t len = strlen(buf);
    while (len < 16) 
    {
        putc('0');
        len++;
    }
    puts(buf);
}