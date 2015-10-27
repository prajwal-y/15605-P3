/** @file interrupt_handler_asm.S //TODO: RENAME THIS FILE
 *
 *  asm file for interrupt handlers
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#ifndef __INTERRUPT_HANDLER_ASM_H
#define __INTERRUPT_HANDLER_ASM_H

/** @brief The page fault handler
 *  
 *  This function calls the C page fault handler.
 *
 *  @return void
 */
void page_fault_handler();

#endif
