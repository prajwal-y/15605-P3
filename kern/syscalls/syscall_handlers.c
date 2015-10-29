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

#define NUM_INTERRUPTS 10 /* The number of interrupts we have defined handlers for */
#define IDT_ENTRY_SIZE 8  /* Size of each IDT */

static void install_gettid_handler();
static void install_print_handler();
static void install_fork_handler();
static void install_exec_handler();

/** @brief The syscall handlers initialization function
 *
 *   @return int 0 on success and negative number on failure
 **/
int install_syscall_handlers() {
    install_gettid_handler();
    install_print_handler();
	install_fork_handler();
	install_exec_handler();
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
