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
#include <core/task.h>
#include <core/scheduler.h>

static int get_num_args(char **argvec);
static char **copy_args(int num_args,char **argvec);
static void free_argvec(int num_args, char **argvec);

/** @brief The entry point for exec
 *
 *  @return nothing if the call succeeds. If exec
 *  fails, then a negative number is returned.
 */
int do_exec(void *arg_packet) {

    //TODO: Sanitize every byte of input
    char *execname = (char *)(*((int *)arg_packet));
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
    free_paging_info(old_pd);
    sfree(execname_kern, execname_len + 1);
    free_argvec(num_args, argvec_kern);

    return 0;
}

char **copy_args(int num_args,char **argvec) {
    int i;
    char *arg;
    char **argvec_kern = (char **)smalloc((num_args + 1) * sizeof(char *));
    for (i = 0; i < num_args; i++) {
        int arg_len = strlen(argvec[i]);
        arg = (char *)smalloc(arg_len + 1);
        strncpy(arg, argvec[i], arg_len + 1);
        argvec_kern[i] = arg;
    }
    arg = (char *)smalloc(sizeof(char));
    *arg = '\0';
    argvec_kern[i] = arg;
    return argvec_kern;
}

int get_num_args(char **argvec) {
    int count = 0;
    while (argvec[count] != '\0') {
        count++;
    }
    return count;
}

void free_argvec(int num_args, char **argvec) {
    int i;
    for (i = 0; i < num_args; i++) {
        int arg_len = strlen(argvec[i]);
        sfree(argvec[i], arg_len + 1);
    }
    sfree(argvec[i], sizeof(char));
    sfree(argvec, (num_args + 1) * sizeof(char *));
}
