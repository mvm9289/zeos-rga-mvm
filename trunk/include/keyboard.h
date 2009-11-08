#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#define KEYBUFF_SIZE 128

extern struct list_head keyboardqueue;
extern char keyboard_buffer[KEYBUFF_SIZE];
int ini_pos;
int end_pos;

#endif /* __KEYBOARD_H__ */
