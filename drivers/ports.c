#include "ports.h"

unsigned char port_byte_in(unsigned short port) {
    unsigned char result;

    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));

    return result;
}


void port_byte_out(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}


unsigned short port_word_in(unsigned short port) {
    unsigned short result;

    __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));

    return result;
}


void port_word_out(unsigned short port, unsigned short data) {
    __asm__("out %%ax, %%dx" : : "a" (data), "d" (port));
}

void insw(u16 port, void *addr, u32 count) {
    asm volatile ("rep insw" : "+D"(addr), "+c"(count) : "d"(port) : "memory");
}

void outsw(u16 port, void *addr, u32 count) {
    asm volatile ("rep outsw" : "+S"(addr), "+c"(count) : "d"(port));
}