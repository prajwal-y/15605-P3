/** @file syscall_util_asm.h
 *
 *  Helper asm functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#define SAVE_REGS \
		pushl %ecx; \
		pushl %eax; \
    	pushl %edx; \
    	pushl %ebx; \
    	pushl %esp; \
    	pushl %ebp; \
    	pushl %edi; \
    	pushl %esi;

#define RESTORE_REGS \
		popl %esi; \
	    popl %edi; \
    	popl %ebp; \
	    popl %esp; \
	    popl %ebx; \
	    popl %edx; \
	    popl %ecx; \
		popl %ecx;
