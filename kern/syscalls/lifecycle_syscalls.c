/** @file lifecyle_syscalls.c
 *
 *  @brief implementation of functions for process and
 *  thread lifecycles
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <core/fork.h>

/** @brief Handler to call the fork function
 *
 *  @return int new PID on success, -ve integer on failure
 */
int fork_handler_c() {
	return do_fork();
}
