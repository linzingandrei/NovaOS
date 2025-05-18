#ifndef IDT_H
#define IDT_H

#include "types.h"


#define KERNEL_CS 0x08

struct idt_entry {
    u16 low_offset;
    u16 segment_selector;
    u8 always0;
    u8 flags;
    u16 high_offset;
} __attribute__((packed));

struct idt_ptr {
    u16 limit;
    u32 base; 
} __attribute__((packed));

#define IDT_ENTRIES 256

void set_idt_gate(int n, u32 handler);
void idt_install();

#endif