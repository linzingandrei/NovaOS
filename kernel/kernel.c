#include "../drivers/screen.h"


void main() {
    clear_screen();

    print_at("X", 6, 1);
    print_at("This text spans multiple lines", 10, 75);
    print_at("There is a line\nbreak", 20, 0);
    print("There is a line\nbreak");
    print_at("What happens when we run out of space?", 24, 45);
}
