#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/isr.h"


void main() {
    kprint("Installing isr...\n");
    isr_install();
    kprint("Done\n");

    clear_screen();

    kprint("Welcome!\n");

    int a = 3 / 0;
    keyboard_input_loop();
}
