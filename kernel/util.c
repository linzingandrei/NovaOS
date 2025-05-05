#include "util.h"


void memory_copy(char* source, char* dest, int no_bytes) {
    int i;
    
    for (int i = 0; i < no_bytes; i++) {
        *(dest + i) = *(source + i);
    }
}
