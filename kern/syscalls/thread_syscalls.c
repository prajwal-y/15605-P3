/** @file thread_syscalls.c
 *
 *  @brief implementation of functions for thread management
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscalls/thread_syscalls.h>
#include <core/thread.h>
#include <global_state.h>
#include <simics.h>

/** @brief implement the functionality to get the tid
 *         from the global curr_thread struct. This passes
 *         the value back to the assembly wrapper which performs
 *         an IRET to return the current thread id to the user
 *
 *  @return int the thread id of the current thread
 */
int gettid_handler_c() {
    return curr_thread->id;
}

