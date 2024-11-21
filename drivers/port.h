// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#ifndef __KPORT_H__
#define __KPORT_H__

#include "../libk/kdef.h"

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void io_wait();

#endif