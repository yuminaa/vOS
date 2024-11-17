; Part of the vOS project
; Licensed under MIT License
; See LICENSE for more information

[BITS 16]
[ORG 0x7C00]

start:
    ; setup segments & stack at 0x7C00
    ; because it can overlap some BIOS data / video memory
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [bootDrive], dl

    mov cx, 0x0002      ; sector 2 (CH = 0, CL = 2)
    mov bx, 0x7E00      ; load stage 2 at memory address 0x7E00
    mov dl, [bootDrive] ; load boot drive number

load_loop:
    mov ah, 0x02        ; BIOS read sector function
    mov al, 3           ; Read 3 sectors (second stage)
    mov ch, 0           ; head number
    int 0x13            ; read disk op
    jc disk_error       ; jump if disk error

    ; print 'Loaded stage 2 booloader'
    mov si, successMessage
    call print_16bit

    mov dl, [bootDrive]
    jmp 0x0000:0x7E00   ; jump to segment 0x0000 at offset 0x7E00

; print error msg for debugging purpose in case the
; bootloader broke
disk_error:
    mov si, diskErrorMessage
    call print_16bit
    mov al, ah          ; store error code in AH
    call get_hex_code
    cli                 ; disable interrupt *IMPORTANT*
.hlt:
    hlt
    jmp .hlt            ; inf recursive loop

print_16bit:
    cld                 ; clear
    lodsb
    or al, al
    jz done
    mov ah, 0x0E        ; print letters
    int 0x10            ; BIOS interrupt
    jmp print_16bit
done:
    ret

get_hex_code:
    push ax
    push bx
    mov bx, ax
    shr al, 4
    call print_hex
    mov al, bl
    and al, 0x0F
    call print_hex
    pop bx
    pop ax
    ret

print_hex:
    and al, 0x0F
    add al, '0'
    cmp al, '9'
    jle .print
    add al, 7
.print:
    mov ah, 0x0E
    int 0x10
    ret

diskErrorMessage db 'Disk error code ', 0
successMessage db 'Loaded bootloader stage 2', 13, 10, 0
bootDrive db 0

; keeps the file limit as 512 otherwise it returns a negative value
times 510 - ($ - $$) db 0
; boot signature
dw 0xAA55