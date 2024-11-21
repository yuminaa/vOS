// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#include "../timer.h"
#include "../init.h"
#include "../../libk/io.h"

static volatile uint64_t timer_ticks = 0;

void init_timer(uint32_t frequency)
{
    apic_write(APIC_TIMER_DIV, 0x3);
    apic_write(APIC_TIMER_INIT, 0x100000);
    apic_write(APIC_LVT_TIMER, (1 << 17) | 32);
}

void timer_tick()
{
    apic_write(APIC_EOI, 0);
}

uint64_t get_ticks()
{
    return timer_ticks;
}