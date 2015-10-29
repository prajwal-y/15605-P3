/** @file thread.h
 *  @brief prototypes of thread manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __THREAD_H
#define __THREAD_H

#include <ureg.h>
#include <stdint.h>
#include <list/list.h>
#include <core/task.h>

/** @brief a schedulable "unit"
 *
 *  Tasks contain threads. A thread contains all the context (registers) 
 *  required to be scheduled by the processor. A task does not contain any
 *  scheduling information. A task must contain atleast one thread.
 */
typedef struct thread_struct {
    int id;                     /* A unique identifier for a thread */
    ureg_t *regs;               /* The set of registers for this thread */
    task_struct_t *parent_task; /* The parent task for this thread */
    void *k_stack;              /* The address of the kernel task base */
	uint32_t k_stack_base;		/* Top of the kernel stack for the thread */
	uint32_t cur_esp;		 	/* Current value of the kernel stack %esp */
	uint32_t cur_ebp;			/* Current value of the kernel stack %ebp */
    list_head runq_link;        /* Link structure for the run queue */
    list_head thread_map_link;  /* Link structure for the hash map */
	list_head cond_wait_link;	/* Link structure for cond_wait */
} thread_struct_t;

void kernel_threads_init();

thread_struct_t *create_thread(task_struct_t *task);

thread_struct_t *get_thread_from_id(int thr_id);

#endif  /* __THREAD_H */
