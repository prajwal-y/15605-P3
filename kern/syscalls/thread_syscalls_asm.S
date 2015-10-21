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
    call gettid_handler_c
	iret
