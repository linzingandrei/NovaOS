#include "types.h"
#include "screen.h"
#include "../libc/string.h"

static inline u8 inb(u16 port);
s8 keyboard_poll_char();
void keyboard_input_loop();
