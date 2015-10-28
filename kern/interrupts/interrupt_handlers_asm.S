/** @file interrupt_handler_asm.S //TODO: Rename this file
 *
 *  asm file for interrupt handlers
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

.globl page_fault_handler
page_fault_handler:
	pusha						/* Save the general purpose registers */
	call page_fault_handler_c	/* Call the C handler for page fault */
	popa						/* Restore the registers */
	popl %ecx 					/* Pop the error code */
	iret
