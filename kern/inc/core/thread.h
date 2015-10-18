/** @file thread.h
 *  @brief prototypes of thread manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __THREAD_H
#define __THREAD_H

#include <ureg.h>
#include <core/task.h>

/** @brief a schedulable "unit"
 *
 *  Tasks contain threads. A thread contains all the context (registers) 
 *  required to be scheduled by the processor. A task does not contain any
 *  scheduling information. A task must contain atleast one thread.
 */
typedef struct thread_struct {
    int id;                    /* A unique identifier for a thread */
    ureg_t *regs;                /* The set of registers for this thread */
    task_struct_t *parent_task;  /* The parent task for this thread */
} thread_struct_t;

void thread_init();

thread_struct_t *create_thread(task_struct_t *task, ureg_t *regs);

#endif  /* __THREAD_H */
