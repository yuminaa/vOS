#include "../paging.h"
#include "../libk/io.h"

void map_page(uint64_t virt, uint64_t phys, uint64_t flags)
{
   page_tb_t *pdp = (page_tb_t*)0x4000;   // Bootloader's PDPT
   page_tb_t *pd = (page_tb_t*)0x5000;    // Bootloader's PD
   
   size_t pdp_idx = (virt >> 30) & 0x1FF;
   size_t pd_idx = (virt >> 21) & 0x1FF;

   pdp->entries[pdp_idx] = ((uint64_t)pd) | PAGE_PRESENT | PAGE_WRITE; // Set up PDPT entry to point to PD
   pd->entries[pd_idx] = phys | flags; // Map the APIC in PD
}

void init_paging()
{
   uint64_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_HUGE | PAGE_CACHE_DISABLE;
   map_page(APIC_VIRT_BASE, APIC_PHYS_BASE, flags);
    
   // Flush TLB
   uint64_t cr3;
   __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
   __asm__ volatile("mov %0, %%cr3" : : "r"(cr3) : "memory");
}