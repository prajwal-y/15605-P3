/** @file scheduler.h
 *  @brief prototypes for scheduler functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __SCHEDULER_H
#define __SCHEDULER_H
#include <core/thread.h>

thread_struct_t *next_thread();

void init_scheduler();

thread_struct_t *get_curr_thread();

task_struct_t *get_curr_task();

void runq_add_thread(thread_struct_t *thr);

void set_running_thread(thread_struct_t *thr);

#endif  /* __SCHEDULER_H */

