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
    pushl %esi
    call print_handler_c
    popl %esi
	iret
