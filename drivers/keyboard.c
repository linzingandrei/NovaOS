#include "keyboard.h"

#define KB_DATA_PORT   0x60
#define KB_STATUS_PORT 0x64


static inline u8 inb(u16 port) {
    u8 ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static s8 key_buffer[5000] = {0};

static const s8 scancode_ascii[128] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0',
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p',
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f',
    [0x22] = 'g', [0x23] = 'h', [0x24] = 'j', [0x25] = 'k',
    [0x26] = 'l',
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = 'm',
    [0x0E] = '\b',
    [0x1C] = '\n',
    [0x39] = ' ',
};

s8 keyboard_get_char() {
    u8 scancode;

    scancode = inb(0x60);

    if (scancode & 0x80)
        return 0;
    if (scancode < 128)
        return scancode_ascii[scancode];
    return 0;
}

int view = 0;
u32 raw_row = 0;
char pio_buffer[600];
void keyboard_input(struct registers_t *regs) {
    s8 ch = keyboard_get_char();
    u32 raw_col = text->raw_col % TEXT_COLS;

    if (cursor_visible) {
        clear_cursor();
        cursor_visible = 0;
    }

    if (ch == '\n') {
        print_char(ch);
        raw_col = text->raw_col;
        // print_string(key_buffer);
        // print_char(ch);

        if (strcmp(key_buffer, "cls") == 0 && view == 0) {
            clear_screen();
            // print_string("DA");
        }
        else if (strcmp(key_buffer, "view") == 0) {
            if (view != 1) {
                raw_row = text->raw_col;
            }
            view = 1;
        }
        else if (strcmp(key_buffer, "exit") == 0) {
            view = 0;
        }
        else if (strcmp(key_buffer, "pio") == 0 && view == 0) {
            memory_set(pio_buffer, 0, 600);
            ata_pio_read28(50, 1, pio_buffer);
            dump_sector_hex(pio_buffer, 512);
        }
        else if (strcmp(key_buffer, "testwrite") == 0 && view == 0) {
            char message[100] = "Hello!";
            memory_copy(message, pio_buffer, strlen(message));
            ata_pio_write28(50, 1, pio_buffer);
            print_string("Gata\n");
            ata_pio_read28(50, 1, pio_buffer);
            dump_sector_hex(pio_buffer, 32);
        }
        else {
            if (view == 0)
                print_string("Command not known!\n");
        }

        // key_buffer[0] = '\0';
        memory_set(key_buffer, 0, strlen(key_buffer));
        if (view == 0) {
            print_string("> ");
        }
    }
    else if (ch == '\b') {
        if (raw_col > 2 && view == 0) {
            key_buffer[strlen(key_buffer) - 1] = '\0';
            backspace();
        }
        else {
            if (view == 1) {
                if((text->raw_col / TEXT_COLS) * SCREEN_WIDTH + (text->raw_col % TEXT_COLS) > raw_row * 8) {
                    key_buffer[strlen(key_buffer) - 1] = '\0';
                    backspace();
                }
            }
        }
    }
    else if (ch != 0) {
        key_buffer[strlen(key_buffer)] = ch;
        key_buffer[strlen(key_buffer) + 1] = '\0';
        print_char(ch);
    }
}

void install_keyboard() {
    irq_install_handler(1, keyboard_input);

    while (1) {
        update_cursor_blink();
    }
}

//// Polling

// s8 keyboard_poll_char() {
//     u8 scancode;

//     while (!(inb(KB_STATUS_PORT) & 1)) {
//         update_cursor_blink();
//     }

//     scancode = inb(KB_DATA_PORT);

//     if (scancode & 0x80)
//         return 0;
//     if (scancode < 128)
//         return scancode_ascii[scancode];
//     return 0;
// }

// int view = 0;
// void keyboard_input_loop() {
//     while (1) {
//         s8 ch = keyboard_poll_char();

//         if (cursor_visible) {
//             clear_cursor();
//             cursor_visible = 0;
//         }

//         if (ch == '\n') {
//             print_char(ch);
//             // print_string(key_buffer);
//             // print_char(ch);

//             if (strcmp(key_buffer, "cls") == 0) {
//                 clear_screen();
//                 // print_string("DA");
//             }
//             else if (strcmp(key_buffer, "view") == 0) {
//                 view = 1;
//             }
//             else if (strcmp(key_buffer, "exit") == 0) {
//                 view = 0;
//             }
//             else {
//                 if (view == 0)
//                     print_string("Command not known!\n");
//             }

//             // key_buffer[0] = '\0';
//             memory_set(key_buffer, 0, strlen(key_buffer));
//             if (view == 0) {
//                 print_string("> ");
//             }
//         }
//         else if (ch == '\b') {
//            key_buffer[strlen(key_buffer) - 1] = '\0';
//            backspace();
//         }
//         else if (ch != 0) {
//             key_buffer[strlen(key_buffer)] = ch;
//             key_buffer[strlen(key_buffer) + 1] = '\0';
//             print_char(ch);
//         }

//         sleep(50);
//     }
// }
