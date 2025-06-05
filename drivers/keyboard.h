#include "../cpu/types.h"
#include "screen.h"
#include "../kernel/util.h"
#include "cursor.h"
#include "../cpu/irq.h"
#include "../cpu/pio.h"

static inline u8 inb(u16 port);
s8 keyboard_get_char();
void keyboard_input(struct registers_t *regs);
void install_keyboard();
// s8 keyboard_poll_char();
// void keyboard_input_loop();
