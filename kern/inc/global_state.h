/** @file global_state.h
 *
 *  @brief critical state that must be global
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __GLOBAL_STATE_H
#define __GLOBAL_STATE_H
#include <core/thread.h>
#include <list/list.h>

extern thread_struct_t *curr_thread; /* The thread currently being run */

extern list_head runnable_threads;    /* List of runnable threads */

void init_state();

#endif  /* __GLOBAL_STATE_H */
