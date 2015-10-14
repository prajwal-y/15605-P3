/** @file keyboard.c
 *  @brief implementation of functions defined in keyboard.h
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @bugs If no application calls readchar() and keys are pressed
 *        continuously we will run out of heap space
 */
#include <keyboard/keyboard.h>
#include <keyboard/keyboard_handler.h>
#include <keyhelp.h>
#include <idt_entry.h>
#include <interrupt_handlers.h>
#include <asm.h>
#include <malloc.h>
#include <list.h>
#include <stddef.h>

/** @brief a struct which represents a node in our
 *         keyboard buffer
 *
 *  Unlike usual linked lists where a one-size-fits-all node
 *  is used to store data, here we store metadata in the struct
 *  to help maintain a doubly linked list.
 */
typedef struct {
    unsigned char code;
    list_head link;
} scancode;
static list_head head; /* Dummy node which does not have any data */
static void initialize_queue();

/** @brief function to install the handler and initialize
 *         our scancode buffer 
 *
 *  @return void
 */
void install_keyboard_handler() {
    add_idt_entry(keyboard_handler, KEY_IDT_ENTRY);
    initialize_queue();
}

/** @brief initialize our queue by setting a dummy head and 
 *         setting the head to point to itself
 *
 *  @return void
 */
void initialize_queue() {
    head = ((scancode *)malloc(sizeof(scancode)))->link;
    init_head(&head);
}

/** @brief add the scancode to the end of our queue and return
 *
 *  @return void 
 */
void enqueue_scancode() {
    unsigned char in = inb(KEYBOARD_PORT);

    scancode *code = (scancode *)malloc(sizeof(scancode));
    code->code = in;
    add_to_tail(&code->link, &head);
    acknowledge_interrupt();
    return;
}

/** @brief Returns the next character in the keyboard buffer
 *
 *  This function does not block if there are no characters in the keyboard
 *  buffer
 *
 *  @return The next character in the keyboard buffer, or -1 if the keyboard
 *          buffer is currently empty
 **/
int readchar() {
    list_head *p;
    while ((p = get_first(&head)) != NULL) { 
        scancode *scan = get_entry(p, scancode, link);
        del_entry(p);
        kh_type key = process_scancode(scan->code);
        free(scan);
        if ((KH_HASDATA(key) != 0) && (KH_ISMAKE(key) == 0)) {
            return KH_GETCHAR(key);
        }
    }
    return -1;
}
