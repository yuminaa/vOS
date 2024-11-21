// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGAMEMORY ((volatile unsigned short*) 0xFFFFFFFF800B8000)

#include "components/interrupt_handler.h"
#include "../drivers/init.h"
#include "../libk/io.h"
#include "../drivers/timer.h"
#include "../drivers/paging.h"

void clear_vga_buffer(uint8_t color)
{
    uint16_t blank = (color << 8) | ' ';
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i)
        VGAMEMORY[i] = blank;
}

void kernel_main()
{
    __asm__ volatile("cli"); 
    
    clear_vga_buffer(0x0F);
    init_cpu();
    init_gdt();
    init_paging();
    init_idt();
    init_interrupt_handlers();
    init_apic();
    init_timer(100);

    printf("All initialized, enabling interrupts\n");
    __asm__ volatile("sti");
    
    printf(".");
    printf(".");
    printf(".");
    
    while(1) 
        __asm__("hlt");
}