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
#include <interrupts/interrupt_handlers.h>
#include <interrupt_defines.h>
#include <drivers/timer/timer.h>
#include <drivers/keyboard/keyboard.h>
#include <simics.h>
#include <interrupts/idt_entry.h>
#include <idt.h>

#define NUM_INTERRUPTS 10 /* The number of interrupts we have defined handlers for */
#define IDT_ENTRY_SIZE 8  /* Size of each IDT */

void tickback(unsigned int ticks) {
    // Add some logic to be run on each timer tick
    return;
}

static void install_divide_error_handler();

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
    install_divide_error_handler();
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

/** @brief this function installs a handler for divide by zero fault conditions
 *
 *  //TODO: Move the handler installer to appropriate place if required
 *
 *  @return void
 */
void install_divide_error_handler() {
    add_idt_entry(divide_error_handler, IDT_DE);
    return;
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
    return;
}
