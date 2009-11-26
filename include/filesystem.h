#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <types.h>

#define MAX_BLOCKS 50
#define BLOCK_SIZE 256
#define EOF -1
#define NON_BLOCK -1

int ZeOSFAT[MAX_BLOCKS];
Byte HardDisk[MAX_BLOCKS][BLOCK_SIZE];
int free_block;

int initZeOSFat();
inline int Alloc_Block();
inline void Free_Blocks(int block);

#endif
