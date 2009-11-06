#ifndef DEVICES_H__
#define  DEVICES_H__

void init_devices();
int sys_write_console(int pos, const char *buffer,int size);
int sys_read_keyboard(int pos, char *buffer, int size);

#endif /* DEVICES_H__*/
