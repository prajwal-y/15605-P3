/** @file thread_syscalls.c
 *
 *  @brief implementation of functions for thread management
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <syscalls/thread_syscalls.h>
#include <core/thread.h>
#include <core/sleep.h>
#include <core/scheduler.h>
#include <core/context.h>
#include <common/errors.h>
#include <simics.h>

/** @brief implement the functionality to get the tid
 *         from the global curr_thread struct. This passes
 *         the value back to the assembly wrapper which performs
 *         an IRET to return the current thread id to the user
 *
 *  @return int the thread id of the current thread
 */
int gettid_handler_c() {
    thread_struct_t *curr_thread = get_curr_thread();
    return curr_thread->id;
}

/** @brief yield to a different thread
 *
 *  @return int 0 on success -ve integer if tid does not exist or
 *              thread is suspended
 */
int yield_handler_c(int tid) {
    if (tid != -1) {
        thread_struct_t *thr = get_thread_from_id(tid);
        if (thr == NULL) {
            return ERR_INVAL;
        }
        if (thr->status == WAITING) {
            return ERR_FAILURE;
        }
    }
    context_switch();
    return 0;
}

/** @brief sleep for a specified amount of time
 *
 *  @return int 0 on success -ve integer if ticks is negative
 */
int sleep_handler_c(int ticks) {
    return do_sleep(ticks);
}
