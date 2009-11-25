
#include <filesystem.h>
#include <devices.h>

int initZeOSFat() {
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

    return 0;
}

int Alloc_Block() {
    int retornar = free_block;

    if(free_block != EOF) {
        free_block = ZeOSFAT[free_block];

        return retornar;
    }

    return -1;
}

void Free_Blocks(int block){
    int oldFB = free_block;

    free_block = block;
    while(ZeOSFAT[block] != EOF) block = ZeOSFAT[block];
    ZeOSFAT[block] = oldFB;
}
