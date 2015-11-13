/** @file syscall_util.c
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <ureg.h>
#include <stdlib.h>
#include <core/scheduler.h>
#include <core/thread.h>
#include <seg.h>
#include <simics.h>
#include <asm/asm.h>
#include <x86/eflags.h>
#include <common/errors.h>
#include <syscalls/syscall_util.h>

static int validate_uregs(ureg_t *uregs);

/** @brief setup the kernel stack
 *
 *  @param pointer to a ureg area containing new values to be applied
 *  @param kernel_stack_base the base of the kernel stack
 *  @return 0 on success, -ve integer on failure
 */
int setup_kernel_stack(ureg_t *ureg, void *kernel_stack_base) {
    int retval = validate_uregs(ureg);
    if (retval < 0) {
        return retval;
    }
	//TODO: !!! REMOVE THE CONSTANTS !!!
    *((int *)(kernel_stack_base) - 1) = ureg->ds;   //Check for validity
    *((int *)(kernel_stack_base) - 2) = ureg->esp;
    *((int *)(kernel_stack_base) - 3) = ureg->eflags;
    *((int *)(kernel_stack_base) - 4) = ureg->cs;   //Check for validity
    *((int *)(kernel_stack_base) - 5) = ureg->eip;

    /* Simulate a pusha for the 8 registers that are pushed */
    //memset(((int *)(kernel_stack_base) - 13), 0, 32);
    *((int *)(kernel_stack_base) - 6) = ureg->eax;
    lprintf("eax value is %x", ureg->eax);
    *((int *)(kernel_stack_base) - 7) = ureg->ecx;
    *((int *)(kernel_stack_base) - 8) = ureg->edx;
    *((int *)(kernel_stack_base) - 9) = ureg->ebx;
    //*((int *)(kernel_stack_base) - 10) = ureg->esp;
    *((int *)(kernel_stack_base) - 11) = ureg->ebp;
    *((int *)(kernel_stack_base) - 12) = ureg->esi;
    *((int *)(kernel_stack_base) - 13) = ureg->edi;

	//*((int *)(kernel_stack_base) - 14) = (int)iret_fun;
    return 0;
}

int validate_uregs(ureg_t *uregs) {
    //TODO: WHAT EFLAGS CAN THE USER CHANGE?
    
	/* IOPL shouldn't be user privilege */
    if ((uregs->eflags & EFLAGS_IOPL) == EFLAGS_IOPL) {
        return ERR_INVAL;
    }
    return 0;
}

/** @brief populate an ureg struct using the current values on the stack
 *
 *  @param ureg a pointer to the ureg struct to be filled in
 *  @param err_code_avail whether an error code is available on the stack
 *
 *  @return void
 */
void populate_ureg(ureg_t *ureg, int err_code_avail, 
                   thread_struct_t *curr_thread) {
    void *kernel_stack_base = (void *)curr_thread->k_stack_base;

    ureg->ds = SEGSEL_USER_DS;
    ureg->es = SEGSEL_USER_DS;
    ureg->fs = SEGSEL_USER_DS;
    ureg->gs = SEGSEL_USER_DS;

    ureg->eax = *((int *)(kernel_stack_base) - 6 - err_code_avail);
    ureg->ecx = *((int *)(kernel_stack_base) - 7 - err_code_avail);
    ureg->edx = *((int *)(kernel_stack_base) - 8 - err_code_avail);
    ureg->ebx = *((int *)(kernel_stack_base) - 9 - err_code_avail);
    ureg->zero = 0;
    ureg->ebp = *((int *)(kernel_stack_base) - 11 - err_code_avail);
    ureg->esi = *((int *)(kernel_stack_base) - 12 - err_code_avail);
    ureg->edi = *((int *)(kernel_stack_base) - 13 - err_code_avail);

    if (err_code_avail == ERR_CODE_AVAIL) {
        ureg->error_code = *((int *)(kernel_stack_base) - 6);
    }
   else {
        ureg->error_code = 0;
   }

   ureg->eip = *((int *)(kernel_stack_base) - 5);
   ureg->cs = *((int *)(kernel_stack_base) - 4);
   ureg->eflags = *((int *)(kernel_stack_base) - 3);
   ureg->esp = *((int *)(kernel_stack_base) - 2);
   ureg->ss = *((int *)(kernel_stack_base) - 1);
}

/** @brief set the return value in the kernel stack
 *
 *  @param the eax vaue to be taken
 *  @return void
 */
void set_kernel_stack_eax(int eax) {
    thread_struct_t *curr_thread = get_curr_thread();
    *((int *)(curr_thread->k_stack_base) - 6) = eax;
}
