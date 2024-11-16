// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#include "../libk/kdef.h"

#define VGA_MEMORY ((uint16_t*)(0xFFFFFFFF800B8000))
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VGA_BLACK 0x0
#define VGA_WHITE 0xF
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)
#define VGA_ENTRY(c, color) ((color << 8) | c)

// This is for debugging purposes
static void vga_clear() 
{
    uint16_t blank = VGA_ENTRY(' ', VGA_COLOR(VGA_WHITE, VGA_BLACK));

    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) 
    {
        VGA_MEMORY[i] = blank;
    }
}

void kernel_main(void) 
{
    vga_clear();
    // Setup memory paging IO etc.

    // Yield the kernel forever
    while (true) 
    {
        asm("hlt");
    }
}