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
#include <sys.h>
#include <keyboard.h>
#include <mm.h>
#include <list.h>

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

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL) {
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

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL) {
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
void setIdt() {
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
    /* Exception info */
    char buffer[6];
    printk("\nProcess:");
    itoa(current()->Pid, buffer);
    printk(buffer);
    printk("\nPage Fault Exception\n");

    /* Process Exit */
    sys_exit();
    
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

void clock_routine() {
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
    scheduler();
    set_eoi=0;
}

void keyboard_routine() {
    char key=inb(0x60);
    int cr;
    char ch;
    unsigned long chars_to_copy;
    unsigned long log_pag_init;
    unsigned long log_pag_end;
    unsigned long num_pag_log;
    struct task_struct *t;
    char *buff_dest;
    int i;

    if(!(key & 0x80)) {
        cr = (key & 0x7F);
        ch = char_map[cr];

        if(ch=='\0') printk_xy(X_KEY, Y_KEY, "Control\0");
        else {
            printk_xy(X_KEY, Y_KEY, "      \0");
            printc_xy(X_KEY + 6, Y_KEY, ch);
        }

        /* Keyboard device */
        if(buff_size < KEYBUFF_SIZE) buff_keyboard_insert(ch);
        if(!list_empty(&keyboardqueue)) {
            t = list_head_to_task_struct(keyboardqueue.next);
            if(buff_size >= t->request_chars_to_keyboard || buff_size == KEYBUFF_SIZE) {
                chars_to_copy = t->request_chars_to_keyboard;
                if(chars_to_copy > buff_size) chars_to_copy = buff_size;

                /* Reserve AUX Logic Pages to Request Process */
                log_pag_init = NUM_PAGE_ADDR((unsigned long)(t->buff_location));
                log_pag_end = NUM_PAGE_ADDR(((unsigned long)(t->buff_location) + chars_to_copy));
                num_pag_log = log_pag_end - log_pag_init + 1;
                for(i=0; i<num_pag_log; i++) {
                    set_ss_pag(PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA+i,
                        t->phys_frames[log_pag_init - PAG_LOG_INIT_DATA_P0 + i]);
                }

                /* Copy Data from keyboard_buffer to User Memory of Request Process */
                buff_dest = (char *)(((PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA)<<12) |
                    ((unsigned long)(t->buff_location) & 0x0FFF));
                for(i=0; i<chars_to_copy; i++) buff_dest[i] = buff_keyboard_get_next();

                /* Delete AUX Logic Pages */
                for(i=0; i<num_pag_log; i++) del_ss_pag(PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA+i);
                set_cr3(); //Flush TLB

                /* Update task_struct of Request Process and Unblock it if necessary */
                t->request_chars_to_keyboard -= chars_to_copy;
                if(t->request_chars_to_keyboard == 0) {
                    list_del(&(t->rq_list));
                    list_add_tail(&(t->rq_list), &runqueue);
                }
                else t->buff_location += chars_to_copy;

                /* If keyboard_buffer was full then insert the new char */
                if(chars_to_copy == KEYBUFF_SIZE) buff_keyboard_insert(ch);
            }
        }
    }
}
