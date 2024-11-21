// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#include "../port.h"

void outb(uint16_t port, uint8_t val) 
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

void io_wait()
{
    outb(0x80, 0);
}
