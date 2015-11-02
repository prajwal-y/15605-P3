/** @file exec.c
 *
 *  File which implements the functions required for the exec
 *  system call.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <simics.h>
#include <malloc.h>
#include <elf_410.h>
#include <common/assert.h>
#include <string/string.h>
#include <vm/vm.h>
#include <cr.h>
#include <common/errors.h>
#include <core/task.h>
#include <core/scheduler.h>
#include <loader/loader.h>

static int get_num_args(char **argvec);
static char **copy_args(int num_args,char **argvec);
static void free_argvec(int num_args, char **argvec);

/** @brief The entry point for exec
 *
 *  @param arg_packet The address of argument packet containing 
 *  the required arguments for exec.
 *
 *  @return nothing if the call succeeds. If exec
 *  fails, then a negative number is returned.
 */
int do_exec(void *arg_packet) {

    //TODO: Sanitize every byte of input. Possibly to move it to a common file
    //since it will be used by many system calls.
    char *execname = (char *)(*((int *)arg_packet));
    int retval = check_program(execname);
    if (retval == PROG_ABSENT_INVALID) {
        return ERR_FAILURE;
    }
    char **argvec = (char **)(*((int *)arg_packet + 1));
    int num_args = get_num_args(argvec);
    int execname_len;
    //int retval;
    void *old_pd = (void *)get_cr3();

    execname_len = strlen(execname);
    if (execname_len > 16) {
        //TODO: Set reasonable limit on execname len
    }

    /* Copy execname to kernel memory */
    char *execname_kern = (char *)smalloc((execname_len + 1) * sizeof(char));
    strncpy(execname_kern, execname, execname_len + 1);

    /* Copy the argument vector into kernel space as we will be freeing
     * the old process's address space soon */
    char **argvec_kern = copy_args(num_args, argvec);
    task_struct_t *t = get_curr_task();

    load_task(execname_kern, num_args, argvec_kern, t);

    /* Free kernel argvec and execname */
    decrement_ref_count_and_free_pages(old_pd);
    sfree(execname_kern, execname_len + 1);
    free_argvec(num_args, argvec_kern);

    return 0;
}

/** @brief Function to copy the arguments to kernel memory
 *
 *  @param num_args Number of arguments
 *  @param argvec Character array of arguments
 *
 *  @return char** copied character vector in kernel memory.
 *  NULL on failure.
 */
char **copy_args(int num_args,char **argvec) {
    int i;
    char *arg;
    char **argvec_kern = (char **)smalloc((num_args + 1) * sizeof(char *));
	if(argvec_kern == NULL) {
		return NULL;
	}
    for (i = 0; i < num_args; i++) {
        int arg_len = strlen(argvec[i]);
        arg = (char *)smalloc(arg_len + 1);
		if(arg == NULL) {
			return NULL;
		}
        strncpy(arg, argvec[i], arg_len + 1);
        argvec_kern[i] = arg;
    }
    arg = (char *)smalloc(sizeof(char));
	if(arg == NULL) {
		return NULL; //TODO: Free allocated memory so far?
	}
    *arg = '\0';
    argvec_kern[i] = arg;
    return argvec_kern;
}

/** @brief Function to get the number of arguments from
 * the argument vector
 *
 *  @param argvec The argument vector
 *  @return int The number of arguments
 */
int get_num_args(char **argvec) {
    int count = 0;
    while (argvec[count] != '\0') {
        count++;
    }
    return count;
}

/** @brief Function to free the kernel memory allocated for
 * storing the arguments from exec
 *
 *  @param num_args Number of arguments
 *  @param argvec The character array of arguments
 *
 *  @return void
 */
void free_argvec(int num_args, char **argvec) {
    int i;
    for (i = 0; i < num_args; i++) {
        int arg_len = strlen(argvec[i]);
        sfree(argvec[i], arg_len + 1);
    }
    sfree(argvec[i], sizeof(char));
    sfree(argvec, (num_args + 1) * sizeof(char *));
}
