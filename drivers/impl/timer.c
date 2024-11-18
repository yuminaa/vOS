#include "../timer.h"
#include "../port.h"
#include "../../libk/io.h"

static volatile uint64_t timer_ticks = 0;

void init_timer(uint32_t frequency)
{
    uint32_t divisor = PIT_FREQUENCY / frequency;
    
    outb(PIT_COMMAND, PIT_SET_COMMAND);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, divisor >> 8);
}

void timer_tick()
{
    timer_ticks++;
    if (timer_ticks % 100 == 0)
        printf("Tick: %llu\n", timer_ticks);
}

uint64_t get_ticks()
{
    uint64_t ticks;
    __asm__ volatile("cli");
    ticks = timer_ticks;
    __asm__ volatile("sti");
    return ticks;
}