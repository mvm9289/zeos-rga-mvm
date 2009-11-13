
#include <filesystem.h>
#include <devices.h>

void init_ZeOSFAT() {
    int i, j;

    free_block = 0;
    for(i = 0; i < MAX_BLOCKS-1; ++i)
        ZeOSFAT[i] = i+1;
    ZeOSFAT[MAX_BLOCKS-1] = EOF;

    for(i = 0; i < DIR_ENTRIES; ++i)
        DIR[i].free = 1;

    /* Initalize Hard Disk Blocks */
    for (i = 0; i < MAX_BLOCKS; i++)
        for (j = 0; j < BLOCK_SIZE; j++)
            HardDisk[i][j] = 0;
}

