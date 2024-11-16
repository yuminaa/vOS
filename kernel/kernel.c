// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#include "../libk/kdef.h"

void kernel_main()
{
    // Setup memory paging IO etc.

    // Yield the kernel forever
    while (true)
        asm("hlt");
}