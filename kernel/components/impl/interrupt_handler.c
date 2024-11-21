#include "../interrupt_handler.h"
#include "../../../libk/io.h"
#include "../../../drivers/port.h"
#include "../../../drivers/init.h"
#include "../../../drivers/timer.h"
#include "../../../drivers/paging.h"

static const char scancode_to_ascii[] = {
    0,   // 0x00 - Error or NULL
    0,   // 0x01 - Esc
    '1', // 0x02
    '2', // 0x03
    '3', // 0x04
    '4', // 0x05
    '5', // 0x06
    '6', // 0x07
    '7', // 0x08
    '8', // 0x09
    '9', // 0x0A
    '0', // 0x0B
    '-', // 0x0C
    '=', // 0x0D
    '\b',// 0x0E - Backspace
    '\t',// 0x0F - Tab
    'q', // 0x10
    'w', // 0x11
    'e', // 0x12
    'r', // 0x13
    't', // 0x14
    'y', // 0x15
    'u', // 0x16
    'i', // 0x17
    'o', // 0x18
    'p', // 0x19
    '[', // 0x1A
    ']', // 0x1B
    '\n',// 0x1C - Enter
    0,   // 0x1D - Left Ctrl
    'a', // 0x1E
    's', // 0x1F
    'd', // 0x20
    'f', // 0x21
    'g', // 0x22
    'h', // 0x23
    'j', // 0x24
    'k', // 0x25
    'l', // 0x26
    ';', // 0x27
    '\'',// 0x28
    '`', // 0x29
    0,   // 0x2A - Left Shift
    '\\',// 0x2B
    'z', // 0x2C
    'x', // 0x2D
    'c', // 0x2E
    'v', // 0x2F
    'b', // 0x30
    'n', // 0x31
    'm', // 0x32
    ',', // 0x33
    '.', // 0x34
    '/', // 0x35
    0,   // 0x36 - Right Shift
    '*', // 0x37
    0,   // 0x38 - Left Alt
    ' ', // 0x39 - Space
};

static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Security Exception"
};

// Common exception handler
static void exception_handler(const char* message, interrupt_frame_t* frame)
{
    printf("EXCEPTION: %s\n", message);
    printf("RIP: 0x%llx\n", frame->ip);
    printf("CS:  0x%llx\n", frame->cs);
    printf("RFLAGS: 0x%llx\n", frame->flags);
    printf("RSP: 0x%llx\n", frame->sp);
    printf("SS:  0x%llx\n", frame->ss);
    for(;;) __asm__("hlt");
}

void exception_div_by_zero(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[0], frame);
}

void exception_debug(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[1], frame);
}

void exception_nmi(interrupt_frame_t* frame) 
{
    printf("NMI Interrupt\n");
}

void exception_breakpoint(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[3], frame);
}

void exception_overflow(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[4], frame);
}

void exception_bound_range(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[5], frame);
}

void exception_invalid_opcode(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[6], frame);
}

void exception_device_not_available(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[7], frame);
}

void exception_double_fault(interrupt_frame_t* frame, uint64_t error) 
{
    printf("EXCEPTION: Double Fault (Error: %llx)\n", error);
    printf("RIP: 0x%llx\n", frame->ip);
    while (true) 
        __asm__("hlt");
}

void exception_coprocessor_segment(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[9], frame);
}

void exception_invalid_tss(interrupt_frame_t* frame, uint64_t error)
{
    printf("EXCEPTION: Invalid TSS (Error: %llx)\n", error);
    printf("RIP: 0x%llx\n", frame->ip);
    while (true) 
        __asm__("hlt");
}

void exception_segment_not_present(interrupt_frame_t* frame, uint64_t error)
{
    printf("EXCEPTION: Segment Not Present (Error: %llx)\n", error);
    printf("RIP: 0x%llx\n", frame->ip);
    for(;;) __asm__("hlt");
}

void exception_stack_segment(interrupt_frame_t* frame, uint64_t error)
{
    printf("EXCEPTION: Stack Segment Fault (Error: %llx)\n", error);
    printf("RIP: 0x%llx\n", frame->ip);
    for(;;) __asm__("hlt");
}

void exception_general_protection(interrupt_frame_t* frame, uint64_t error)
{
    printf("EXCEPTION: General Protection Fault (Error: %llx)\n", error);
    printf("RIP: 0x%llx\n", frame->ip);
    for(;;) __asm__("hlt");
}

