#include "../cpu/types.h"

unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);
unsigned short port_word_in(unsigned short port);
void port_word_out(unsigned short port, unsigned short data);
void insw(u16 port, void *addr, u32 count);
void outsw(u16 port, void *addr, u32 count);