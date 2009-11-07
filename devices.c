
#include <devices.h>
#include <io.h>
#include <string.h>
#include <systemfile.h>

void init_devices() {
    int i;

    /* Initializes Keyboard */
    strcpy(DIR[0].name, "KEYBOARD");
    DIR[0].acces_mode = O_RDONLY;
    file_operations[0].sys_read_dev = sys_read_keyboard;
    file_operations[0].sys_write_dev = NULL;
    DIR[0].ops_indx = 0;

    /* Initializes Display */
    strcpy(DIR[1].name, "DISPLAY");
    DIR[1].acces_mode = O_WRONLY;
    file_operations[1].sys_read_dev = NULL;
    file_operations[1].sys_write_dev = sys_write_console;
    DIR[1].ops_indx = 1;

    /* Initializes Opened Files Table */
    for(i=0; i<CTABLE_SIZE*NR_TASKS; i++)
	    OFT[i].num_refs = 0;
}

int sys_write_console (int pos, const char *buffer, int size) {
    int i;
    for(i=0; i<size; i++)
        printc(buffer[i]);

    return i;
}

int sys_read_keyboard (int pos, char *buffer, int size) {
    return 0;
}
