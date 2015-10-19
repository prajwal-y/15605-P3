/** @file idt_entry.h
 *  @brief helper functions to manipulate and define the 
 *         IDT entry
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 */
#ifndef __IDT_ENTRY_H
#define __IDT_ENTRY_H

#define TRAP_GATE 0
#define INTERRUPT_GATE 1

void add_idt_entry(void (*handler)(void), int interrupt_num, int type); 

#endif  /* __IDT_ENTRY_H */
