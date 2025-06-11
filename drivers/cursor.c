#include "cursor.h"


u8 cursor_visible = 0;
u32 last_cursor_blink_time = 0;
u8 char_at_pos = '\0'; 
u8 char_at_pos_aux = '\0';

void clear_cursor() {
    // clear_char_at(text->raw_col);
    clear_char_at(text->raw_col, text->bg_color);
    if (text->raw_col < max_raw_col)
        draw_char_at(get_char_at(text->raw_col), text->fg_color);
    // char_at_pos_aux = '\0';
}

void draw_cursor() {
    // draw_char_at(CURSOR_CHAR);
    draw_char_at(CURSOR_CHAR, text->fg_color);
}

void update_cursor_blink() {
    if (timer_ticks - last_cursor_blink_time >= CURSOR_BLINK_INTERVAL_MS) {
        if (cursor_visible) {
            clear_cursor();
        } else {
            draw_cursor();
            // char_at_pos_aux = get_char_at(text->raw_col);
        }
        cursor_visible = !cursor_visible;
        last_cursor_blink_time = timer_ticks;
    }
}
