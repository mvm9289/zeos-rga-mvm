
#include <devices.h>
#include <io.h>
#include <string.h>
#include <keyboard.h>

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

    if(!list_is_last(keyboardqueue.next, &keyboardqueue) ||
      buff_size < size) {
        list_del(&(current()->rq_list));
        list_add_tail(&(current()->rq_list), &keyboardqueue);
    }
    else
        for(i=0; i<size; i++)
            buffer[i] = buff_keyboard_get_next();  
    
    return size;
}
