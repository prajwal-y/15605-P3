/** @file task.h
 *  @brief task manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __TASK_H
#define __TASK_H

#include <list/list.h>

#define DEFAULT_STACK_OFFSET 56 

struct thread_struct;

/** @brief the protection domain comprising a task */
typedef struct task_struct {
	int id;
    void *pdbr;         	/* the address of the page directory */
	struct thread_struct *thr;	/* the reference to the first thread */
    list_head thread_head;    
} task_struct_t;

task_struct_t *create_task();

void load_bootstrap_task(const char *prog_name);

void load_task(const char *prog_name);

#endif  /* __TASK_H */

