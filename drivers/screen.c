#include "screen.h"
#include "ports.h"
#include "../kernel/util.h"


// Function declarations
///////////////////////////////////////////////////////////////////////////
int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int row, int col, char attr);
int get_screen_offset(int row, int col);
int get_offset_row(int offset);
int get_offset_col(int offset);
///////////////////////////////////////////////////////////////////////////


void kprint_at(char* message, int row, int col) {
    int offset;

    if (row >= 0 && col >= 0) {
        offset = get_screen_offset(row, col);
    } else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    int i = 0;
    while (message[i] != 0) {
        offset = print_char(message[i], row, col, WHITE_ON_BLACK);
        i += 1;

        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}


void kprint(char* message) {
    kprint_at(message, -1, -1);
}

void kputchar(char c) {
    static char str[2];
    str[0] = c;
    str[1] = '\0';
    kprint(str);
}

void kprint_backspace() {
    int offset = get_cursor_offset() - 2;

    int row = get_offset_row(offset);
    int col = get_offset_col(offset);

    print_char('\b', row, col, WHITE_ON_BLACK);
}

void clear_screen() {
    int row = 0;
    int col = 0;

    for (row = 0; row < MAX_ROWS; row++) {
        for (col = 0; col < MAX_COLS; col++) {
            print_char(' ', row, col, WHITE_ON_BLACK);
        }
    }

    set_cursor_offset(get_screen_offset(0, 0));
}


// Private kernel functions
/////////////////////////////////////////////////////////////////////////


int print_char(char character, int row, int col, char attribute_byte) {
    unsigned char* video_memory = (unsigned char *) VIDEO_ADDRESS;

    if (!attribute_byte) {
        attribute_byte = WHITE_ON_BLACK;
    }

    if (row >= MAX_ROWS || col >= MAX_COLS) {
        video_memory[2 * MAX_COLS * MAX_ROWS - 2] = 'E';
        video_memory[2 * MAX_COLS * MAX_ROWS - 1] = RED_ON_WHITE;
        
        return get_screen_offset(row, col);
    }

    int offset;
    if (row >= 0 && col >= 0) {
        offset = get_screen_offset(row, col);
    } else {
        offset = get_cursor_offset();
    }

    if (character == '\n') {
        int row = get_offset_row(offset);
        offset = get_screen_offset(row + 1, 0);
    }
    else if (character == '\b') {
        video_memory[offset] = ' ';
        video_memory[offset + 1] = attribute_byte;
    }
    else {
        video_memory[offset] = character;
        video_memory[offset + 1] = attribute_byte;
        offset += 2;
    }

    if (get_offset_row(offset) >= MAX_ROWS) {
        int i;

        for (int i = 1; i < MAX_ROWS; i++) {
            memory_copy(
                (char*)(get_screen_offset(i, 0) + VIDEO_ADDRESS),
                (char*)(get_screen_offset(i - 1, 0) + VIDEO_ADDRESS),
                MAX_COLS * 2
            );
        }

        char *last_line = (char *)(get_screen_offset(MAX_ROWS - 1, 0) + VIDEO_ADDRESS);
        for (int i = 0; i < MAX_COLS * 2; i += 2) {
            last_line[i] = 0x20;
            last_line[i + 1] = attribute_byte;
        }

        offset -= MAX_COLS * 2;
    }

    set_cursor_offset(offset);
    return offset;
}


int get_offset_row(int offset) {
    return offset / (2 * MAX_COLS);
}


int get_offset_col(int offset) {
    return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2;
}


int get_screen_offset(int row, int col) {
    return 2 * (row * MAX_COLS + col);
}

int get_cursor_offset() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}

void set_cursor_offset(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char) (offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char) (offset & 0xff));
}
