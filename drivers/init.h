#ifndef __KINIT_H__
#define __KINIT_H__

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_MSR_ENABLE 0x800
#define APIC_ID_REG 0x20
#define APIC_SPURIOUS_REG 0xF0
#define GDT_ENTRIES 7
#define IDT_ENTRIES 256

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
void apic_write(uint32_t reg, uint32_t value);
uint32_t apic_read(uint32_t reg);
void init_apic();
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags, uint8_t ist);
void set_kernel_stack(uint64_t stack);
void set_ist(int ist_num, uint64_t stack);

#endif