; Part of the vOS project
; Licensed under MIT License
; See LICENSE for more information

[BITS 64]
[GLOBAL kernel_start]

section .text
extern kernel_main

kernel_start:
    call kernel_main
.halt:
    hlt
    jmp .halt