org 0x7c00
KERNEL_OFFSET equ 0x7E00

    mov [BOOT_DRIVE], dl

    mov bp, 0x3000
    mov sp, bp

    mov ah, 0x00
    mov al, 0x13
    int 0x10

    call load_kernel

    in al, 0x92
    test al, 2
    jnz after
    or al, 2
    and al, 0xFE
    out 0x92, al
    after:
        call switch_to_pm
        jmp $

%include "boot/print.asm"
%include "boot/disk.asm"
%include "boot/gdt.asm"
%include "boot/32bit_print.asm"
%include "boot/switch_pm.asm"

bits 16
load_kernel:
    mov bx, KERNEL_OFFSET
    mov dh, 63 
    mov dl, [BOOT_DRIVE]
    call disk_load

    ret

bits 32
BEGIN_PM:
    jmp KERNEL_OFFSET


BOOT_DRIVE db 0x80

times 510-($-$$) db 0
dw 0xAA55
