/** @file context.c
 * 
 *  File for implementing context switching
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <cr.h>
#include <asm/asm.h>
#include <global_state.h>

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
	/* Get the head of the list from the runnable queue */

	/* Add the current thread to the end of the runnable queue */

	/* Call switch_to_thread with the new thread */
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

	/*Set the current value of the stack pointer*/
	curr_thread->cur_k_stack = get_esp0();

	/* Set ds for the new thread*/
	set_ds((thread->regs)->ds);

	/*Set the esp for the new thread*/	
	set_esp0(thread->cur_k_stack);
}
