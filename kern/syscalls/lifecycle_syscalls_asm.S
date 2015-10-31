/** @file lifecycle_syscalls_asm.S
 *  
 *  handlers for lifecycle management system calls
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include<simics.h>

.globl fork_handler
fork_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
    call fork_handler_c
    popl %edi
	popl %esi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
	iret

.globl exec_handler
exec_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    push %esi
    call exec_handler_c
    pop %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
    iret

.globl set_status_handler
set_status_handler:
    pusha
    pushl %esi
    call set_status_handler_c
    popl %esi
    popa
    iret
