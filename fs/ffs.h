#ifndef SFS_H
#define SFS_H


#include "../cpu/types.h"

#define FS_MAGIC            0xCAFE
#define MAX_NAME_LENGTH     16
#define FS_START_SECTOR     32 
#define MAX_FILES           64
#define FILE_TABLE_BLOCKS   4
#define BLOCK_SIZE          512

struct superblock_t {
    u32     magic;
    u32     block_size;
    u32     total_blocks;
    u32     file_table_start;
    u32     file_table_blocks;
    u8      reserved[492];
} __attribute__((packed));

struct file_entry_t {
    u8      name[MAX_NAME_LENGTH];
    u32     start_block;
    u32     byte_count;
    u8      used;
    u8      reserved[7];
} __attribute__((packed));

void format(u32 total_blocks);
void list_files(u8 *options);
int write_file(u8 *file_name, u8 *file_data, u32 file_size);
int read_file(u8 *file_name);
int edit_file(u8 *file_name, u8 *file_data, u8 file_size);
int remove_file(u8 *file_name);

#endif