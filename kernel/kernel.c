#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/cursor.h"


void main() {
    idt_install();
    isr_install();
    irq_install();

    screen_init();

    __asm__ __volatile__("sti");

    install_timer();

    print_string("**********************\n");
    print_string("**********************\n");
    print_string("** Welcome!         **\n");
    print_string("**                  **\n");
    print_string("**                  **\n");
    print_string("**                  **\n");
    print_string("**********************\n");
    print_string("**********************\n");
    print_string("> ");

    install_keyboard();

    // screen_put();

    // print_char('a');
    // print_char('A', 10, 60, 0xAC);
    // int a = 3 / 0;

    // keyboard_input_loop();
}
