#include "../init.h"
#include "../cpu.h"
#include "../port.h"

#define GDT_ENTRIES 7
#define IDT_ENTRIES 256

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

void init_gdt(void)
{
    gdtp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gdtp.base = (uint64_t)&gdt;

    // Null descriptor
    gdt_set_gate(0, 0, 0, 0, 0);
    // Kernel code segment
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xA0);
    // Kernel data segment
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xA0);
    // User code segment
    gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xA0);
    // User data segment
    gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xA0);
    // TSS
    tss_set_gate(5, 0, sizeof(struct tss_entry), 0x89);

    // Load GDT
    __asm__ volatile ("lgdt %0" : : "m"(gdtp));
    
    // Reload segments
    __asm__ volatile (
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

static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_middle = (base >> 16) & 0xFFFF;
    idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = 0;
    idt[num].flags = flags;
    idt[num].reserved = 0;
}

void init_idt() 
{
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (uint64_t)&idt;

    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate(i, 0, 0x08, 0x8E);
    // Load IDT
    __asm__ volatile ("lidt %0" : : "m"(idtp));
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
        // Handle missing MSR
        __asm__ volatile("hlt");

    uint64_t cr4;
    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 5);  // Enable PAE
    cr4 |= (1 << 7);  // Enable PGE
    __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));
}

void init_pic()
{
    // ICW1
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    
    // ICW2: Vector offsets
    outb(0x21, 0x20);    // Master: IRQ 0-7 mapped to interrupts 32-39
    outb(0xA1, 0x28);    // Slave: IRQ 8-15 mapped to interrupts 40-47
    
    // ICW3: Cascading
    outb(0x21, 0x04);    // Tell master there's a slave at IRQ2
    outb(0xA1, 0x02);    // Tell slave its cascade identity
    
    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    
    // Mask all interrupts
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}