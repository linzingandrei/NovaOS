#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/cursor.h"
#include "../fs/ffs.h"


void main() {
    idt_install();
    isr_install();
    irq_install();

    screen_init();

    __asm__ __volatile__("sti");

    install_timer();

    format(5);

    print_string("**********************\n");
    print_string("**********************\n");
    print_string("** Welcome!         **\n");
    print_string("**                  **\n");
    print_string("**                  **\n");
    print_string("**                  **\n");
    print_string("**********************\n");
    print_string("**********************\n");
    print_string("> ");

    struct superblock_t *sb;

    // write_file("HELLO", "ACASA", 5);
    // write_file("HELLO2", "ACASA2", 6);

    install_keyboard();

    // screen_put();

    // print_char('a');
    // print_char('A', 10, 60, 0xAC);
    // int a = 3 / 0;

    // keyboard_input_loop();
}
