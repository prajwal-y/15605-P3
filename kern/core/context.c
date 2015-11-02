/** @file context.c
 * 
 *  File for implementing context switching
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <cr.h>
#include <asm.h>
#include <asm/asm.h>
#include <vm/vm.h>
#include <syscall.h>
#include <core/scheduler.h>
#include <core/thread.h>
#include <simics.h>
#include <common/malloc_wrappers.h>

static void switch_to_thread(thread_struct_t *curr_thread, 
								thread_struct_t *new_thread);

/** @brief Function to context switch to a different thread
 *
 *  This function calls the scheduler to get the next schedulable
 *  thread. It also sends the current thread to the scheduler to
 *  be added back to the runnable queue.
 *
 *  @return Void
 */
void context_switch() {

	disable_interrupts();	/* Context switching is a critical section */

	/* Get the next thread to be run from scheduler */
    thread_struct_t *thr = next_thread();
	
	if(thr == NULL) { /* There are no other threads to schedule */
		return;
	}
    
	thread_struct_t *curr_thread = get_curr_thread();
	if(curr_thread->status == RUNNING) {
		curr_thread->status = RUNNABLE;
		runq_add_thread(curr_thread);
	}
    if (curr_thread->status == EXITED) { //TODO: where exactly to free the stack?
		sfree(curr_thread->k_stack, KERNEL_STACK_SIZE);
        sfree(curr_thread, sizeof(thread_struct_t));
        curr_thread = NULL;
    }

	//lprintf("Going to switch to thread id %d with stack_base %p", thr->id, (void *)thr->k_stack_base);
	
	/* Call switch_to_thread with the new thread */
    switch_to_thread(curr_thread, thr);

	//lprintf("Switched to thread id: %d from thread %d", thr->id, curr_thread->id);

	enable_interrupts();
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
 *  TODO: Need to fix a possible race condition in this function 
 *  because context switch can be invoked from two places now.
 */
void switch_to_thread(thread_struct_t *curr_thread, 
						thread_struct_t *next_thread) {
	/* This should never happpen as this check exists in the calling function */
	if(next_thread == NULL) {
		return;
	}
	
	/* Set ds for the new thread */
	set_ds((next_thread->regs)->ds);
	set_es((next_thread->regs)->es);
	set_fs((next_thread->regs)->fs);
	set_gs((next_thread->regs)->gs);

    /* Set page directory for the new thread */
    task_struct_t *parent_task = next_thread->parent_task;
    set_cur_pd(parent_task->pdbr);

	/* Set the esp for the new thread */	
	set_esp0(next_thread->k_stack_base);

	/* Set the new thread as the currently running thread */
	set_running_thread(next_thread);
	next_thread->status = RUNNING;

    if (curr_thread != NULL) {
        update_stack(next_thread->cur_esp, next_thread->cur_ebp, 
                    (uint32_t)&curr_thread->cur_esp, 
                    (uint32_t)&curr_thread->cur_ebp);
    }
    else {
        update_stack_single(next_thread->cur_esp, next_thread->cur_ebp); 
    }

}
