/** @file context.c
 * 
 *  File for implementing context switching
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <cr.h>
#include <asm/asm.h>
#include <vm/vm.h>
#include <core/scheduler.h>

static void switch_to_thread(thread_struct_t *thread);

/** @brief Function to context switch to a different thread
 *
 *  This function calls the scheduler to get the next schedulable
 *  thread. It also sends the current thread to the scheduler to
 *  be added back to the runnable queue.
 *
 *  @return Void
 */
void context_switch() {
	/* Get the next thread to be run from scheduler */
    thread_struct_t *thr = next_thread();

	/* Call switch_to_thread with the new thread */
    switch_to_thread(thr);
}

/** @brief Function to switch to a new thread.
 *
 *  This function replaces the general purpose registers and the
 *  stack pointer to the kernel stack of the new thread. The value
 *  of %cr3 is set to the page directory of the new thread. Since
 *  all the threads are suspended at the same point in execution,
 *  the value of %eip need not be explicitly changed.
 *
 *  @param thread The thread to which we need to switch to
 *
 *  @return Void
 */
void switch_to_thread(thread_struct_t *thread) {
	if(thread == NULL) {
		return;
	}
    thread_struct_t *curr_thread = get_curr_thread();

	/* Set the current value of the stack pointer */
	curr_thread->cur_k_stack = get_esp0();

	/* Set ds for the new thread */
	set_ds((thread->regs)->ds);

    /* Set page directory for the new thread */
    task_struct_t *parent_task = thread->parent_task;
    set_cur_pd(parent_task->pdbr);

	/* Set the esp for the new thread */	
	set_esp0(thread->cur_k_stack);
}
