/** @file idt_entry.h
 *  @brief helper functions to manipulate and define the 
 *         IDT entry
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 */
#ifndef __IDT_ENTRY_H
#define __IDT_ENTRY_H

void add_idt_entry(void (*handler)(void), int interrupt_num);

#endif  /* __IDT_ENTRY_H */
