/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <entry.h>
#include <io.h>
#include <string.h>
#include <sched.h>

#define KERNEL_LEVEL 0
#define USER_LEVEL 3

#define X_CLOCK 75
#define Y_CLOCK 0

#define X_KEY 73
#define Y_KEY 24

int ticks = 0;
int secs = 0;

Gate idt[IDT_ENTRIES];
Register    idtR;

char char_map[] =
    {
        '\0','\0','1','2','3','4','5','6',
        '7','8','9','0','\'','¡','\0','\0',
        'q','w','e','r','t','y','u','i',
        'o','p','`','+','\0','\0','a','s',
        'd','f','g','h','j','k','l','ñ',
        '\0','º','\0','ç','z','x','c','v',
        'b','n','m',',','.','-','\0','*',
        '\0','\0','\0','\0','\0','\0','\0','\0',
        '\0','\0','\0','\0','\0','\0','\0','7',
        '8','9','-','4','5','6','+','1',
        '2','3','0','\0','\0','\0','<','\0',
        '\0','\0','\0','\0','\0','\0','\0','\0',
        '\0','\0'
    };

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
    /***********************************************************************/
    /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
    /* ***************************                                         */
    /* flags = x xx 0x110 000 ?????                                        */
    /*         |  |  |                                                     */
    /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
    /*         |   \ DPL = Num. higher PL from which it is accessible      */
    /*          \ P = Segment Present bit                                  */
    /***********************************************************************/
    Word flags = (Word)(maxAccessibleFromPL << 13);
    flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

    idt[vector].lowOffset       = lowWord((DWord)handler);
    idt[vector].segmentSelector = __KERNEL_CS;
    idt[vector].flags           = flags;
    idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
    /***********************************************************************/
    /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
    /* ********************                                                */
    /* flags = x xx 0x111 000 ?????                                        */
    /*         |  |  |                                                     */
    /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
    /*         |   \ DPL = Num. higher PL from which it is accessible      */
    /*          \ P = Segment Present bit                                  */
    /***********************************************************************/
    Word flags = (Word)(maxAccessibleFromPL << 13);

    //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
    /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
       the system calls will be thread-safe. */
    flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

    idt[vector].lowOffset       = lowWord((DWord)handler);
    idt[vector].segmentSelector = __KERNEL_CS;
    idt[vector].flags           = flags;
    idt[vector].highOffset      = highWord((DWord)handler);
}
void setIdt()
{
    /* Program interrups/exception service routines */
    idtR.base  = (DWord)idt;
    idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;

    /* EXCEPTION INITIALIZATION */
    setInterruptHandler(0, divide_error_handler, KERNEL_LEVEL);
    setInterruptHandler(1, debug_handler, USER_LEVEL);
    setInterruptHandler(2, nm1_handler, KERNEL_LEVEL);
    setInterruptHandler(3, breakpoint_handler, USER_LEVEL);
    setInterruptHandler(4, overflow_handler, KERNEL_LEVEL);
    setInterruptHandler(5, bounds_check_handler, KERNEL_LEVEL);
    setInterruptHandler(6, invalid_opcode_handler, KERNEL_LEVEL);
    setInterruptHandler(7, device_not_available_handler, KERNEL_LEVEL);
    setInterruptHandler(8, double_fault_handler, KERNEL_LEVEL);
    setInterruptHandler(9, coprocessor_segment_overrun_handler, KERNEL_LEVEL);
    setInterruptHandler(10, invalid_tss_handler, KERNEL_LEVEL);
    setInterruptHandler(11, segment_not_present_handler, KERNEL_LEVEL);
    setInterruptHandler(12, stack_exception_handler, KERNEL_LEVEL);
    setInterruptHandler(13, general_protection_handler, KERNEL_LEVEL);
    setInterruptHandler(14, page_fault_handler, KERNEL_LEVEL);
    setInterruptHandler(16, floating_point_error_handler, KERNEL_LEVEL);
    setInterruptHandler(17, alignment_check_handler, KERNEL_LEVEL);

    /* INTERRUPT INITIALIZATION */
    setInterruptHandler(32, clock_handler, KERNEL_LEVEL);
    setInterruptHandler(33, keyboard_handler, KERNEL_LEVEL);

    /* SYSCALL INITIALIZATION */
    setTrapHandler(0x80, syscall_handler, USER_LEVEL);

    set_idt_reg(&idtR);
}

void divide_error_routine() {
    printk("\nDivide error Exception\n");
    while(1);
}

void debug_routine() {
    printk("\nDebug Exception\n");
    while(1);
}

void nm1_routine() {
    printk("\nNM1 Exception\n");
    while(1);
}

void breakpoint_routine() {
    printk("\nBreakpoint Exception\n");
    while(1);
}

void overflow_routine() {
    printk("\nOverflow Exception\n");
    while(1);
}

void bounds_check_routine() {
    printk("\nBounds Check Exception\n");
    while(1);
}

void invalid_opcode_routine() {
    printk("\nInvalid Opcode Exception\n");
    while(1);
}

void device_not_available_routine() {
    printk("\nDevice not Available Exception\n");
    while(1);
}

void double_fault_routine() {
    printk("\nDouble Fault Exception\n");
    while(1);
}

void coprocessor_segment_overrun_routine() {
    printk("\nCoprocessor Segment Overrun Exception\n");
    while(1);
}

void invalid_tss_routine() {
    printk("\nInvalid TSS Exception\n");
    while(1);
}

void segment_not_present_routine() {
    printk("\nSegment not present Exception\n");
    while(1);
}

void stack_exception_routine() {
    printk("\nStack Exception\n");
    while(1);
}

void general_protection_routine() {
    printk("\nGeneral Protection Exception\n");
    while(1);
}

void page_fault_routine() {
    printk("\nPage Fault Exception\n");
    while(1);
}

void floating_point_error_routine() {
    printk("\nFloating Point Error Exception\n");
    while(1);
}

void alignment_check_routine() {
    printk("\nAlignment Check Exception\n");
    while(1);
}

void clock_routine()
{
    char min_buff[10];
    char sec_buff[3];
    int aux;

    /* Elapsed Time */
    ++ticks;
    if(ticks == 485)  // 485 =~ 1 sec
    {
        ticks = 0;
        ++secs;

        itoa(secs/60, min_buff);
        itoa(secs%60, sec_buff);
	
        if((aux = strlen(min_buff)) == 1) printc_xy(X_CLOCK, Y_CLOCK, '0');
        printk_xy(X_CLOCK - aux+2, Y_CLOCK, min_buff);
        printc_xy(X_CLOCK+2, Y_CLOCK, ':');
        if((aux = strlen(sec_buff)) == 1) printc_xy(X_CLOCK+3, Y_CLOCK, '0');
        printk_xy(X_CLOCK - aux+5, Y_CLOCK, sec_buff);
    }

    /* Scheduling */
    set_eoi=1;
    current()->nbtics_cpu++; // Update Stats
    scheduler();
    set_eoi=0;
}

void keyboard_routine() {
    char key=inb(0x60);
    int cr;
    char ch;
char buff[128];
itoa(current()->Pid, buff); //-------------------------->PARA PRUEBAS!!!!
printk(buff);
    if(!(key & 0x80)) {
        cr = (key & 0x7F);
        ch = char_map[cr];

        if(ch=='\0') printk_xy(X_KEY, Y_KEY, "Control\0");
        else {
            printk_xy(X_KEY, Y_KEY, "      \0");
            printc_xy(X_KEY + 6, Y_KEY, ch);
        }
    }
}
