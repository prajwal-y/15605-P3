/** @file thread.h
 *  @brief prototypes of thread manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __THREAD_H
#define __THREAD_H
#include <common/thread_struct.h>

void thread_init();

thread_struct_t *create_thread(task_struct_t *task, ureg_t *regs);

#endif  /* __THREAD_H */
