#include "timer.h"


int timer_ticks = 0;

void init_timer(u32 hz) {
    int divisor = 1193180 / hz;
    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, divisor & 0xFF);
    port_byte_out(0x40, divisor >> 8);
}

void timer_handler(struct registers_t *regs) {
    timer_ticks++;

    // if (timer_ticks % 100 == 0) {
    //     kprint("One second has passed\n");
    // }
}

void install_timer() {
    init_timer(100);

    irq_install_handler(0, timer_handler);
}