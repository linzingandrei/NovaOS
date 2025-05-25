#include "util.h"


void memory_copy(char* source, char* dest, int no_bytes) {
    int i;
    
    for (int i = 0; i < no_bytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(char* dest, char val, int no_bytes) {
    char *temp = (char *) dest;
    for (; no_bytes != 0; no_bytes--) {
        *temp++ = val;
    }
}

int strlen(char* s) {
    int i = 0;

    while (s[i] != '\0') i++;
        return i;
}

void int_to_ascii(int n, char* str) {
    int i, sign;

    if ((sign = n) < 0)
        n = -n;

    i = 0;
    do {
        str[i++] = (n % 10) + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) {
        str[i++] = '-';
    }
    str[i] = '\0';
}

void sleep(u32 ms) {
    u32 ticks = ms / 10;

    u32 start_tick = timer_ticks;
    while ((timer_ticks - start_tick) < ticks) {}
}

int strcmp(char *s1, char *s2) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}