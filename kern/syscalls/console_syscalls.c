/** @file console_syscalls.c
 *
 *  @brief implementation of functions for console I/O
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscalls/console_syscalls.h>
#include <console.h>
#include <simics.h>

/** @brief print to screen 
 *
 *  @param arg_packet pointer to a memory location containing the arguments
 *  @return int 0 on success, -ve integer on failure
 */
int print_handler_c(void *arg_packet) {
    //TODO: arg_packet SHOULD NOT be in kernel address space
    //TODO: other checks?
    int len = *(int *)arg_packet;
    char *buf = (char *)(*((int *)arg_packet + 1));
    putbytes(buf, len);
    return 0;
}

