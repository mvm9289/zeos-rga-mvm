#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <list.h>

#define KEYBUFF_SIZE 128

extern struct list_head keyboardqueue;

extern char keyboard_buffer[KEYBUFF_SIZE];
extern int ini_pos;
extern int end_pos;
extern int buff_size;

void buff_keyboard_insert(char c);
char buff_keyboard_get_next();

#endif /* __KEYBOARD_H__ */
