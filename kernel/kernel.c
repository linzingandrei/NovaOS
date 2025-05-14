#include "../drivers/screen.h"
#include "../drivers/keyboard.h"


void main() {
    clear_screen();

    kprint("Welcome!\n");
    keyboard_input_loop();
}
