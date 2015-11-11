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
#include <sync/cond_var.h>
#include <sync/mutex.h>
#include <syscall.h>

#define DEFAULT_STACK_OFFSET 56 

struct thread_struct;

/** @brief the protection domain comprising a task */
typedef struct task_struct {
	int id;
    void *pdbr;         	        /* the address of the page directory */
	struct thread_struct *thr;	    /* the reference to the first thread */
    list_head thread_head;          /* List of threads in the task */
    int exit_status;                /* Exit status of task */
    struct task_struct *parent;     /* Link to the parent task */

    list_head child_task_head;      /* Linked list head for alive child tasks of THIS task */
    list_head child_task_link;      /* Link for list of alive child tasks in parent */ 
    
    list_head dead_child_head;      /* Linked list head for dead children of THIS task */
    list_head dead_child_link;      /* Link for list of dead children in parent */
    swexn_handler_t eip;            /* The swexn handler function */
    void *swexn_args;               /* Arguments to the swexn function */
    void *swexn_esp;                /* ESP to run the swexn handler on */

    /* Cond var for threads of THIS task to wait on child vanish()es */
    cond_t exit_cond_var;           
    /* Mutex to synchronize access to dead and alive child task lists */    
    mutex_t child_list_mutex;             
    /* Mutex to synchronize access to the current task's thread list */    
    mutex_t thread_list_mutex;
    /* Mutex to synchronize access to the vanish() system call */    
    mutex_t vanish_mutex;

     
} task_struct_t;

task_struct_t *create_task(task_struct_t *parent);

void load_bootstrap_task(const char *prog_name);

void load_kernel_task(const char *prog_name);

int load_task(const char *prog_name, int num_arg, char **argvec, 
               task_struct_t *t);

task_struct_t *get_init_task();

#endif  /* __TASK_H */

