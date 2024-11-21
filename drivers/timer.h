#ifndef __KTIMER_H__
#define __KTIMER_H__

#include "../libk/kdef.h"

#define APIC_TIMER_DIV  0x3E0
#define APIC_TIMER_INIT 0x380
#define APIC_LVT_TIMER  0x320
#define APIC_EOI        0xB0

void init_timer(uint32_t frequency);
uint64_t get_ticks();
void timer_tick();

#endif