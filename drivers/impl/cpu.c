// Part of the vOS project
// Licensed under MIT License
// See LICENSE for more information

#include "../cpu.h"

void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d)
{
    __asm__ volatile("cpuid"
        : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
        : "a"(code));
}

__attribute__((weak)) void cpu_get_vendor(char* vendor)
{
    uint32_t dummy;
    cpuid(CPUID_VENDOR_ID, &dummy, (uint32_t*)(vendor),
          (uint32_t*)(vendor + 8), (uint32_t*)(vendor + 4));
    vendor[12] = '\0';
}

__attribute__((weak)) cpuid_registers_t cpu_get_features()
{
    cpuid_registers_t regs;
    cpuid(CPUID_FEATURES, &regs.eax, &regs.ebx, &regs.ecx, &regs.edx);
    return regs;
}

int cpu_has_feature(uint32_t feature)
{
    cpuid_registers_t regs = cpu_get_features();
    return (regs.edx & feature) || (regs.ecx & feature);
}
