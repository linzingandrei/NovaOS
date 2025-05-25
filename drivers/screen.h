#ifndef SCREEN_H
#define SCREEN_H

#include "../cpu/types.h"
#include "../kernel/util.h"
#include "ports.h"


#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define FONT_WIDTH    8
#define FONT_HEIGHT   8
#define TEXT_COLS     (SCREEN_WIDTH / FONT_WIDTH)
#define TEXT_ROWS     (SCREEN_HEIGHT / FONT_HEIGHT)

extern u8 font_bitmap[128][8];

typedef struct {
    u32 cursor_row;
    u32 cursor_col;
    u32 text_cols;
    u32 text_rows;
    u32 raw_col;
    u8 *vga_buffer;
    u8  fg_color;
    u8  bg_color;
} textStruct;

extern textStruct *text;

void screen_init();
void put_pixel(int x, int y, u8 color_index);
void screen_put();
void print_char(char c);
void backspace();
void print_string(char *str);
void draw_char_at(char c);
void clear_char_at(u32 raw_col);
void clear_screen();

#endif