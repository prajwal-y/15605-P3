/** @file interrupt_handlers.h
 *  @brief prototypes for interrupt handling utility
 *         functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 */
#ifndef __INTERRUPT_HANDLERS_H
#define __INTERRUPT_HANDLERS_H

void acknowledge_interrupt();

int install_handlers();

void page_fault_handler();

#endif  /* __INTERRUPT_HANDLERS_H */
