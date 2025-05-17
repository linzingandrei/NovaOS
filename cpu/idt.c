#include "idt.h"


struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

void set_idt_gate(int n, u32 handler) {
    idt[n].low_offset = low_16(handler);
    idt[n].high_offset = high_16(handler);
    idt[n].segment_selector = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
}

void idt_install() {
    idtp.limit = sizeof(struct idt_entry) * 256 - 1;
    idtp.base = (u32) &idt;
    
    __asm__ __volatile__("lidtl (%0)" : : "r" (&idtp));
}