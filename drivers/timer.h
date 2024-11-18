#ifndef __KTIMER_H__
#define __KTIMER_H__

#include "../libk/kdef.h"

#define PIT_CHANNEL0    0x40
#define PIT_COMMAND     0x43
#define PIT_FREQUENCY   1193180
#define PIT_SET_COMMAND 0x36

void init_timer(uint32_t frequency);
uint64_t get_ticks();
void timer_tick();

#endif