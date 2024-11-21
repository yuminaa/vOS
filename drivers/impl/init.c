// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#include "../init.h"
#include "../cpu.h"
#include "../port.h"
#include "../paging.h"
#include "../../libk/io.h"

static struct gdt_entry gdt[GDT_ENTRIES];
static struct tss_entry tss;
static struct idt_entry idt[IDT_ENTRIES];
static struct gdt_ptr gdtp;
static struct idt_ptr idtp;

static void gdt_set_gate(int num, uint64_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].access = access;
}

static void tss_set_gate(uint64_t num, uint64_t base, uint32_t limit, uint8_t access)
{
    uint64_t tss_base = (uint64_t)&tss;
    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].base_low = tss_base & 0xFFFF;
    gdt[num].base_middle = (tss_base >> 16) & 0xFF;
    gdt[num].access = access;
    gdt[num].granularity = ((limit >> 16) & 0x0F);
    gdt[num].base_high = (tss_base >> 24) & 0xFF;
}

void init_gdt()
{
    gdtp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gdtp.base = (uint64_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0); // Null descriptor
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xA0); // Kernel code segment
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xA0); // Kernel data segment
    gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xA0); // User code segment
    gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xA0); // User data segment
    tss_set_gate(5, 0, sizeof(struct tss_entry), 0x89); // TSS

    __asm__ volatile ("lgdt %0" : : "m"(gdtp)); // Load GDT
    __asm__ volatile (      // Reload segments
        "pushq $0x08\n"
        "pushq $reload_cs\n"
        "retfq\n"
        "reload_cs:\n"
        "movw $0x10, %ax\n"
        "movw %ax, %ds\n"
        "movw %ax, %es\n"
        "movw %ax, %fs\n"
        "movw %ax, %gs\n"
        "movw %ax, %ss\n"
    );
}

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags, uint8_t ist)
{
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_middle = (base >> 16) & 0xFFFF;
    idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = ist;
    idt[num].flags = flags;
    idt[num].reserved = 0;
}

void set_ist(int ist_num, uint64_t stack)
{
    if (ist_num < 1 || ist_num > 7)
        return;
    tss.ist[ist_num - 1] = stack;
}

void init_idt() 
{
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (uint64_t)&idt;

    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate(i, 0, 0x08, 0x8E, 0);
    
    __asm__ volatile ("lidt %0" : : "m"(idtp)); // Load IDT
}

void set_kernel_stack(uint64_t stack)
{
    tss.rsp0 = stack;
}

void init_cpu()
{
    char vendor[13];
    cpu_get_vendor(vendor);
    
    if (!cpu_has_feature(CPU_FEATURE_PAE))
        __asm__ volatile("hlt");
    
    if (!cpu_has_feature(CPU_FEATURE_MSR)) 
        __asm__ volatile("hlt"); // Handle missing MSR support (later)

    uint64_t cr4;
    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 5);  // Enable PAE
    cr4 |= (1 << 7);  // Enable PGE
    __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));
}

void apic_write(uint32_t reg, uint32_t value) 
{
    volatile uint32_t *apic = (volatile uint32_t *)(uintptr_t)APIC_VIRT_BASE;
    apic[reg >> 2] = value;
}

uint32_t apic_read(uint32_t reg)
{
    volatile uint32_t *apic = (volatile uint32_t *)(uintptr_t)APIC_VIRT_BASE;
    return apic[reg >> 2];
}

void init_apic() 
{
    uint32_t low, high;
    __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(APIC_BASE_MSR));
    uint64_t phys_base = ((uint64_t)high << 32) | (low & 0xFFFFF000);
    
    low = (uint32_t)(phys_base | APIC_BASE_MSR_ENABLE);
    __asm__ volatile("wrmsr" : : "c"(APIC_BASE_MSR), "a"(low), "d"(high));

    uint32_t id = apic_read(APIC_ID_REG);
    uint32_t svr = apic_read(APIC_SPURIOUS_REG);
    apic_write(APIC_SPURIOUS_REG, svr | 0x100);

    svr = apic_read(APIC_SPURIOUS_REG);
}
