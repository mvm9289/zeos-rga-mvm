
#include <devices.h>
#include <openmodes.h>
#include <io.h>
#include <string.h>
#include <keyboard.h>
#include <list.h>
#include <filesystem.h>
#include <errno.h>

void init_devices() { //MIRAR DE HACER CON OPENS
    int i;

    /* Initializes Keyboard */
    strcpy(DIR[0].name, "KEYBOARD");
    DIR[0].nb_refs = 0;
    DIR[0].access_mode = O_RDONLY;
    file_ops[0].sys_open_dep = sys_open_keyboard;
    file_ops[0].sys_read_dep = sys_read_keyboard;  // warning pk no cuadran los parámetros
    file_ops[0].sys_write_dep = NULL;
    file_ops[0].sys_release_dep = sys_release_std;
    file_ops[0].sys_unlink_dep = sys_unlink_dev;
    DIR[0].ops = &file_ops[0];

    /* Initializes Display */
    strcpy(DIR[1].name, "DISPLAY");
    DIR[1].nb_refs = 0;
    DIR[1].access_mode = O_WRONLY;
    file_ops[1].sys_open_dep = sys_open_console;
    file_ops[1].sys_read_dep = NULL;
    file_ops[1].sys_write_dep = sys_write_console; // warning pk no cuadran los parámetros
    file_ops[1].sys_release_dep = sys_release_std;
    file_ops[1].sys_unlink_dep = sys_unlink_dev;
    DIR[1].ops = &file_ops[1];

    /* Initializes Opened Files Table */
    for(i=0; i<CTABLE_SIZE*NR_TASKS; i++)
	    OFT[i].num_refs = 0;
}

inline int pathlen_isOK(const char *path) {
    int i = 0;

    while(path[i] != '\0') {
        ++i;
        if(i >= FILE_NAME_SIZE) return 0;
    }

    return 1;
}

inline struct logic_device* searchFile(char *name) {
	int i;

	for(i = 0; i < DIR_ENTRIES; ++i)
		if(!DIR[i].free && strcmp(name,DIR[i].name)) return &DIR[i];

	return NULL;
}	

inline struct logic_device* createFile(char *name, int flags)
{
	int i;
	
	for(i = 0; i < DIR_ENTRIES; ++i)
	{
		if(DIR[i].free)
		{
			DIR[i].free = 0;
			DIR[i].nb_refs = 0;
			DIR[i].access_mode = flags; // flags&0x03 del open o O_RDWR como en prosofib?
			DIR[i].size = 0;
			strcpy(DIR[i].name, name);
			if(free_block != EOF)
			{
				DIR[i].firstBlock = free_block;
				free_block = ZeOSFAT[free_block];
				ZeOSFAT[DIR[i].firstBlock] = EOF;
			}else return NULL; // se debería diferenciar entre el return de abajo y este...
			file_ops[i].sys_open_dep = sys_open_file; // ??
			file_ops[i].sys_read_dep = sys_read_file;
			file_ops[i].sys_write_dep = sys_write_file; // warning pk no cuadran los parámetros
			file_ops[i].sys_release_dep = sys_release_std;
			file_ops[i].sys_unlink_dep = sys_unlink_file;
			DIR[i].ops = &file_ops[i];
			return &DIR[i];
		}
	}
	
	return NULL;
}			


inline int getFreeChannel(struct channel *channels) {
	int i;

	for(i = 0; i < CTABLE_SIZE; ++i)
		if(channels[i].free) return i;

	return -1;
}

inline struct OFT_item* getNewOpenedFile() {
	int i;

	for(i = 0; i < CTABLE_SIZE*NR_TASKS; ++i)
		if(!OFT[i].num_refs) return &OFT[i];

	return NULL;
}

int sys_open_console (const char *path, int flags) {
    int new_fd;
    return new_fd;
}

int sys_write_console (int *pos, const char *buffer, int size) {
    int i;
    for(i=0; i<size; i++)
        printc(buffer[i]);

    return i;
}

int sys_open_keyboard (const char *path, int flags) {
    int new_fd;
    return new_fd;
}

int sys_read_keyboard (int *pos, char *buffer, int size) {
    int i;
    union task_union *t;

    if(!list_empty(&keyboardqueue) || buff_size < size) {
        /* Update Current Task Struct */
        t=(union task_union *)current();
        t->task.request_chars_to_keyboard = size;
        t->task.buff_location=buffer;
        t->stack[KERNEL_STACK_SIZE-10]=size;
        
        /* Block Current Process */
        list_del(&(current()->rq_list));
        list_add_tail(&(current()->rq_list), &keyboardqueue);

        /* Select New Process to Run */
        t=(union task_union *) list_head_to_task_struct(runqueue.next);
        life=t->task.quantum;
        t->task.nbtrans++;
        t->task.remaining_life=life;
        task_switch(t);
    }
    else
        for(i=0; i<size; i++)
            buffer[i] = buff_keyboard_get_next();  
    
    return size;
}

int sys_unlink_dev (struct logic_device *file) {
    file->free=1;
    
    return 0;
}

int sys_open_file(const char *path, int flags) { // para que? quien? como?
    
}

int sys_read_file(int fd, char *buffer, int size) 
{
	struct OFT_item *OFTfile = current()->channel_table[fd].opened_file;
	int nblock = OFTfile->seq_pos / BLOCK_SIZE;
	int blockPos = OFTfile->seq_pos % BLOCK_SIZE;
	int block = OFTfile->file->firstBlock;
	int i;
    int bytes = 0;

	if((OFTfile->seq_pos + size) > OFTfile->file->size) return -1; // MIRAR ERROR

	for(i = 0; i < nblock; ++i) block = ZeOSFAT[block];

	for(i = 0; i < size; ++i)
	{
		buffer[i] = HardDisk[block][blockPos];
		++blockPos;
		++OFTfile->seq_pos;		
        ++bytes;           
 
		if(blockPos == 256)
		{
			blockPos = 0;
			block = ZeOSFAT[block];
		}
	}
    return bytes;				
}

int sys_write_file(int fd, char *buffer, int size) 
{
	struct OFT_item *OFTfile = current()->channel_table[fd].opened_file;
	int nblock = OFTfile->seq_pos / BLOCK_SIZE;
	int blockPos = OFTfile->seq_pos % BLOCK_SIZE;
	int i;
    int bytes = 0;
	int block = OFTfile->file->firstBlock;

	for(i = 0; i < nblock; ++i) block = ZeOSFAT[block];

	for(i = 0; i < size; ++i)  //se puede hacer con strcpy iendo con cuidado?
	{
		HardDisk[block][blockPos] = buffer[i];
		++blockPos;
		++OFTfile->seq_pos;
        if(OFTfile->seq_pos > OFTfile->file->size) ++OFTfile->file->size; // MIRAR BIEN
        ++bytes;		

		if(blockPos == 256) // y si ya tenía reservado ese espacio??
		{
			block = getFreeBlock(block);
			if(block == -1) return -1; // mirar error
			blockPos = 0;
		}
	}
    return bytes;		 
}

int sys_unlink_file(struct logic_device *file) {
    
}

int sys_release_std (struct logic_device *file) {
    file->nb_refs--;

    return 0;
}