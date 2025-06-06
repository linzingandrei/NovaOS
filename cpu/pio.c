#include "types.h"
#include "../drivers/ports.h"
#include "pio.h"
#include "timer.h"

/*
An example of a 28 bit LBA PIO mode read on the Primary bus:
1. Send 0xE0 for the "master" or 0xF0 for the "slave", ORed with the highest 4 bits of the LBA to port 0x1F6: outb(0x1F6, 0xE0 | (slavebit << 4) | ((LBA >> 24) & 0x0F))
2. Send a NULL byte to port 0x1F1, if you like (it is ignored and wastes lots of CPU time): outb(0x1F1, 0x00)
3. Send the sectorcount to port 0x1F2: outb(0x1F2, (unsigned char) count)
4. Send the low 8 bits of the LBA to port 0x1F3: outb(0x1F3, (unsigned char) LBA))
5. Send the next 8 bits of the LBA to port 0x1F4: outb(0x1F4, (unsigned char)(LBA >> 8))
6. Send the next 8 bits of the LBA to port 0x1F5: outb(0x1F5, (unsigned char)(LBA >> 16))
7. Send the "READ SECTORS" command (0x20) to port 0x1F7: outb(0x1F7, 0x20)
8. Wait for an IRQ or poll.
9. Transfer 256 16-bit values, a uint16_t at a time, into your buffer from I/O port 0x1F0. (In assembler, REP INSW works well for this.)
10. Then loop back to waiting for the next IRQ (or poll again -- see next note) for each successive sector.
*/
void ata_pio_read28(u32 LBA, u8 sector_count, u8 *target) {
    port_byte_out(0x1F6, 0xE0 | ((LBA >> 24) & 0x0F));
    port_byte_out(0x1F1, 0x00);
    port_byte_out(0x1F2, sector_count);
    port_byte_out(0x1F3, LBA & 0xFF);
    port_byte_out(0x1F4, (LBA >> 8) & 0xFF);
    port_byte_out(0x1F5, (LBA >> 16) & 0xFF);
    port_byte_out(0x1F7, 0x20);

    for (u8 i = 0; i < sector_count; i++) {
        while (1) {
            u8 status = port_byte_in(0x1F7);
            if (status & (1 << 3)) {
                break;
            }
        }
        insw(0x1F0, (void *)target, 256);
        target += 256;
    }
}

/*
To write sectors in 28 bit PIO mode, send command "WRITE SECTORS" (0x30) to the Command port. Do not use REP OUTSW to transfer data. There must be a tiny delay between each OUTSW output uint16_t. A jmp $+2 size of delay. Make sure to do a Cache Flush (ATA command 0xE7) after each write command completes.
*/
void ata_pio_write28(u32 LBA, u8 sector_count, u8 *target) {
    port_byte_out(0x1F6, 0xE0 | ((LBA >> 24) & 0x0F));
    port_byte_out(0x1F1, 0x00);
    port_byte_out(0x1F2, sector_count);
    port_byte_out(0x1F3, LBA & 0xFF);
    port_byte_out(0x1F4, (LBA >> 8) & 0xFF);
    port_byte_out(0x1F5, (LBA >> 16) & 0xFF);
    port_byte_out(0x1F7, 0x30);

    for (u8 i = 0; i < sector_count; i++) {
        while (1) {
            u8 status = port_byte_in(0x1F7);
            if (status & (1 << 3)) {
                break;
            }
        }
        outsw(0x1F0, (void *)target, 256);
        target += 256;
    }

    port_byte_out(0x1F7, 0xE7);
    while (port_byte_in(0x1F7) & (1 << 7)) {}
}