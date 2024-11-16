; Part of the vOS project
; Licensed under MIT License
; See LICENSE for more information
;
; Last Edited: Nov 15th 2024 by alplusplus
;   x86 bootloader in NASM syntax

[BITS 16]
[ORG 0x7C00]

KERNEL_OFFSET_HIGH  equ 0xFFFF_FFFF_8000_0000
KERNEL_LOAD_SEGMENT equ 0x1000
KERNEL_LOAD_OFFSET  equ 0x0000
KERNEL_OFFSET_LOW   equ (KERNEL_LOAD_SEGMENT << 4) + KERNEL_LOAD_OFFSET

start:
    ; Set up segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Load kernel sectors
    mov ah, 0x02            ; BIOS read sector function
    mov al, 9               ; Read 9 sectors
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Start from sector 2
    mov dh, 0               ; Head 0
    push KERNEL_LOAD_SEGMENT
    pop es
    mov bx, KERNEL_LOAD_OFFSET
    int 0x13
    jc halt

    ; Enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Load GDT and enter protected mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

[BITS 32]
protected_mode:
    ; Set up protected mode segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x200000

    ; Set up paging
    mov edi, 0x1000
    xor eax, eax
    mov ecx, 5 * 4096 / 4
    rep stosd

    ; Identity map first 2MB and set up higher half
    mov dword [0x1000], 0x00002003
    mov dword [0x2000], 0x00003003
    mov dword [0x3000], 0x00000083
    
    mov dword [0x1000 + 8 * 511], 0x00004003
    mov dword [0x4000 + 8 * 510], 0x00005003
    mov dword [0x5000], 0x00000083

    ; Enable paging and long mode
    mov edi, 0x1000
    mov cr3, edi

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, 10100000b
    mov cr4, eax

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Load 64-bit GDT and jump to long mode
    lgdt [gdt64_descriptor]
    jmp 0x08:long_mode

[BITS 64]
long_mode:
    ; Set up 64-bit segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack for higher half kernel
    mov rsp, 0x90000 + KERNEL_OFFSET_HIGH

    ; Jump to kernel
    mov rax, KERNEL_OFFSET_HIGH + KERNEL_OFFSET_LOW
    jmp rax

halt:
    cli
    hlt
    jmp halt

align 16
gdt_start:
    dq 0                    ; Null descriptor
    dq 0x00CF9A000000FFFF  ; 32-bit code
    dq 0x00CF92000000FFFF  ; 32-bit data
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

gdt64_start:
    dq 0                   ; Null descriptor
    dq 0x00AF9A000000FFFF  ; 64-bit code
    dq 0x00AF92000000FFFF  ; 64-bit data
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1
    dq gdt64_start

times 510-($-$$) db 0
dw 0xAA55