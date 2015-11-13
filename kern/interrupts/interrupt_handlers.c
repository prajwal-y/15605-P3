/** @file interrupt_handlers.c
 *  @brief This file contains interrupt initialization
 *         and activation code and other interrupt helper
 *         routines
 *
 *  This file currenty initializes the timer and keyboard
 *  drivers. It initializes the PIT in the square wave mode
 *  and sets the frequency of interrupts.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <asm.h>
#include <asm/asm.h>
#include <core/scheduler.h>
#include <interrupts/interrupt_handlers.h>
#include <interrupts/interrupt_handlers_asm.h>
#include <interrupt_defines.h>
#include <drivers/timer/timer.h>
#include <vm/vm.h>
#include <drivers/keyboard/keyboard.h>
#include <simics.h>
#include <interrupts/idt_entry.h>
#include <core/context.h>
#include <core/wait_vanish.h>
#include <stdio.h>
#include <idt.h>
#include <cr.h>
#include <syscall.h>
#include <syscalls/syscall_util.h>
#include <common/malloc_wrappers.h>
#include <string.h>
#include <common/assert.h>

#define NUM_INTERRUPTS 10 /* The number of interrupts we have defined handlers for */
#define IDT_ENTRY_SIZE 8  /* Size of each IDT */

void tickback(unsigned int ticks) {
	//lprintf("Tick: %d", ticks);
	context_switch();
    // Add some logic to be run on each timer tick
    //return;
}

//static void install_divide_error_handler();
static void install_page_fault_handler();
static void *setup_swexn_stack(void *esp3, ureg_t *ureg, void *arg);
static void update_fault_stack(void *esp3, swexn_handler_t eip, 
                               thread_struct_t *curr_thread);

void install_handler_4();
void install_handler_5();
void install_handler_6();
void install_handler_10();
void install_handler_11();
void install_handler_12();
void install_handler_13();
void install_handler_17();
void install_handler_18();

/** @brief The driver-library initialization function
 *
 *   Installs the timer and keyboard interrupt handler.
 *   NOTE: handler_install should ONLY install and activate the
 *   handlers; any application-specific initialization should
 *   take place elsewhere.
 *
 *   @param tickback Pointer to clock-tick callback function
 *   
 *   @return A negative error code on error, or 0 on success
 **/
int install_handlers() {
    initialize_timer(tickback);
    install_keyboard_handler();
    //install_divide_error_handler();
	install_page_fault_handler();

	install_handler_4();
	install_handler_5();
	install_handler_6();
	install_handler_10();
	install_handler_11();
	install_handler_12();
	install_handler_13();
	install_handler_17();
	install_handler_18();

    // Add all the handlers we require here
    return 0;
}

/** @brief this function handles a divide by zero error condition.
 *
 *  //TODO: add logic to pass a software exception to user task
 *  If the user process has not defined a software exception handler for 
 *  this cause the kernel will kill the process. Else the process has a chance
 *  to handle the faulty divide.
 *
 *  @return void
 */
void divide_error_handler() {
    lprintf("Oh noes! y u divid bye zaero?");
    return;
}

/** @brief This function handles the page fault
 *
 * TODO: FIX THIS
 *
 * @return Void
 */
void page_fault_handler_c() {

	int error_code = get_err_code();

	void *page_fault_addr = (void *)get_cr2();
	
    thread_struct_t *curr_thread = get_curr_thread();
	int tid = curr_thread->id;
    task_struct_t *curr_task = get_curr_task();

	if(is_addr_cow(page_fault_addr)) {
		handle_cow(page_fault_addr);
	} 
    else if (curr_task->eip != NULL){
        ureg_t *ureg = (ureg_t *)smalloc(sizeof(ureg_t));
        thread_assert(ureg != NULL);
        populate_ureg(ureg, ERR_CODE_AVAIL, curr_thread);
        void *stack_bottom = setup_swexn_stack(curr_task->swexn_esp, ureg, curr_task->swexn_args);
        update_fault_stack(stack_bottom, curr_task->eip, curr_thread);
        curr_task->eip = NULL;
        return;
    }
    else {
		lprintf("Address that caused page fault: %p Cause of error= %d. Thread that is failed %d", page_fault_addr, error_code, tid);
		printf("Segmentation fault");
		get_curr_task()->exit_status = -2;
		do_vanish();
		//MAGIC_BREAK;
	}

	//TODO: HANDLE OTHER CASES OF PAGE FAULT. AND MOVE THIS TO OTHER FILE
}

