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
