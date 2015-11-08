/** @file thread_syscalls.S
 *
 *  Implementations of thread interrupt handlers
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include<simics.h>

.globl gettid_handler
gettid_handler:
	pushl %eax
    pushl %ecx
    pushl %edx
    pushl %ebx
    pushl %esp
    pushl %ebp
    pushl %esi
    pushl %edi
    call gettid_handler_c
	popl %edi
    popl %esi
    popl %ebp
    popl %esp
    popl %ebx
    popl %edx
    popl %ecx
    popl %ecx
	iret

.globl yield_handler
yield_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    push %esi
    call yield_handler_c
    pop %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
    iret

.globl sleep_handler
sleep_handler:
	pushl %eax
    pushl %ecx
    pushl %edx
    pushl %ebx
    pushl %esp
    pushl %ebp
    pushl %edi
    pushl %esi
    call sleep_handler_c
    popl %esi
	popl %edi
    popl %ebp
    popl %esp
    popl %ebx
    popl %edx
    popl %ecx
    popl %ecx
	iret
