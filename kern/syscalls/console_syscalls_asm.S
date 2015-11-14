/** @file console_syscalls.S
 *
 *  Implementations of console I/O
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include<simics.h>

.globl print_handler
print_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    pushl %esi
    call print_handler_c
    popl %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
	iret

.globl readline_handler
readline_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    pushl %esi
    call readline_handler_c
    popl %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
	iret

.globl set_term_color_handler
set_term_color_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    pushl %esi
    call set_term_color_handler_c
    popl %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
	iret

.globl set_cursor_pos_handler
set_cursor_pos_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    pushl %esi
    call set_cursor_pos_handler_c
    popl %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
	iret

.globl get_cursor_pos_handler
get_cursor_pos_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    pushl %esi
    call get_cursor_pos_handler_c
    popl %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
	iret

.globl getchar_handler
getchar_handler:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esp
	pushl %ebp
	pushl %edi
    pushl %esi
    call getchar_handler_c
    popl %esi
    popl %edi
	popl %ebp
	popl %esp
	popl %ebx
	popl %edx
	popl %ecx
	popl %ecx
	iret

