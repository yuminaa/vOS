#ifndef __KPORT_H
#define __KPORT_H

#include "../libk/kdef.h"

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

#endif