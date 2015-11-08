/** @file syscall_handlers.c
 *  @brief This file contains functions to install
 *         various syscall handlers.
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <asm.h>
#include <syscalls/syscall_handlers.h>
#include <syscall_int.h>
#include <interrupts/idt_entry.h>
#include <idt.h>
#include <syscalls/thread_syscalls.h>
#include <syscalls/console_syscalls.h>
#include <syscalls/lifecycle_syscalls.h>
#include <syscalls/misc_syscalls.h>
#include <syscalls/memory_syscalls.h>
#include <syscalls/system_check_syscalls.h>

#define NUM_INTERRUPTS 10 /* The number of interrupts we have defined handlers for */
#define IDT_ENTRY_SIZE 8  /* Size of each IDT */

static void install_gettid_handler();
static void install_print_handler();
static void install_fork_handler();
static void install_exec_handler();
static void install_set_status_handler();
static void install_halt_handler();
static void install_wait_handler();
static void install_vanish_handler();
static void install_new_pages_handler();
static void install_remove_pages_handler();
static void install_readline_handler();
static void install_yield_handler();
static void install_sleep_handler();

//TODO: Remove sanity check syscall
static void install_memcheck_handler();

/** @brief The syscall handlers initialization function
 *
 *   @return int 0 on success and negative number on failure
 **/
int install_syscall_handlers() {
    install_gettid_handler();
    install_print_handler();
	install_fork_handler();
	install_exec_handler();
	install_set_status_handler();
	install_halt_handler();
    install_wait_handler();
    install_vanish_handler();
    install_new_pages_handler();
    install_remove_pages_handler();
    install_readline_handler();
    install_yield_handler();
    install_memcheck_handler();
    install_sleep_handler();
    return 0;
}

/** @brief this function installs a handler for gettid
 *
 *  @return void
 */
void install_gettid_handler() {
    add_idt_entry(gettid_handler, GETTID_INT, TRAP_GATE, USER_DPL);
}

/** @brief this function installs a handler for print
 *
 *  @return void
 */
void install_print_handler() {
    add_idt_entry(print_handler, PRINT_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for fork
 *
 *  @return void
 */
void install_fork_handler() {
	add_idt_entry(fork_handler, FORK_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for exec
 *
 *  @return void
 */
void install_exec_handler() {
	add_idt_entry(exec_handler, EXEC_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for set_status
 *
 *  @return void
 */
void install_set_status_handler() {
	add_idt_entry(set_status_handler, SET_STATUS_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for halt
 *
 *  @return void
 */
void install_halt_handler() {
	add_idt_entry(halt_handler, HALT_INT, TRAP_GATE, USER_DPL);
}
/** @brief Function to install a handler for wait syscall
 *
 *  @return void
 */
void install_wait_handler() {
	add_idt_entry(wait_handler, WAIT_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for vanish syscall
 *
 *  @return void
 */
void install_vanish_handler() {
	add_idt_entry(vanish_handler, VANISH_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for new_pages syscall
 *
 *  @return void
 */
void install_new_pages_handler() {
	add_idt_entry(new_pages_handler, NEW_PAGES_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for readline syscall
 *
 *  @return void
 */
void install_readline_handler() {
	add_idt_entry(readline_handler, READLINE_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for remove_pages syscall
 *
 *  @return void
 */
void install_remove_pages_handler() {
	add_idt_entry(remove_pages_handler, REMOVE_PAGES_INT, TRAP_GATE, USER_DPL);
}

/** @brief Function to install a handler for yield syscall
 *
 *  @return void
 */
void install_yield_handler() {
	add_idt_entry(yield_handler, YIELD_INT, TRAP_GATE, USER_DPL);
}

void install_memcheck_handler() {
    add_idt_entry(memory_check_handler, MEMORY_CHECK_INT, INTERRUPT_GATE, USER_DPL);
}

/** @brief Function to install a handler for sleep syscall
 *
 *  @return int 0 on success, -ve integer on failure
 */
void install_sleep_handler() {
	add_idt_entry(sleep_handler, SLEEP_INT, TRAP_GATE, USER_DPL);
}
