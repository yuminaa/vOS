#ifndef __KCPU_H__
#define __KCPU_H__

#include "../libk/kdef.h"

#define CPUID_VENDOR_ID          0x0
#define CPUID_FEATURES           0x1
#define CPUID_TLB               0x2
#define CPUID_SERIAL            0x3
#define CPUID_EXT_FEATURES      0x7
#define CPUID_HIGHEST_EXT      0x80000000
#define CPUID_EXT_FEATURES_2   0x80000001
#define CPUID_BRAND_STRING     0x80000002
#define CPUID_BRAND_STRING_2   0x80000003
#define CPUID_BRAND_STRING_3   0x80000004

#define CPU_FEATURE_FPU    (1 << 0)  // Floating-point unit
#define CPU_FEATURE_VME    (1 << 1)  // Virtual Mode Extension
#define CPU_FEATURE_DE     (1 << 2)  // Debugging Extension
#define CPU_FEATURE_PSE    (1 << 3)  // Page Size Extension
#define CPU_FEATURE_TSC    (1 << 4)  // Time Stamp Counter
#define CPU_FEATURE_MSR    (1 << 5)  // Model Specific Registers
#define CPU_FEATURE_PAE    (1 << 6)  // Physical Address Extension
#define CPU_FEATURE_MCE    (1 << 7)  // Machine Check Exception
#define CPU_FEATURE_CX8    (1 << 8)  // CMPXCHG8 Instruction
#define CPU_FEATURE_APIC   (1 << 9)  // APIC On-Chip
#define CPU_FEATURE_SEP    (1 << 11) // SYSENTER/SYSEXIT
#define CPU_FEATURE_MTRR   (1 << 12) // Memory Type Range Registers
#define CPU_FEATURE_PGE    (1 << 13) // Page Global Enable
#define CPU_FEATURE_MCA    (1 << 14) // Machine Check Architecture
#define CPU_FEATURE_PAT    (1 << 16) // Page Attribute Table
#define CPU_FEATURE_PSE36  (1 << 17) // 36-bit Page Size Extension
#define CPU_FEATURE_PSN    (1 << 18) // Processor Serial Number
#define CPU_FEATURE_MMX    (1 << 23) // MMX Technology
#define CPU_FEATURE_FXSR   (1 << 24) // FXSAVE/FXRSTOR
#define CPU_FEATURE_SSE    (1 << 25) // SSE Extensions
#define CPU_FEATURE_SSE2   (1 << 26) // SSE2 Extensions

#define CPU_FEATURE_SSE3   (1 << 0)  // SSE3 Extensions
#define CPU_FEATURE_PCLMUL (1 << 1)  // PCLMULQDQ Instruction
#define CPU_FEATURE_MONITOR (1 << 3)  // MONITOR/MWAIT
#define CPU_FEATURE_VMX    (1 << 5)  // Virtual Machine Extensions
#define CPU_FEATURE_SSSE3  (1 << 9)  // SSSE3 Extensions
#define CPU_FEATURE_FMA    (1 << 12) // FMA Extensions
#define CPU_FEATURE_SSE41  (1 << 19) // SSE4.1 Extensions
#define CPU_FEATURE_SSE42  (1 << 20) // SSE4.2 Extensions
#define CPU_FEATURE_AES    (1 << 25) // AES Instructions
#define CPU_FEATURE_AVX    (1 << 28) // Advanced Vector Extensions

typedef struct
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpuid_registers_t;

void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);
__attribute__((used)) void cpu_get_vendor(char* vendor);
cpuid_registers_t cpu_get_features();
__attribute__((used)) int cpu_has_feature(uint32_t feature);

#endif