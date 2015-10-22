/** @file global_state.c
 *
 *  @brief definition of critical state that must be global
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <global_state.h>
#include <list/list.h>

thread_struct_t *curr_thread; /* The thread currently being run */
list_head runnable_threads;   /* List of runnable threads */

void init_state() {
    init_head(&runnable_threads);
}

