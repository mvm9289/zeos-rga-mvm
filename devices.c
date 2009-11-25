
#include <devices.h>
#include <openmodes.h>
#include <io.h>
#include <string.h>
#include <keyboard.h>
#include <list.h>
#include <filesystem.h>
#include <errno.h>

void init_devices() {
    int i;

    /* Initializes Keyboard */
    strcpy(DIR[0].name, "KEYBOARD");
    DIR[0].free = 0;
    DIR[0].nb_refs = 0;
    DIR[0].access_mode = O_RDONLY;
    DIR[0].firstBlock = NON_BLOCK;
    file_ops[0].sys_open_dep = NULL;
    file_ops[0].sys_read_dep = sys_read_keyboard;
    file_ops[0].sys_write_dep = NULL;
    file_ops[0].sys_release_dep = NULL;
    file_ops[0].sys_unlink_dep = NULL;
    DIR[0].ops = &file_ops[0];

    /* Initializes Display */
    strcpy(DIR[1].name, "DISPLAY");
    DIR[1].free = 0;
    DIR[1].nb_refs = 0;
    DIR[1].access_mode = O_WRONLY;
    DIR[1].firstBlock = NON_BLOCK;
    file_ops[1].sys_open_dep = NULL;
    file_ops[1].sys_read_dep = NULL;
    file_ops[1].sys_write_dep = sys_write_console;
    file_ops[1].sys_release_dep = NULL;
    file_ops[1].sys_unlink_dep = NULL;
    DIR[1].ops = &file_ops[1];

    /* Initializes Open Files Table */
    for(i=0; i<OFT_MAX_SIZE; i++)
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

inline struct logic_device* searchFile(const char *name) {
    int i;

    for(i = 0; i < DIR_ENTRIES; ++i)
        if(!DIR[i].free && strcmp(name,DIR[i].name)) return &DIR[i];

    return NULL;
}	

inline struct logic_device* createFile(const char *name) {
    int i;

    for(i = 0; i < DIR_ENTRIES; ++i) {
        if(DIR[i].free) {
            DIR[i].free = 0;
            DIR[i].nb_refs = 0;
            DIR[i].access_mode = O_RDWR;
            DIR[i].size = 0;
            strcpy(DIR[i].name, name);
            DIR[i].firstBlock = free_block;
            free_block = ZeOSFAT[free_block];
            ZeOSFAT[DIR[i].firstBlock] = EOF;
            file_ops[i].sys_open_dep = sys_open_file;
            file_ops[i].sys_read_dep = sys_read_file;
            file_ops[i].sys_write_dep = sys_write_file;
            file_ops[i].sys_release_dep = sys_release_file;
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

    for(i = 0; i < OFT_MAX_SIZE; ++i)
        if(!OFT[i].num_refs) return &OFT[i];

    return NULL;
}

int sys_write_console (void *OFTitem, const char *buffer, int size) {
    int i;

    for(i=0; i<size; i++) printc(buffer[i]);

    return i;
}

int sys_read_keyboard (void *OFTitem, char *buffer, int size) {
    int i;
    union task_union *t;

    if(current()->Pid == 0) return -EPERM;

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

int sys_open_file(struct logic_device *file) {
    file->nb_refs++;

    return 0;
}

int sys_read_file(void *OFTitem, char *buffer, int size) {
    struct OFT_item *opened_file = (struct OFT_item *)OFTitem;
    struct logic_device *file = opened_file->file;
	int nblock = opened_file->seq_pos / BLOCK_SIZE;
	int blockPos = opened_file->seq_pos % BLOCK_SIZE;
	int block = file->firstBlock;
	int i;
    int bytes = 0;

    for(i = 0; i < nblock; ++i) block = ZeOSFAT[block];

    for(i = 0; (i < size) && (opened_file->seq_pos < file->size); ++i) {
        buffer[i] = HardDisk[block][blockPos];
        ++blockPos;
        ++opened_file->seq_pos;		
        ++bytes;           

        if(blockPos == 256) {
            blockPos = 0;
            block = ZeOSFAT[block];
        }
    }

    return bytes;		
}

int sys_write_file(void *OFTitem, const char *buffer, int size) {
    struct OFT_item *opened_file = (struct OFT_item *)OFTitem;
    struct logic_device *file = opened_file->file;
    int nblock = opened_file->seq_pos / BLOCK_SIZE;
    int blockPos = opened_file->seq_pos % BLOCK_SIZE;
    int i;
    int bytes = 0;
    int block = file->firstBlock;
    int newBlock;

    for(i = 0; i < nblock; ++i) block = ZeOSFAT[block];

    for(i = 0; i < size; ++i) {
        HardDisk[block][blockPos] = buffer[i];
        ++blockPos;
        ++opened_file->seq_pos;
        if(opened_file->seq_pos > file->size) ++file->size;
        ++bytes;		

        if(blockPos == BLOCK_SIZE) {
            if(ZeOSFAT[block] == EOF) {
                newBlock = Alloc_Block();
                if(newBlock == -1) return -ENOSPC;
                ZeOSFAT[block] = newBlock;
                ZeOSFAT[newBlock] = EOF;
                block = newBlock;
            }
            else block = ZeOSFAT[block];
            blockPos = 0;
        }
    }

    return bytes;
}

int sys_unlink_file(struct logic_device *file) {
    if(file->nb_refs > 0) return -EBUSY;

    Free_Blocks(file->firstBlock);
    file->free = 1;

    return 0;
}

int sys_release_file (struct logic_device *file) {
    file->nb_refs--;

    return 0;
}
