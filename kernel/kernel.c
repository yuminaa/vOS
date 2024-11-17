// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGAMEMORY ((volatile unsigned short*) 0xFFFFFFFF800B8000)

void clear_vga_buffer(unsigned char color)
{
    unsigned short blank = (color << 8) | ' ';
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i)
        VGAMEMORY[i] = blank;
}

void putchar_at(int pos, char c, unsigned char color)
{
    VGAMEMORY[pos] = (color << 8) | c;
}

int get_pos(int x, int y)
{
    return y * VGA_WIDTH + x;
}

void write_he(unsigned char color)
{
    putchar_at(get_pos(0, 0), 'H', color);
    putchar_at(get_pos(1, 0), 'e', color);
}

void kernel_main()
{
    unsigned char default_color = 0x0F;
    clear_vga_buffer(default_color);

    write_he(default_color);
    
    putchar_at(get_pos(2, 0), 'l', default_color);
    putchar_at(get_pos(3, 0), 'l', default_color);
    putchar_at(get_pos(4, 0), 'o', default_color);

    putchar_at(get_pos(0, 1), 'W', default_color);
    putchar_at(get_pos(1, 1), 'o', default_color);
    putchar_at(get_pos(2, 1), 'r', default_color);
    putchar_at(get_pos(3, 1), 'l', default_color);
    putchar_at(get_pos(4, 1), 'd', default_color);

    while (1);
}