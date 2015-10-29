/** @file lifecycle_syscalls.h
 *
 *  @brief prototypes of functions for task lifecycle functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __LIFECYCLE_SYSCALLS_H
#define __LIFECYCLE_SYSCALLS_H

void fork_handler();

int fork_handler_c();

void exec_handler();

int exec_handler_c();

#endif  /* __LIFECYCLE_SYSCALLS_H */
