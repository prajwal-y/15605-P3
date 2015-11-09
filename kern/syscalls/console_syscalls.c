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
#include <core/scheduler.h>
#include <core/thread.h>
#include <drivers/keyboard/keyboard.h>
#include <common/errors.h>
#include <stdio.h>

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

/** @brief read a line from console
 *
 *  @param arg_packet pointer to memory location containing the arguments
 *  @return int number of bytes copied into the buffer
 */
int readline_handler_c(void *arg_packet) {
    //TODO: arg_packet SHOULD NOT be in kernel address space
    //TODO: other checks?
    int len = *(int *)arg_packet;
    char *buf = (char *)(*((int *)arg_packet + 1));
    thread_struct_t *curr_thread = get_curr_thread();

    mutex_lock(&readline_mutex);
    int retval = nextline(buf, len);
    if (retval == ERR_INVAL) {
        return ERR_INVAL;
    }
    while (retval == ERR_NOTAVAIL) {
        cond_wait(&readline_cond_var, &readline_mutex, 
                  &curr_thread->cond_wait_link, WAITING);
        retval = nextline(buf, len);
    }
    mutex_unlock(&readline_mutex);
    return retval;
}

