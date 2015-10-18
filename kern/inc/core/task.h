/** @file task.h
 *  @brief task manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __TASK_H
#define __TASK_H

#include <list/list.h>

/** @brief the protection domain comprising a task */
typedef struct task_struct {
    void *pdbr;         /* the address of the page directory */
    list_head thread_head;    
} task_struct_t;

void create_task();

void load_bootstrap_task(const char *prog_name);

#endif  /* __TASK_H */

