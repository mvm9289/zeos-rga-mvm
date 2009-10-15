/*
 * io.c - 
 */

#include <io.h>
#include <utils.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=15;
unsigned char dirty_line = 1;

/* Read a byte from 'port' */
Byte inb (unsigned short port)
{
    Byte v;

    __asm__ __volatile__ ("inb %w1,%0":"=a" (v):"Nd" (port));
    return v;
}

void erase_line(int line) {
    DWord scr1 = 0xb8000 + 2*line*NUM_COLUMNS;
    Word ch_aux=(Word) (' ' & 0x00FF) | 0x200;
    int i;

    for(i=0; i < NUM_COLUMNS; i++) {
        asm("movw %0, (%1)":: "g" (ch_aux), "g" (scr1));
        scr1 += 2;
    }
}

void scroll_screen(void) {
    DWord scr1 = 0xb8000 + NUM_COLUMNS*2;
    DWord scr2 = scr1 + NUM_COLUMNS*2;

    if(dirty_line) {
        erase_line(0);
        dirty_line = 0;
    }
    copy_data((void *)scr2, (void *)scr1, (23*NUM_COLUMNS + 79)*2);
    erase_line(23);
}

void printc(char c)
{
    Word ch;
    DWord screen;

    if(c=='\n') {
        x = 0;
        if (++y >= NUM_ROWS-1) {
            scroll_screen();
            y = NUM_ROWS-2;
        }
    }
    else {
        if (x >= NUM_COLUMNS)
        {
            x = 0;
            if (++y >= NUM_ROWS-1) {
                scroll_screen();
                y = NUM_ROWS-2;
            }
        }

        ch = (Word) (c & 0x00FF) | 0x0200;
        screen = 0xb8000 + (y * NUM_COLUMNS + x++) * 2;

        __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c));

        asm("movw %0, (%1)" : : "g"(ch), "g"(screen));
    }
}

void printk(char *string)
{
    int i;
    for (i = 0; string[i]; i++)
        printc(string[i]);
}

void printc_xy(int x, int y, char c) {
    Word ch = (Word) (c & 0x00FF) | 0x0200;
    DWord screen = 0xb8000 + (y * NUM_COLUMNS + x) * 2;

    __asm__ __volatile__ ( "movb %0, %%al; outb $0xe9" ::"a"(c));

    asm("movw %0, (%1)" : : "g"(ch), "g"(screen));
}

void printk_xy(int x, int y, char *string)
{
    int i;
    for (i = 0; string[i]; i++)
        printc_xy(x+i,y,string[i]);
}
