[bits 16]
disk_load:
    pusha
    push si
    push es
    push edi

next_sector:
    cmp cx, 0
    je done_read

    
    mov BYTE [disk_packet], 0x10
    mov BYTE [disk_packet + 1], 0x00
    mov WORD [disk_packet + 2], 1
    mov WORD [disk_packet + 4], bx
    mov WORD [disk_packet + 6], es
    mov DWORD [disk_packet + 8], edi
    mov DWORD [disk_packet + 12], 0

    mov ah, 0x42
    mov si, disk_packet
    int 0x13
    jc disk_error

    add edi, 1
    add bx, 512

    dec cx
    jmp next_sector

done_read:
    pop edi
    pop es
    pop si
    popa
    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print
    call print_nl

    jmp $

DISK_ERROR_MSG db "Disk read error!", 0

disk_packet:
    times 16 db 0