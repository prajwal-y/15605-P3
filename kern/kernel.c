/** @file kernel.c
 *  @brief An initial kernel.c
 *
 *  You should initialize things in kernel_main(),
 *  and then run stuff.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 *  @bug No known bugs.
 */

#include <common_kern.h>

/* libc includes. */
#include <stdio.h>
#include <simics.h>                 /* lprintf() */

/* multiboot header file */
#include <multiboot.h>              /* boot_info */

/* x86 specific includes */
#include <x86/asm.h>                /* enable_interrupts() */
#include <x86/seg.h>                /* install_user_segs() */
#include <interrupts/interrupt_handlers.h>  
#include <drivers/console/console_util.h>
#include <console.h>
#include <allocator/frame_allocator.h>

static void set_default_color();

/** @brief Kernel entrypoint.
 *  
 *  This is the entrypoint for the kernel.
 *
 * @return Does not return
 */
int kernel_main(mbinfo_t *mbinfo, int argc, char **argv, char **envp)
{
    set_default_color();
    install_handlers();
    enable_interrupts();
    clear_console();
    init_frame_allocator();
    putbytes("foo", 3);

    while (1) {
        continue;
    }

    return 0;
}

/** @brief function to set the default color of the console
 *
 *  By default the console will have a background color of black (0x00)
 *  and foreground color of white (0xF). This function also sets a 2 byte
 *  array to the empty character which is used for backspace and for scrolling
 *  among other things. Thi 2 byte array is memcpy'ed into video memory locations
 *  where an empty character is required.
 *
 *  @return void
 */
void set_default_color() {
    console_color = BGND_BLACK | FGND_WHITE;
    empty_char[0] = ' ';
    empty_char[1] = console_color;
}
