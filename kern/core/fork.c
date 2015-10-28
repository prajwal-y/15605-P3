/** @file fork.c
 *
 *  File which implements the functions required for fork
 *  system call.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <core/task.h>
#include <core/scheduler.h>
#include <vm/vm.h>
#include <asm/asm.h>
#include <common/errors.h>
#include <string.h>
#include <syscall.h>
#include <simics.h>

/** @brief The entry point for fork
 *
 *  @return int the ID of the new task. If fork
 *  fails, then a negative number is returned.
 */
int do_fork() {

	/* Create a child task */
	task_struct_t *child_task = create_task();
	if(child_task == NULL) {
		return ERR_NOMEM;
	}
	
	/* Clone the address space */
	task_struct_t *curr_task = get_curr_task();
	void *new_pd_addr = clone_paging_info(curr_task->pdbr);
	if(new_pd_addr == NULL) {
		return ERR_FAILURE;
	}
	child_task->pdbr = new_pd_addr;

	/* Clone the kernel stack */
	memcpy(child_task->thr->k_stack, curr_task->thr->k_stack, PAGE_SIZE);
	*((int *)(child_task->thr->k_stack_base) - 14) = (int)iret_fun; //TODO: REMOVE THAT CONSTANT
	child_task->thr->cur_esp = child_task->thr->k_stack_base 
									- DEFAULT_STACK_OFFSET;

	/* Add the first thread of the new task to runnable queue */
	runq_add_thread(child_task->thr);

	/* Dummy operation to invalidate TLB */
	set_cur_pd(curr_task->pdbr);

	return child_task->id;	
}