void exception_page_fault(interrupt_frame_t* frame, uint64_t error)
{
    uint64_t fault_addr;
    __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
    
    printf("PAGE FAULT\n");
    printf("Error code: %llx\n", error);
    printf("Fault address: %llx\n", fault_addr);
    printf("RIP: %llx\n", frame->ip);
    
    printf("Fault details:\n");
    if (!(error & 0x1)) 
    {
        printf("- Page not present\n");
        
        uint64_t pml4_idx = (fault_addr >> 39) & 0x1FF;
        uint64_t pdp_idx = (fault_addr >> 30) & 0x1FF;
        uint64_t pd_idx = (fault_addr >> 21) & 0x1FF;
        
        page_tb_t* pml4 = (page_tb_t*)0x1000;
        printf("PML4[%d] = %llx\n", pml4_idx, pml4->entries[pml4_idx]);
        
        if (pml4->entries[pml4_idx] & PAGE_PRESENT)
        {
            page_tb_t* pdp = (page_tb_t*)(pml4->entries[pml4_idx] & ~0xFFF);
            printf("PDP[%d] = %llx\n", pdp_idx, pdp->entries[pdp_idx]);
        }
    }
    printf("\nAttempted access near:\n");
    printf("Page aligned address: %llx\n", fault_addr & ~0xFFF);
    printf("Offset in page: %x\n", fault_addr & 0xFFF);
    
    while(1) 
        __asm__("hlt");
}

void exception_x87_float(interrupt_frame_t* frame) 
{
    exception_handler(exception_messages[16], frame);
}

void exception_alignment_check(interrupt_frame_t* frame, uint64_t error)
{
    printf("EXCEPTION: Alignment Check (Error: %llx)\n", error);
    printf("RIP: 0x%llx\n", frame->ip);
    while (true) 
        __asm__("hlt");
}

void exception_machine_check(interrupt_frame_t* frame)
{
    exception_handler(exception_messages[18], frame);
}

void exception_simd_float(interrupt_frame_t* frame)
{
    exception_handler(exception_messages[19], frame);
}

void exception_virtualization(interrupt_frame_t* frame)
{
    exception_handler(exception_messages[20], frame);
}

void exception_security(interrupt_frame_t* frame, uint64_t error) 
{
    printf("EXCEPTION: Security Exception (Error: %llx)\n", error);
    printf("RIP: 0x%llx\n", frame->ip);
    while (true) 
        __asm__("hlt");
}

// PIC helper functions
static inline void pic_send_eoi(unsigned char irq) 
{
    if(irq >= 8)
        outb(0xA0, 0x20);    // Send EOI to slave PIC
    outb(0x20, 0x20);        // Send EOI to master PIC
}

void enable_irq(uint8_t irq) 
{
    uint16_t port;
    uint8_t value;
    
    if(irq < 8) 
    {
        port = PIC1_DATA;
    } 
    else 
    {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void disable_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t value;
    
    if(irq < 8) 
    {
        port = PIC1_DATA;
    } 
    else 
    {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

// IRQ Handlers
void irq0_handler(interrupt_frame_t* frame)
{
    timer_tick();
    pic_send_eoi(0);
}

void irq1_handler(interrupt_frame_t* frame)
{
    uint8_t scancode = inb(0x60);
    
    // Check if it's a key press (not a release)
    if (!(scancode & 0x80)) 
    {
        if (scancode < sizeof(scancode_to_ascii) && scancode_to_ascii[scancode] != 0)
            printf("%c", scancode_to_ascii[scancode]);
    }
    pic_send_eoi(1);
}

// Default handler for other IRQs
static void default_irq_handler(int irq, interrupt_frame_t* frame) 
{
    pic_send_eoi(irq);
}

// Generate handlers for remaining IRQs
void irq2_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(2, frame); 
}

void irq3_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(3, frame); 
}

void irq4_handler(interrupt_frame_t* frame) 
{
    default_irq_handler(4, frame); 
}

void irq5_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(5, frame); 
}

void irq6_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(6, frame); 
}

void irq7_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(7, frame); 
}

void irq8_handler(interrupt_frame_t* frame)
{
    default_irq_handler(8, frame); 
}

void irq9_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(9, frame); 
}

void irq10_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(10, frame); 
}

void irq11_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(11, frame);
}

void irq12_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(12, frame); 
}

void irq13_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(13, frame); 
}

void irq14_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(14, frame); 
}

void irq15_handler(interrupt_frame_t* frame) 
{ 
    default_irq_handler(15, frame); 
}

static uint8_t* allocate_stack(size_t size)
{
    // Will replace the static allocation with a dynamic one later
    static uint8_t stack1[16384];
    static uint8_t stack2[16384];
    static uint8_t stack3[16384];
    static int current_stack = 0;
    
    switch(current_stack++)
    {
        case 0: 
            return stack1;
        case 1: 
            return stack2;
        case 2:
            return stack3;
        default: 
            return NULL;
    }
}

