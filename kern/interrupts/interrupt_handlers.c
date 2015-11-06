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
#include <idt.h>
#include <cr.h>

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
	
	int tid = get_curr_thread()->id;

    //lprintf("PD is %p", (void *)get_cr3());
	lprintf("Address that caused page fault: %p Cause of error= %d. Thread that is failed %d", page_fault_addr, error_code, tid);

	if(is_addr_cow(page_fault_addr)) {
		handle_cow(page_fault_addr);
	} else {
		MAGIC_BREAK;
	}

	//TODO: HANDLE OTHER CASES OF PAGE FAULT. AND MOVE THIS TO OTHER FILE
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
