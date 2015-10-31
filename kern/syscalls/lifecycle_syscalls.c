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
#include <core/task.h>
#include <core/scheduler.h>

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

/** @brief Handler to call the set_status function
 *
 *  @return void
 */
void set_status_handler_c(int status) {
    task_struct_t *curr_task = get_curr_task();
    curr_task->exit_status = status;
}
