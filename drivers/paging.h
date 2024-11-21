#ifndef __KPAGING_H__
#define __KPAGING_H__

#include "../libk/kdef.h"

#define KERNEL_OFFSET_HIGH 0xFFFFFFFF80000000ULL
#define APIC_PHYS_BASE 0xFEE00000ULL
#define APIC_VIRT_BASE 0xFFFFFFFFFEE00000ULL

#define PAGE_SIZE 0x200000ULL
#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITE (1ULL << 1)
#define PAGE_USER (1ULL << 2) 
#define PAGE_WRITETHROUGH (1ULL << 3)
#define PAGE_CACHE_DISABLE (1ULL << 4)
#define PAGE_HUGE (1ULL << 7)

typedef struct 
{
   uint64_t entries[512];
} page_tb_t;

void init_paging();
void map_page(uint64_t virt, uint64_t phys, uint64_t flags);

#endif