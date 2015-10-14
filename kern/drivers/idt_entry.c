/** @file idt_entry.c
 *  @brief Implementation of functions to read and 
 *         manipulate IDT entries
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 */

#include <idt_entry.h>
#include <asm.h>
#include <seg.h>
#include <malloc.h>
#include <string/string.h>

/** @brief a struct to store the fields of an IDT entry
 *         in a compact fashion. 
 */
typedef struct {
    unsigned offset_1:16;
    unsigned seg_selector:16;
    unsigned reserved:5;
    unsigned zeroes:3;
    unsigned flags:5;
    unsigned dpl:2;
    unsigned present:1;
    unsigned offset_2:16;
} idt_entry;

static idt_entry *get_default_idt();

/** @brief function to add an entry in the IDT table
 *
 *  The function calls the idt_base() function to retrieve the 
 *  base of the IDT and uses the interrupt_num to figure out the 
 *  right memory location to copy the interrupt handler to
 *
 *  @param handler the address of the interrupt handler
 *         interrupt_num the interrupt for which we wish to install an
 *                       interrupt handler
 *  @return void
 */
void add_idt_entry(void (*handler)(void), int interrupt_num) {
    void *idt_base_addr = idt_base();
    idt_entry *entry = get_default_idt();

    entry->offset_1 = (int)handler & 0xffff;
    entry->offset_2 = ((int)handler >> 16) & 0xffff;
    memcpy((char *)idt_base_addr + (8 * interrupt_num), entry, 
            sizeof(idt_entry));

    free(entry);
}

/** @brief function to return a pointer to an idt_entry struct 
 *         with the default values filled in
 *
 *  @return idt_entry * a pointer to a malloc'ed idt_entry
 *          with default entries filled
 */
idt_entry *get_default_idt() {
    idt_entry *entry = malloc(sizeof(idt_entry));

    entry->seg_selector = SEGSEL_KERNEL_CS;
    entry->zeroes = 0;
    entry->flags = 15;
    entry->dpl = 0;
    entry->present = 1;

    return entry;
}
