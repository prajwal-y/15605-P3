/* The 15-410 kernel project
 *
 *     loader.h
 *
 * Structure definitions, #defines, and function prototypes
 * for the user process loader.
 */

#ifndef __LOADER_H
#define __LOADER_H
#include <elf_410.h>

int load_program(simple_elf_t *se_hdr);

#endif /* __LOADER_H */
