/** @file asm.S
 *
 *  Implementations of the set segment selector 
 *  register functions defined in asm.h
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <simics.h>

.globl get_cs
get_cs:
	movl %cs, %eax
	ret

.globl get_ds
get_ds:
	movl %ds, %eax
	ret

.globl get_ss
get_ss:
	movl %ss, %eax
	ret

.globl set_cs
set_cs:
    movl 4(%esp), %ecx /*Get the parameter*/
    mov %ecx, %cs  /* Set the code segment selector*/
    ret

.globl set_ss
set_ss:
    movl 4(%esp), %ecx
    mov %ecx, %ss /*Set the stack segment selector*/
    ret

.globl set_ds
set_ds:
    movl 4(%esp), %ecx
    mov %ecx, %ds /*Set the data segment selector*/
    ret

.globl set_es
set_es:
    movl 4(%esp), %ecx
    mov %ecx, %es
    ret

.globl set_fs
set_fs:
    movl 4(%esp), %ecx
    mov %ecx, %fs
    ret

.globl set_gs
set_gs:
    movl 4(%esp), %ecx
    mov %ecx, %gs
    ret

.globl call_iret
call_iret:
	movl %esp, %ecx
	mov $0x2b, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	
	pushl $0x2b			/*SS*/
	pushl $0xc0000000	/*ESP*/
	pushl 4(%ecx)		/*EFLAGS*/
	pushl $0x23			/*CS*/
	pushl 8(%ecx)		/*EIP*/
	iret
