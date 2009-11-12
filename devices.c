
#include <devices.h>
#include <openmodes.h>
#include <io.h>
#include <string.h>
#include <keyboard.h>
#include <list.h>

void init_devices() {
    int i;

    /* Initializes Keyboard */
    strcpy(DIR[0].name, "KEYBOARD");
    DIR[0].access_mode = O_RDONLY;
    file_ops[0].sys_read_dep = sys_read_keyboard;
    file_ops[0].sys_write_dep = NULL;
    DIR[0].ops = &file_ops[0];

    /* Initializes Display */
    strcpy(DIR[1].name, "DISPLAY");
    DIR[1].access_mode = O_WRONLY;
    file_ops[1].sys_read_dep = NULL;
    file_ops[1].sys_write_dep = sys_write_console;
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

inline struct logic_device* getFile(char *name) {
	int i;

	for(i = 0; i < DIR_ENTRIES; ++i)
		if(strcmp(name,DIR[i].name)) return &DIR[i];

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

int sys_write_console (int *pos, const char *buffer, int size) {
    int i;
    for(i=0; i<size; i++)
        printc(buffer[i]);

    return i;
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
