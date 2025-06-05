#ifndef SFS_H
#define SFS_H


#include "../cpu/types.h"

#define MAX_NAME_LENGTH 16

struct file_entry_t {
    char    name[MAX_NAME_LENGTH];
    u32     start_block;
    u32     byte_count;
    u8      used;

} __attribute__((packed));


#endif