/** @brief setup the stack for the swexn handler
 *
 *  @param esp3 the stack where the swexn handler will run
 *  @param ureg the registers for the swexn handler
 *  @param arg the arguments to the swexn handler
 *  @return void* the bottom of the stack 
 */
void *setup_swexn_stack(void *esp3, ureg_t *ureg, void *arg) {
    lprintf("In swexn stack ureg is %p, stack_bottom is %p", ureg, esp3);
    void *stack_bottom = (char *)esp3 - sizeof(ureg_t);
    memcpy(stack_bottom, ureg, sizeof(ureg_t));

    *((int *)(stack_bottom) - 1) = (int)stack_bottom;   //Check for validity
    *((int *)(stack_bottom) - 2) = (int)arg;
    *((int *)(stack_bottom) - 3) = (int)ureg->eip;
    return (int *)stack_bottom - 3;
}

/** @brief update the kernel stack to change return esp and eip
 *
 *  @param esp the new esp valur to be set in the kernel stack
 *  @param eip the first instruction of the fault handler
 *  @return void
 */
void update_fault_stack(void *esp, swexn_handler_t eip, 
                        thread_struct_t *curr_thread) {
    *((int *)(curr_thread->k_stack_base) - 2) = (int)esp;
    *((int *)(curr_thread->k_stack_base) - 5) = (int)eip;
}

    

/** @brief this function installs a handler for divide by zero fault conditions
 *
 *  //TODO: Move the handler installer to appropriate place if required
 *
 *  @return void
 */
void install_divide_error_handler() {
    add_idt_entry(divide_error_handler, IDT_DE, TRAP_GATE, KERNEL_DPL);
}

/** @brief This function installs a handler for page fault
 *
 * @return void
 *
 */
void install_page_fault_handler() {
	add_idt_entry(page_fault_handler, IDT_PF, TRAP_GATE, KERNEL_DPL);
}

void four_fault_handler() {
	lprintf("4 fult");
	MAGIC_BREAK;
}
void install_handler_4() {
	add_idt_entry(four_fault_handler, 4, INTERRUPT_GATE, KERNEL_DPL);
}
void five_fault_handler() {
	lprintf("5 fult");
	MAGIC_BREAK;
}
void install_handler_5() {
	add_idt_entry(five_fault_handler, 5, INTERRUPT_GATE, KERNEL_DPL);
}
void six_fault_handler() {
	void *instr = (void *)get_err_instr();
	lprintf("Address that caused 6 fult: %p", instr);
	lprintf("6 fult");
	MAGIC_BREAK;
}
void install_handler_6() {
	add_idt_entry(six_fault_handler, 6, INTERRUPT_GATE, KERNEL_DPL);
}
void ten_fault_handler() {
	lprintf("10 fult");
	MAGIC_BREAK;
}
void install_handler_10() {
	add_idt_entry(ten_fault_handler, 10, INTERRUPT_GATE, KERNEL_DPL);
}
void leven_fault_handler() {
	lprintf("11 fult");
	MAGIC_BREAK;
}
void install_handler_11() {
	add_idt_entry(leven_fault_handler, IDT_NP, INTERRUPT_GATE, KERNEL_DPL);
}

void twel_fault_handler() {
	lprintf("12 fult");
	MAGIC_BREAK;
}
void install_handler_12() {
	add_idt_entry(twel_fault_handler, IDT_SS, INTERRUPT_GATE, KERNEL_DPL);
}

void thirrt_fault_handler() {
	MAGIC_BREAK;
	lprintf("13 fult");
}
void install_handler_13() {
	add_idt_entry(thirrt_fault_handler, IDT_GP, INTERRUPT_GATE, KERNEL_DPL);
}

void sevent_fault_handler() {
	lprintf("17 fult");
	MAGIC_BREAK;
}
void install_handler_17() {
	add_idt_entry(sevent_fault_handler, IDT_AC, INTERRUPT_GATE, KERNEL_DPL);
}
void eighte_fault_handler() {
	lprintf("18 fult");
	MAGIC_BREAK;
}
void install_handler_18() {
	add_idt_entry(eighte_fault_handler, 18, INTERRUPT_GATE, KERNEL_DPL);
}

/** @brief this function acknowledges the interrupt
 *
 *  We send an ACK to the PIC control port to indicate
 *  that we have finished processing the interrupt
 *
 *  @return void
 */
void acknowledge_interrupt() {
    outb(INT_CTL_PORT, INT_ACK_CURRENT);
}
