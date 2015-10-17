/** @file task_struct.h
 *  @brief the task struct
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __TASK_STRUCT_H
#define __TASK_STRUCT_H

#include <list/list.h>

/** @brief the protection domain comprising a task */
typedef struct task_struct {
    void *pdbr;         /* the address of the page directory */
    list_head thread_head;    
} task_struct_t;

#endif  /* __TASK_STRUCT_H */
