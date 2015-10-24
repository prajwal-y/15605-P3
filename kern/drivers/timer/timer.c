/** @file timer.c
 *  @brief this file contains implementation of the timer
 *         driver functionality
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 */

#include <timer_defines.h>
#include <asm.h>
#include <seg.h>
#include <common/malloc_wrappers.h>
#include <string/string.h>
#include <interrupts/idt_entry.h>
#include <interrupts/interrupt_handlers.h>
#include <drivers/timer/timer_handler.h>

#define INT_FREQ 10
#define MILLISECONDS 1000

static void set_mode_freq();
static void install_timer_handler();
static void (*callback)(unsigned int);
static unsigned int tick_counter = 0;

/** @brief initialize the timer and install handler for it
 *
 *  @param tickback the callback function for the interrupt handler
 *  @return void
 */
void initialize_timer(void (*tickback)(unsigned int)) {
    callback = tickback;
    set_mode_freq();
    install_timer_handler();
}

/** @brief function to initialize the tier
 *
 *  This function initializes the timer to square wave mode
 *  and sets the number of interrupts we get per second by
 *  dividing the clock frequency by the required frequency.
 *
 *  @return void
 */
void set_mode_freq() {
    int output_freq = MILLISECONDS / INT_FREQ;
    int freq_divider = TIMER_RATE / output_freq;
    outb(TIMER_MODE_IO_PORT, TIMER_SQUARE_WAVE);
    outb(TIMER_PERIOD_IO_PORT, (unsigned char)(freq_divider & 0xff));
    outb(TIMER_PERIOD_IO_PORT, (unsigned char)((freq_divider >> 8) & 0xff));
}

/** @brief function to install timer handler in the IDT
 *
 *  @return void
 */
void install_timer_handler() {
    add_idt_entry(timer_handler, TIMER_IDT_ENTRY, TRAP_GATE);
}

/** @brief called by interrupt to do any processing needed
 *
 *  This function increments a variable keeping count of the number of
 *  ticks so far and calls the callback function with this variable
 *
 *  @return void
 */
void callback_handler() {
    tick_counter++;
    callback(tick_counter);
    acknowledge_interrupt();
    return;
}

/** @brief return the number of ticks since startup
 *
 *  This function returns the number of ticks since system
 *  startup. This can be used as a seed for a random number 
 *  generator and also to keep track of time.
 *
 *  @return unsigned int total_ticks
 */
unsigned int total_ticks() {
    return tick_counter;
}
