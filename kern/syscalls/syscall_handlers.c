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

#define NUM_INTERRUPTS 10 /* The number of interrupts we have defined handlers for */
#define IDT_ENTRY_SIZE 8  /* Size of each IDT */

static void install_gettid_handler();

/** @brief The syscall handlers initialization function
 *
 *   @return int 0 on success and negative number on failure
 **/
int install_syscall_handlers() {
    install_gettid_handler();
    return 0;
}

/** @brief this function installs a handler for gettid
 *
 *
 *  @return void
 */
void install_gettid_handler() {
    add_idt_entry(gettid_handler, GETTID_INT, INTERRUPT_GATE);
}
