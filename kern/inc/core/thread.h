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
#include <syscall.h>

#define KERNEL_STACK_SIZE ((PAGE_SIZE) * 2)

enum thread_status {
	RUNNING,
	RUNNABLE,
	WAITING,
	EXITED
};

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
    void *k_stack;              /* The address of the kernel task base (low) */
	uint32_t k_stack_base;		/* Top of the kernel stack for the thread (top) */
	uint32_t cur_esp;		 	/* Current value of the kernel stack %esp */
	uint32_t cur_ebp;			/* Current value of the kernel stack %ebp */
	enum thread_status status;	/* Life state of the thread */
    list_head runq_link;        /* Link structure for the run queue */
    list_head sleepq_link;       /* Link structure for the sleep queue */
    list_head thread_map_link;  /* Link structure for the hash map */
	list_head cond_wait_link;	/* Link structure for cond_wait */
    list_head task_thread_link; /* Link structure for list of threads in parent */
    long wake_time;             /* Time when this thread is to be woken up */
} thread_struct_t;

void kernel_threads_init();

thread_struct_t *create_thread(task_struct_t *task);

thread_struct_t *get_thread_from_id(int thr_id);

void remove_thread_from_map(int thr_id);

#endif  /* __THREAD_H */
