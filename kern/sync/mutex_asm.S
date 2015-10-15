/** @file mutex_asm.S 
 *  
 *  Assembly wrappers for functions used by the thread library
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapdithaya (pyadapad)
 */
#include <syscall_int.h>

.global test_and_unset
test_and_unset:
    movl 4(%esp), %ecx  /*Get the parameter value (address of mutex variable)*/
    movl $0, %eax		/*Set 0 to eax*/
    xchg (%ecx), %eax	/*Atomically exchange the value*/
    ret
    
.global test_and_set
test_and_set:
    movl 4(%esp), %ecx	/*Get the parameter value (address of mutex variable)*/
    movl $1, %eax		/*Set 1 to eax*/
    xchg (%ecx), %eax	/*Atomically exchange the value*/
    ret
