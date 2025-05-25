#include "cursor.h"


u8 cursor_visible = 0;
u32 last_cursor_blink_time = 0;

void clear_cursor() {
    clear_char_at(text->raw_col);
}

void draw_cursor() {
    draw_char_at(CURSOR_CHAR);
}

void update_cursor_blink() {
    if (timer_ticks - last_cursor_blink_time >= CURSOR_BLINK_INTERVAL_MS) {
        if (cursor_visible) {
            clear_cursor();
        } else {
            draw_cursor();
        }
        cursor_visible = !cursor_visible;
        last_cursor_blink_time = timer_ticks;
    }
}
