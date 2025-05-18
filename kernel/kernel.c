#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/timer.h"


void main() {
    kprint("Installing idt...\n");
    idt_install();
    kprint("Done\n");

    kprint("Installing isr...\n");
    isr_install();
    kprint("Done\n");

    kprint("Installing irq...\n");
    irq_install();
    kprint("Done\n");

    __asm__ __volatile__("sti");

    install_timer();

    clear_screen();

    kprint("Welcome!\n");

    // int a = 3 / 0;

    keyboard_input_loop();
}
