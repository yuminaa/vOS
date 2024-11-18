#ifndef __KINIT_H__
#define __KINIT_H__

#define PIC1_COMMAND    0x20    // Command port for PIC1
#define PIC1_DATA       0x21    // Data port for PIC1

// PIC2 (Slave PIC)
#define PIC2_COMMAND    0xA0    // Command port for PIC2
#define PIC2_DATA       0xA1    // Data port for PIC2

// Initialization Command Words
#define ICW1_ICW4       0x01    // ICW4 needed
#define ICW1_INIT       0x10    // Initialization
#define ICW4_8086       0x01    // 8086/88 mode

#include "../libk/kdef.h"

struct gdt_entry 
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct tss_entry 
{
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb;
} __attribute__((packed));

struct idt_entry 
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  flags;
    uint16_t base_middle;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct gdt_ptr 
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct idt_ptr 
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void init_gdt();
void init_idt();
void init_cpu();
void init_pic();
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags, uint8_t ist);
void set_kernel_stack(uint64_t stack);
void set_ist(int ist_num, uint64_t stack);

#endif