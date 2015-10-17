/** @file asm.S
 *
 *  Implementations of the set segment selector 
 *  register functions defined in asm.h
 *  
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

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
