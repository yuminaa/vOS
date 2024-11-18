#ifndef __KINTERRUPT_HANDLER_H__
#define __KINTERRUPT_HANDLER_H__

#define IST_NONE            0
#define IST_NMI            1
#define IST_DOUBLE_FAULT   2
#define IST_STACK_FAULT    3

#include "../../libk/kdef.h"

typedef struct
{
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
} __attribute__((packed)) interrupt_frame_t;

// CPU Exceptions (0-31)
__attribute__((interrupt)) void exception_div_by_zero(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_debug(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_nmi(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_breakpoint(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_overflow(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_bound_range(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_invalid_opcode(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_device_not_available(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_double_fault(interrupt_frame_t* frame, uint64_t error);
__attribute__((interrupt)) void exception_coprocessor_segment(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_invalid_tss(interrupt_frame_t* frame, uint64_t error);
__attribute__((interrupt)) void exception_segment_not_present(interrupt_frame_t* frame, uint64_t error);
__attribute__((interrupt)) void exception_stack_segment(interrupt_frame_t* frame, uint64_t error);
__attribute__((interrupt)) void exception_general_protection(interrupt_frame_t* frame, uint64_t error);
__attribute__((interrupt)) void exception_page_fault(interrupt_frame_t* frame, uint64_t error);
__attribute__((interrupt)) void exception_x87_float(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_alignment_check(interrupt_frame_t* frame, uint64_t error);
__attribute__((interrupt)) void exception_machine_check(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_simd_float(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_virtualization(interrupt_frame_t* frame);
__attribute__((interrupt)) void exception_security(interrupt_frame_t* frame, uint64_t error);

// IRQ Handlers (32-47)
__attribute__((interrupt)) void irq0_handler(interrupt_frame_t* frame);  // Timer
__attribute__((interrupt)) void irq1_handler(interrupt_frame_t* frame);  // Keyboard
__attribute__((interrupt)) void irq2_handler(interrupt_frame_t* frame);  // Cascade
__attribute__((interrupt)) void irq3_handler(interrupt_frame_t* frame);  // COM2
__attribute__((interrupt)) void irq4_handler(interrupt_frame_t* frame);  // COM1
__attribute__((interrupt)) void irq5_handler(interrupt_frame_t* frame);  // LPT2
__attribute__((interrupt)) void irq6_handler(interrupt_frame_t* frame);  // Floppy
__attribute__((interrupt)) void irq7_handler(interrupt_frame_t* frame);  // LPT1
__attribute__((interrupt)) void irq8_handler(interrupt_frame_t* frame);  // RTC
__attribute__((interrupt)) void irq9_handler(interrupt_frame_t* frame);  // Free
__attribute__((interrupt)) void irq10_handler(interrupt_frame_t* frame); // Free
__attribute__((interrupt)) void irq11_handler(interrupt_frame_t* frame); // Free
__attribute__((interrupt)) void irq12_handler(interrupt_frame_t* frame); // PS/2 Mouse
__attribute__((interrupt)) void irq13_handler(interrupt_frame_t* frame); // FPU
__attribute__((interrupt)) void irq14_handler(interrupt_frame_t* frame); // Primary ATA
__attribute__((interrupt)) void irq15_handler(interrupt_frame_t* frame); // Secondary ATA

void enable_irq(uint8_t irq);
void disable_irq(uint8_t irq);
void init_interrupt_handlers();

#endif