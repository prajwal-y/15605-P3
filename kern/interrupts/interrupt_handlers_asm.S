/** @file interrupt_handler_asm.S //TODO: Rename this file
 *
 *  asm file for interrupt handlers
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

.globl page_fault_handler
page_fault_handler:
	pusha
	call page_fault_handler_c
	popa
	popl %ecx
	iret
