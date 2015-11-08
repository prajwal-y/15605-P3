/** @file thread_syscalls.h
 *
 *  @brief prototypes of functions for thread management
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __THREAD_SYSCALLS_H
#define __THREAD_SYSCALLS_H

int gettid_handler();

int gettid_handler_c();

int yield_handler();

int yield_handler_c(int tid);

int sleep_handler();

int sleep_handler_c(int ticks);

#endif  /* __THREAD_SYSCALLS_H */
