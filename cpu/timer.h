#ifndef TIMER_H
#define TIMER_H

#include "irq.h"
#include "../kernel/util.h"
#include "../drivers/screen.h"


extern int timer_ticks;

void init_timer(u32 hz);
void install_timer();

#endif