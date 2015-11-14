/** @file misc_syscalls_asm.S
 *  
 *  handlers for miscellaneous system calls
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include<simics.h>

.globl halt_handler
halt_handler:
    call sim_halt
    cli             /* In case sim_halt is a no-op */
    hlt             /* No more interrupts, no more instructions */

.globl readfile_handler
readfile_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    push %esi
    call readfile_handler_c
    pop %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
    iret
