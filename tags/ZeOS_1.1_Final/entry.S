/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>
#include <errno.h>

/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %edx; \
      pushl %ecx; \
      pushl %ebx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es


#define RESTORE_ALL \
	popl %ebx; \
	popl %ecx; \
	popl %edx; \
	popl %esi; \
	popl %edi; \
	popl %ebp; \
	popl %eax; \
	popl %ds; \
	popl %es; \
	popl %fs; \
	popl %gs


#define EOI \
	movb $0x20, %al; \
	outb %al, $0x20


ENTRY(divide_error_handler)
SAVE_ALL
call divide_error_routine
RESTORE_ALL
iret


ENTRY(debug_handler)
SAVE_ALL
call debug_routine
RESTORE_ALL
iret


ENTRY(nm1_handler)
SAVE_ALL
call nm1_routine
RESTORE_ALL
iret


ENTRY(breakpoint_handler)
SAVE_ALL
call breakpoint_routine
RESTORE_ALL
iret


ENTRY(overflow_handler)
SAVE_ALL
call overflow_routine
RESTORE_ALL
iret


ENTRY(bounds_check_handler)
SAVE_ALL
call bounds_check_routine
RESTORE_ALL
iret


ENTRY(invalid_opcode_handler)
SAVE_ALL
call invalid_opcode_routine
RESTORE_ALL
iret


ENTRY(device_not_available_handler)
SAVE_ALL
call device_not_available_routine
RESTORE_ALL
iret


ENTRY(double_fault_handler)
SAVE_ALL
call double_fault_routine
RESTORE_ALL
addl $4, %esp
iret


ENTRY(coprocessor_segment_overrun_handler)
SAVE_ALL
call coprocessor_segment_overrun_routine
RESTORE_ALL
iret


ENTRY(invalid_tss_handler)
SAVE_ALL
call invalid_tss_routine
RESTORE_ALL
addl $4, %esp
iret


ENTRY(segment_not_present_handler)
SAVE_ALL
call segment_not_present_routine
RESTORE_ALL
addl $4, %esp
iret


ENTRY(stack_exception_handler)
SAVE_ALL
call stack_exception_routine
RESTORE_ALL
addl $4, %esp
iret


ENTRY(general_protection_handler)
SAVE_ALL
call general_protection_routine
RESTORE_ALL
addl $4, %esp
iret


ENTRY(page_fault_handler)
SAVE_ALL
call page_fault_routine
RESTORE_ALL
addl $4, %esp
iret


ENTRY(floating_point_error_handler)
SAVE_ALL
call floating_point_error_routine
RESTORE_ALL
iret


ENTRY(alignment_check_handler)
SAVE_ALL
call alignment_check_routine
RESTORE_ALL
addl $4, %esp
iret


ENTRY(clock_handler)
SAVE_ALL
call clock_routine
EOI
RESTORE_ALL
iret


ENTRY(keyboard_handler)
SAVE_ALL
call keyboard_routine
EOI
RESTORE_ALL
iret


ENTRY(sys_call_table)
.long sys_ni_syscall
.long sys_ni_syscall
.long sys_ni_syscall
.long sys_ni_syscall
.long sys_write


ENTRY(syscall_handler)
SAVE_ALL
cmpl $4, %eax
jg syscall_wrong
cmpl $0, %eax
jge syscall_right

syscall_wrong:
movl $-(ENOSYS), %eax
jmp return

syscall_right:
call *sys_call_table(, %eax, 0x04)

return:
movl %eax, 24(%esp)
RESTORE_ALL
iret
