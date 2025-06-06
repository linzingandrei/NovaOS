#include "keyboard.h"
#include "../fs/ffs.h"

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
static const s8 shift_scancode_ascii[128] = {
    [0x02] = '!', [0x03] = '@', [0x04] = '#', [0x05] = '$',
    [0x06] = '%', [0x07] = '^', [0x08] = '&', [0x09] = '*',
    [0x0A] = '(', [0x0B] = ')',
    [0x10] = 'Q', [0x11] = 'W', [0x12] = 'E', [0x13] = 'R',
    [0x14] = 'T', [0x15] = 'Y', [0x16] = 'U', [0x17] = 'I',
    [0x18] = 'O', [0x19] = 'P',
    [0x1E] = 'A', [0x1F] = 'S', [0x20] = 'D', [0x21] = 'F',
    [0x22] = 'G', [0x23] = 'H', [0x24] = 'J', [0x25] = 'K',
    [0x26] = 'L',
    [0x2C] = 'Z', [0x2D] = 'X', [0x2E] = 'C', [0x2F] = 'V',
    [0x30] = 'B', [0x31] = 'N', [0x32] = 'M',
    [0x35] = '?',
};

u8 shift_pressed = 0;
s8 keyboard_get_char() {
    u8 scancode;

    scancode = inb(0x60);

    switch (scancode) {
        case 0x2A:
        case 0x36:
            shift_pressed = 1; 
            break;

        case 0xAA: 
        case 0xB6: 
            shift_pressed = 0; 
            break;

        default: break;
    }

    if (scancode & 0x80)
        return 0;
    
    if (scancode < 128) {
        char c = shift_pressed ? shift_scancode_ascii[scancode] : scancode_ascii[scancode];
        return c;
    }

    return 0;
}

int view = 0;
u32 raw_row = 0;
char pio_buffer[600];
int where_view_mode_starts = 0;
char view_buffer[5000] = {0};
void keyboard_input(struct registers_t *regs) {
    s8 ch = keyboard_get_char();
    u32 raw_col = text->raw_col % TEXT_COLS;

    if (cursor_visible) {
        clear_cursor();
        cursor_visible = 0;
    }

    if (ch == '\n') {
        if (view == 1) {
            view_buffer[strlen(view_buffer)] = ch;
            view_buffer[strlen(view_buffer) + 1] = '\0';
        }

        print_char(ch);
        raw_col = text->raw_col;
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
            where_view_mode_starts = text->raw_col;
            // print_int(where_view_mode_starts);
            // print_char('\n');
            memory_set(view_buffer, 0, 5000);
        }
        else if (strncmp(key_buffer, "save", 3) == 0 && view == 1) {
            // print_string("DA");
            // print_char('\n');

            u8 rest_chars[27];
            memory_set(rest_chars, 0, strlen(rest_chars));

            for (int i = 4; i < strlen(key_buffer); i++) {
                rest_chars[i - 5] = key_buffer[i];
            }
            rest_chars[strlen(rest_chars)] = '\0';

            // print_int(strlen(view_buffer));
            // print_int(strlen(key_buffer));

            u8 view_buffer_aux[5000] = {0};
            memory_copy(view_buffer, view_buffer_aux, strlen(view_buffer) - strlen(key_buffer) - 2);
            // memory_copy(view_buffer, view_buffer, strlen(view_buffer) - strlen(key_buffer));

            write_file(rest_chars, view_buffer_aux, strlen(view_buffer_aux));

            memory_set(view_buffer, 0, strlen(view_buffer));

            // print_string(rest_chars);
        }
        else if (strcmp(key_buffer, "exit") == 0) {
            view = 0;
            memory_set(view_buffer, 0, 5000);
        }
        // else if (strcmp(key_buffer, "pio") == 0 && view == 0) {
        //     memory_set(pio_buffer, 0, 600);
        //     ata_pio_read28(66, 1, pio_buffer);
        //     dump_sector_hex(pio_buffer, 512);
        //     u8 ok = '0';
        //     for (int i = 0; i < sizeof(pio_buffer); i++) {
        //         if (pio_buffer[i] != 0x00) {
        //             ok = '1';
        //         }
        //     }
        //     print_char(ok);

        // }
        // else if (strcmp(key_buffer, "testwrite") == 0 && view == 0) {
        //     char message[100] = "Hello!";
        //     memory_copy(message, pio_buffer, strlen(message));
        //     ata_pio_write28(50, 1, pio_buffer);
        //     print_string("Gata\n");
        //     ata_pio_read28(50, 1, pio_buffer);
        //     dump_sector_hex(pio_buffer, 32);
        // }
        else if (strcmp(key_buffer, "ls") == 0 && view == 0) {
            list_files("-l");
        }
        else if (strncmp(key_buffer, "rm", 2) == 0 && view == 0) {
            u8 rest_chars[27];
            memory_set(rest_chars, 0, strlen(rest_chars));

            for (int i = 3; i < strlen(key_buffer); i++) {
                rest_chars[i - 3] = key_buffer[i];
            }

            remove_file(rest_chars);
        }
        else if (strncmp(key_buffer, "cat", 3) == 0 && view == 0) {
            // print_string("DA");
            u8 rest_chars[27];
            memory_set(rest_chars, 0, strlen(rest_chars));

            for (int i = 4; i < strlen(key_buffer); i++) {
                rest_chars[i - 4] = key_buffer[i];
            }
            rest_chars[strlen(rest_chars)] = '\0';

            // print_string(rest_chars);

            // // memory_set(first_3_chars, 0, strlen(first_3_chars));
            // // memory_set(rest_chars, 0, strlen(rest_chars));

            read_file(rest_chars);
            print_char('\n');
        }
        else if (strcmp(key_buffer, "stopos") == 0 && view == 0) {
            asm("hlt");
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
                // if((text->raw_col / TEXT_COLS) * SCREEN_WIDTH + (text->raw_col % TEXT_COLS) > raw_row * 8) {
                if (where_view_mode_starts < text->raw_col) {
                    key_buffer[strlen(key_buffer) - 1] = '\0';
                    view_buffer[strlen(view_buffer) - 1] = '\0';
                    backspace();
                }
            }
        }
    }
    else if (ch != 0) {
        key_buffer[strlen(key_buffer)] = ch;
        key_buffer[strlen(key_buffer) + 1] = '\0';
        print_char(ch);

        if (view == 1) {
            view_buffer[strlen(view_buffer)] = ch;
            view_buffer[strlen(view_buffer) + 1] = '\0';

            // print_string(view_buffer);
            // print_char('\n');
            // print_char(ch);
        }
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
