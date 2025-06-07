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

void list_files(u8 *options) {
    struct file_entry_t entries[MAX_FILES];

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_read28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (entries[i].used) {
            print_string(entries[i].name);
            if (strcmp(options, "-l") == 0) {
                print_string("   ");
                print_int(FS_START_SECTOR + entries[i].start_block + 1);
                print_string("   ");
                print_int(entries[i].byte_count);
                print_string(" bytes");
            }
            print_char('\n');
        }
    }
}

int write_file(u8 *file_name, u8 *file_data, u32 file_size) {
    // print_string(file_data);
    // print_char('\n');

    struct file_entry_t entries[MAX_FILES];
    u8 sector_buffer[BLOCK_SIZE + 1];

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_read28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    u32 free_space = -1;
    int nr = 0;
    int nr2 = 0;
    u32 sectors_needed = (file_size + 512) / 512;
    u32 sectors_needed_aux = sectors_needed;
    for (int i = 0; i < MAX_FILES; i++) {
        // if (i < 10) {
        //     print_string("i: ");
        //     print_hex8((u8)i);
        //     print_string("start_block: ");
        //     print_hex8(entries[i].start_block);
        //     print_char('\n');
        // }
        // u32 i_aux = i
        // for (int j = 0; j < sectors_needed; j++) {
        //     for (int k = i_aux; k < MAX_FILES; k++) {
        //         if (entries[k].used == 0)
        //             nr += 1
        //     }
        // }
        // else {
        //     nr2 += entries[i].byte_count;
        // }

        if (entries[i].used == 0) {
            sectors_needed -= 1;
            print_string("i: ");
            print_hex8((u8)i);
            print_char('\n');
            // if (i > 0)
            //     free_space = entries[i - 1].start_block;
            // else
            // free_space = i;// entries[i].start_block;

            // break;
        }
        else {
            sectors_needed = sectors_needed_aux;

        }

        if (sectors_needed == 0) {
            free_space = i - sectors_needed_aux + 1;
            break;
        }

        nr2 += (entries[i].byte_count + 512) / 512;
        // nr2 += entries[i].byte_count;
        nr += 1;

    }

    if (free_space == -1) {
        return 0;
    }

    print_string("nr: ");
    print_int(nr);
    print_char('\n');

    u32 start_block = sb.file_table_blocks + (nr2 - ((file_size + 512) / 512)) + 1;// + (nr2 + BLOCK_SIZE) / 512;   // 1 + sb.file_table_blocks;
    u32 num_blocks = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    print_string("num_blocks: ");
    print_int(num_blocks);
    print_char('\n');

    print_string("start_block: ");
    print_hex8((u8)start_block);
    print_char('\n');

    print_string("free_space: ");
    print_hex8((u8)free_space);
    print_char('\n');

    // print_string("file_data: ");
    // print_string(file_data);
    // print_char('\n');

    // print_string("file_size: ");
    // print_int(file_size);
    // print_char('\n');

    for (int i = 0; i < num_blocks; i++) {
        // memory_set(sector_buffer, 0, BLOCK_SIZE);
        for (int j = 0; j < BLOCK_SIZE; j++) {
            u32 index = i * BLOCK_SIZE + j;

            if (index < file_size) {
                sector_buffer[j] = file_data[index];
                // print_char(file_data[index]);

            }
            else
                sector_buffer[j] = 0x00;
        }

        // print_hex8(strlen(sector_buffer));
        // print_string(sector_buffer);
        print_int(FS_START_SECTOR + start_block + i + 1);
        print_char('\n');
        ata_pio_write28(FS_START_SECTOR + start_block + i + 1, 1, sector_buffer);
    }
    // ata_pio_write28(37, 1, file_data);

    for (int i = 0; i < MAX_NAME_LENGTH; i++) {
        entries[free_space].name[i] = file_name[i];
        
        if (file_name[i] == 0)
            break;
    }
    entries[free_space].start_block = start_block;
    entries[free_space].byte_count = file_size;
    entries[free_space].used = 1;

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_write28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    return 1;
}

int read_file(u8 *file_name) {
    struct file_entry_t entries[MAX_FILES];
    u8 sector_buffer[BLOCK_SIZE + 1]; // needs to be bigger than BLOCK_SIZE ???? If + 1 is removed it puts another file title which makes absolutely no sense. Some kind of overflow I believe

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_read28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    // print_string(file_name);
    // print_char('\n');

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
    // print_int(start_block_aux);
    // print_char('\n');
    // ata_pio_read28(FS_START_SECTOR + start_block_aux, 1, sector_buffer);
    // print_string(sector_buffer);

    u32 number_sectors = (byte_count_aux + BLOCK_SIZE) / 512;
    // print_int(number_sectors);
    // print_hex8((u8)(FS_START_SECTOR + start_block_aux));
    memory_set(sector_buffer, 0, sizeof(sector_buffer));
    // ata_pio_read28(FS_START_SECTOR + start_block_aux + 0 + 1, 1, sector_buffer);
    // print_string("sector buffer: ");
    // print_string((u8 *)sector_buffer);
    // print_int(strlen(sector_buffer));
    // print_char('\n');

    for (int i = 0; i < number_sectors; i++) {
        ata_pio_read28(FS_START_SECTOR + start_block_aux + i + 1, 1, sector_buffer);
        // print_string("sector buffer: ");
        // print_string(sector_buffer);
        // print_char('\n');

        for (int j = 0; j < strlen(sector_buffer); j++) {
            print_char(sector_buffer[j]);
        }

        memory_set(sector_buffer, 0, sizeof(sector_buffer));
    }

    return 0;
}

int edit_file(u8 *file_name, u8 *file_data, u8 file_size) {
    struct file_entry_t entries[MAX_FILES];
    u8 sector_buffer[BLOCK_SIZE];

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_read28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

}

int remove_file(u8 *file_name) {
    struct file_entry_t entries[MAX_FILES];
    u8 sector_buffer[BLOCK_SIZE + 1];
    
    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_read28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    u32 file_location = -1;
    u32 start_block = 0;
    u32 byte_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(entries[i].name, file_name) == 0 && entries[i].used == 1) {
            start_block = entries[i].start_block;
            byte_count = entries[i].byte_count;

            file_location = i;

            break;
        }
    }

    if (file_location == -1) {
        return 1;
    }

    int num_blocks = (byte_count + 512) / 512;
    for (int i = 0; i < num_blocks; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            u32 index = i * BLOCK_SIZE + j;

            sector_buffer[index] = 0;
        }

        ata_pio_write28(FS_START_SECTOR + start_block + i + 1, 1, sector_buffer);
    }

    memory_set(entries[file_location].name, 0, MAX_NAME_LENGTH);
    entries[file_location].byte_count = 0;
    entries[file_location].used = 0;

    for (int i = 0; i < FILE_TABLE_BLOCKS; i++) {
        ata_pio_write28(FS_START_SECTOR + i + 1, 1, &((u8 *)entries)[i * BLOCK_SIZE]);
    }

    return 0;
}