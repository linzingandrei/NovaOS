#include "../cpu/types.h"
#include "screen.h"

#define CURSOR_BLINK_INTERVAL_MS 30 
#define CURSOR_CHAR '_'
#define CURSOR_COLOR 0x0F

extern u8 cursor_visible;
extern u32 last_cursor_blink_time;

void clear_cursor();
void draw_cursor();
void update_cursor_blink();
