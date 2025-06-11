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

int strncmp(char *s1, char *s2, int n) {
    int i;
    for (i = 0; i < n && s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    if (i == n)
        return 0;
    return s1[i] - s2[i];
}

char hex_digit(u8 value) {
    return (value < 10) ? ('0' + value) : ('A' + (value - 10));
}

void print_int(u32 value) {
    if (value == 0) {
        print_char('0');
        print_char('\n');
        return;
    }

    u8 result[100];
    int i = 0;

    while (value > 0) {
        result[i++] = (value % 10) + '0';
        value /= 10;
    }

    while (i > 0) {
        i -= 1;
        print_char(result[i]);
    }
    // print_char('\n');
}


void print_hex8(u8 value) {
    char hex[5];
    hex[0] = '0';
    hex[1] = 'x';
    hex[2] = hex_digit((value >> 4) & 0xF);
    hex[3] = hex_digit(value & 0xF);
    hex[4] = '\0';
    print_string(hex);
}

void dump_sector_hex(u8 *buf, u16 cnt) {
    for (int i = 0; i < cnt; i++) {
        print_hex8(buf[i]);
        print_string(" ");
        if ((i + 1) % 16 == 0) {
            print_string("\n");
        }
    }
}

void append(u8 *s, u8 val) {
    int n = strlen(s);
    s[n] = val;
    s[n + 1] = '\0';
}