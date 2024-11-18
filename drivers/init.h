#ifndef GDT_IDT_H
#define GDT_IDT_H

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
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
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
void set_kernel_stack(uint64_t stack);

#endif