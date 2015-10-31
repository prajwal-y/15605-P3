/** @file task.h
 *  @brief task manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __TASK_H
#define __TASK_H

#include <list/list.h>
#include <core/task.h>

#define DEFAULT_STACK_OFFSET 56 

struct thread_struct;

/** @brief the protection domain comprising a task */
typedef struct task_struct {
	int id;
    void *pdbr;         	    /* the address of the page directory */
	struct thread_struct *thr;	/* the reference to the first thread */
    list_head thread_head;      /* List of threads in the task */
    int exit_status;            /* Exit status of task */
} task_struct_t;

task_struct_t *create_task();

void load_bootstrap_task(const char *prog_name);

void load_kernel_task(const char *prog_name);

void load_task(const char *prog_name, int num_arg, char **argvec, 
               task_struct_t *t);

#endif  /* __TASK_H */

