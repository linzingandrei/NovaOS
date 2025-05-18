#ifndef UTIL_H
#define UTIL_H

#include "../cpu/types.h"
#include "../cpu/timer.h"


void memory_copy(char* source, char* dest, int no_bytes);
int strlen(char* s);
void int_to_ascii(int n, char* str);
void sleep(u32 ms);

#endif