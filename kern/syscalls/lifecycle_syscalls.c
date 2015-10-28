/** @file lifecyle_syscalls.c
 *
 *  @brief implementation of functions for process and
 *  thread lifecycles
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <core/fork.h>
#include <core/exec.h>

/** @brief Handler to call the fork function
 *
 *  @return int new PID on success, -ve integer on failure
 */
int fork_handler_c() {
	return do_fork();
}

/** @brief Handler to call the exec function
 *
 *  @return does not return on success, -ve integer on failur
 */
int exec_handler_c(void *arg_packet) {
	return do_exec(arg_packet);
}
