
#include <filesystem.h>
#include <devices.h>

int initZeOSFAT() { //MIRAR EL ERROR!!!!
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

int Alloc_Block()
{
	int retornar = free_block;
	if(free_block != EOF)
	{
		free_block = ZeOSFAT[free_block];

		return retornar;
	}
	return -1;
}

int Free_Blocks(int block)
{ // para hacerlo facil podríamos poner el free_block al primero del fichero y el ultimo de este fichero enlazarlo con el free_block antiguo. free_block no sería el primer bloque libre... pero sería un bloque libre y estaría todo consistente. Lo que quizá es guarro.

    int oldFB = free_block;
    free_block = block;
    int i = block;
    while(ZeOSFAT[i] != EOF) i = ZeOSFAT[i];
    ZeOSFAT[i] = free_block;

}

