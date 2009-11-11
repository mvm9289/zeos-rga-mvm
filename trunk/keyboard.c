
#include <keyboard.h>

LIST_HEAD(keyboardqueue);

char keyboard_buffer[KEYBUFF_SIZE];
int ini_pos = 0;
int end_pos = 0;
int buff_size = 0;

void buff_keyboard_insert(char c) {
    keyboard_buffer[end_pos] = c;
    buff_size++;
    end_pos++;
    if(end_pos == KEYBUFF_SIZE) end_pos=0;
}

char buff_keyboard_get_next() {
    char res;

    res = keyboard_buffer[ini_pos];
    buff_size--;
    ini_pos++;
    if(ini_pos == KEYBUFF_SIZE) ini_pos=0;

    return res;
}
