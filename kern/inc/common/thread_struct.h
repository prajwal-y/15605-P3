/** @file thread_struct.h
 *  @brief the thread struct
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __THREAD_STRUCT_H
#define __THREAD_STRUCT_H

#include <ureg.h>
#include <common/task_struct.h>

/** @brief a schedulable "unit"
 *
 *  Tasks contain threads. A thread contains all the context (registers) 
 *  required to be scheduled by the processor. A task does not contain any
 *  scheduling information. A task must contain atleast one thread.
 */
typedef struct thread_struct {
    int tid;                    /* A unique identifier for a thread */
    ureg_t *regs;                /* The set of registers for this thread */
    task_struct_t *parent_task;  /* The parent task for this thread */
} thread_struct_t;

#endif  /* __THREAD_STRUCT_H */
