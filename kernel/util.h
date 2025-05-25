#ifndef UTIL_H
#define UTIL_H

#include "../cpu/types.h"
#include "../cpu/timer.h"


void memory_copy(char* source, char* dest, int no_bytes);
void memory_set(char* dest, char val, int no_bytes);
int strlen(char* s);
void int_to_ascii(int n, char* str);
void sleep(u32 ms);
int strcmp(char *s1, char *s2);

#endif