void init_interrupt_handlers() 
{
    // Set up CPU exception handlers (0-31)
    idt_set_gate(0, (uint64_t)exception_div_by_zero, 0x08, 0x8E, 0);
    idt_set_gate(1, (uint64_t)exception_debug, 0x08, 0x8E, 0);
    idt_set_gate(2, (uint64_t)exception_nmi, 0x08, 0x8E, IST_NMI);         // Use IST for NMI
    idt_set_gate(3, (uint64_t)exception_breakpoint, 0x08, 0x8E, 0);
    idt_set_gate(4, (uint64_t)exception_overflow, 0x08, 0x8E, 0);
    idt_set_gate(5, (uint64_t)exception_bound_range, 0x08, 0x8E, 0);
    idt_set_gate(6, (uint64_t)exception_invalid_opcode, 0x08, 0x8E, 0);
    idt_set_gate(7, (uint64_t)exception_device_not_available, 0x08, 0x8E, 0);
    idt_set_gate(8, (uint64_t)exception_double_fault, 0x08, 0x8E, IST_DOUBLE_FAULT); // Use IST
    idt_set_gate(9, (uint64_t)exception_coprocessor_segment, 0x08, 0x8E, 0);
    idt_set_gate(10, (uint64_t)exception_invalid_tss, 0x08, 0x8E, 0);
    idt_set_gate(11, (uint64_t)exception_segment_not_present, 0x08, 0x8E, 0);
    idt_set_gate(12, (uint64_t)exception_stack_segment, 0x08, 0x8E, IST_STACK_FAULT); // Use IST
    idt_set_gate(13, (uint64_t)exception_general_protection, 0x08, 0x8E, 0);
    idt_set_gate(14, (uint64_t)exception_page_fault, 0x08, 0x8E, 0);
    idt_set_gate(16, (uint64_t)exception_x87_float, 0x08, 0x8E, 0);
    idt_set_gate(17, (uint64_t)exception_alignment_check, 0x08, 0x8E, 0);
    idt_set_gate(18, (uint64_t)exception_machine_check, 0x08, 0x8E, 0);
    idt_set_gate(19, (uint64_t)exception_simd_float, 0x08, 0x8E, 0);
    idt_set_gate(20, (uint64_t)exception_virtualization, 0x08, 0x8E, 0);
    idt_set_gate(30, (uint64_t)exception_security, 0x08, 0x8E, 0);

    // Set up IRQ handlers (32-47)
    idt_set_gate(32, (uint64_t)irq0_handler, 0x08, 0x8E, 0);  // Timer
    idt_set_gate(33, (uint64_t)irq1_handler, 0x08, 0x8E, 0);  // Keyboard
    idt_set_gate(34, (uint64_t)irq2_handler, 0x08, 0x8E, 0);  // Cascade
    idt_set_gate(35, (uint64_t)irq3_handler, 0x08, 0x8E, 0);  // COM2
    idt_set_gate(36, (uint64_t)irq4_handler, 0x08, 0x8E, 0);  // COM1
    idt_set_gate(37, (uint64_t)irq5_handler, 0x08, 0x8E, 0);  // LPT2
    idt_set_gate(38, (uint64_t)irq6_handler, 0x08, 0x8E, 0);  // Floppy
    idt_set_gate(39, (uint64_t)irq7_handler, 0x08, 0x8E, 0);  // LPT1
    idt_set_gate(40, (uint64_t)irq8_handler, 0x08, 0x8E, 0);  // CMOS RTC
    idt_set_gate(41, (uint64_t)irq9_handler, 0x08, 0x8E, 0);  // Free
    idt_set_gate(42, (uint64_t)irq10_handler, 0x08, 0x8E, 0); // Free
    idt_set_gate(43, (uint64_t)irq11_handler, 0x08, 0x8E, 0); // Free
    idt_set_gate(44, (uint64_t)irq12_handler, 0x08, 0x8E, 0); // PS/2 Mouse
    idt_set_gate(45, (uint64_t)irq13_handler, 0x08, 0x8E, 0); // FPU
    idt_set_gate(46, (uint64_t)irq14_handler, 0x08, 0x8E, 0); // Primary ATA
    idt_set_gate(47, (uint64_t)irq15_handler, 0x08, 0x8E, 0); // Secondary ATA

    // Set up IST stacks for critical interrupts
    uint8_t* nmi_stack = allocate_stack(16384);        // 16KB stack
    uint8_t* df_stack = allocate_stack(16384);         // 16KB stack
    uint8_t* sf_stack = allocate_stack(16384);         // 16KB stack

    set_ist(IST_NMI, (uint64_t)nmi_stack + 16384);           // Stack grows down
    set_ist(IST_DOUBLE_FAULT, (uint64_t)df_stack + 16384);
    set_ist(IST_STACK_FAULT, (uint64_t)sf_stack + 16384);
}