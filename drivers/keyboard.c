#include "keyboard.h"

#define KB_DATA_PORT   0x60
#define KB_STATUS_PORT 0x64


static inline u8 inb(u16 port) {
    u8 ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static s8 key_buffer[256] = {0};

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

s8 keyboard_poll_char() {
    u8 scancode;

    while (!(inb(KB_STATUS_PORT) & 1));

    scancode = inb(KB_DATA_PORT);

    if (scancode & 0x80)
        return 0;
    if (scancode < 128)
        return scancode_ascii[scancode];
    return 0;
}

void keyboard_input_loop() {
    while (1) {
        s8 ch = keyboard_poll_char();

        if (ch == '\n') {
            key_buffer[0] = '\0';
            kprint("\n");
        }
        else if (ch == '\b') {
           key_buffer[strlen(key_buffer) - 1] = '\0';
           kprint_backspace();
        }
        else if (ch != 0) {
            key_buffer[strlen(key_buffer)] = ch;
            key_buffer[strlen(key_buffer) + 1] = '\0';
            kputchar(ch);
        }

        sleep(30);
    }
}
