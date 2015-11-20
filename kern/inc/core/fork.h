/** @file fork.h
 *
 *  Header file for fork.c
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#ifndef __FORK_H
#define __FORK_H

int do_fork();

int do_thread_fork();

void disable_forking();

void enable_forking();

#endif  /* __FORK_H */
