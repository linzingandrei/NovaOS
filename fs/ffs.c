#include "ffs.h"
#include "../kernel/util.h"
#include "../cpu/pio.h"


struct superblock_t sb;
void format(u32 total_blocks) {
    u8 buffer[512];

    sb.magic             = FS_MAGIC;
    sb.block_size        = 512; 
    sb.total_blocks      = total_blocks;
    sb.file_table_start  = FS_START_SECTOR;
    sb.file_table_blocks = FILE_TABLE_BLOCKS;

    for (int i = 0; i < 492; i++) {
        sb.reserved[i] = 0;
    }

    memory_copy((char *)&sb, buffer, sizeof(sb));
    ata_pio_write28(FS_START_SECTOR, 1, buffer);    

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        for (int j = 0; j < 512; j++) {
            buffer[j] = 0x00;
        }

        ata_pio_write28(FS_START_SECTOR + i + 1, 1, buffer);
    }
}

void list_files() {
    struct file_entry_t entries[MAX_FILES];

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_read28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (entries[i].used) {
            print_string(entries[i].name);
            print_char('\n');
        }
    }
}

int write_file(u8 *file_name, u8 *file_data, u8 file_size) {
    struct file_entry_t entries[MAX_FILES];
    u8 sector_buffer[BLOCK_SIZE];

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_read28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    u32 free_space = -1;
    int nr = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (entries[i].used != 1) {
            // print_hex8((u8)i);
            if (i > 0)
                free_space = entries[i - 1].start_block;
            else
                free_space = entries[i].start_block;

            // print_hex8((u8)free_space);
            break;
        }
        nr += 1;
    }

    if (free_space == -1) {
        return 0;
    }
    // print_hex8((u8)free_space);

    u32 start_block = sb.file_table_blocks + 1 + nr;   // 1 + sb.file_table_blocks;
    u32 num_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // print_hex8((u8)start_block);

    for (int i = 0; i < num_blocks; i++) {
        memory_set(sector_buffer, 0, BLOCK_SIZE);
        for (int j = 0; j < BLOCK_SIZE; j++) {
            u32 index = i * BLOCK_SIZE + j;

            if (index < file_size)
                sector_buffer[j] = file_data[index];
            else
                sector_buffer[j] = 0x00;
        }

        // print_hex8(strlen(sector_buffer));
        // print_string(sector_buffer);
        ata_pio_write28(FS_START_SECTOR + start_block + i, 1, sector_buffer);
    }
    // ata_pio_write28(37, 1, file_data);

    for (int i = 0; i < MAX_NAME_LENGTH; i++) {
        entries[free_space].name[i] = file_name[i];
        
        if (file_name[i] == 0)
            break;
    }
    entries[free_space].start_block = start_block * num_blocks;
    entries[free_space].byte_count = file_size;
    entries[free_space].used = 1;

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_write28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    return 1;
}

int read_file(u8 *file_name) {
    struct file_entry_t entries[MAX_FILES];
    u8 sector_buffer[BLOCK_SIZE];

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_read28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    u32 start_block_aux = 0;
    u32 byte_count_aux = 0;
    int ok = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (entries[i].used == 1 && strcmp(entries[i].name, file_name) == 0) {
            start_block_aux = entries[i].start_block;
            byte_count_aux = entries[i].byte_count;

            ok = 1;
        }
    }

    if (ok == 0) {
        return 1;
    }

    // print_hex8((u8)number_sectors);
    // print_hex8((u8)start_block_aux);
    // ata_pio_read28(FS_START_SECTOR + start_block_aux, 1, sector_buffer);
    // print_string(sector_buffer);

    u32 number_sectors = (byte_count_aux + BLOCK_SIZE) / 512;
    // print_hex8((u8)number_sectors);
    // print_hex8((u8)(FS_START_SECTOR + start_block_aux));
    memory_set(sector_buffer, 0, sizeof(sector_buffer));

    for (int i = 0; i < number_sectors; i++) {
        ata_pio_read28(FS_START_SECTOR + start_block_aux + i, 1, sector_buffer);

        for (int j = 0; j < strlen(sector_buffer); j++) {
            print_char(sector_buffer[j]);
        }

        memory_set(sector_buffer, 0, sizeof(sector_buffer));
    }

    return 0;
}