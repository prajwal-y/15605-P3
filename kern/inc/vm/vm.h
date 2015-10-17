/** @file vm.h
 *  @brief prototypes for virtual memory
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __VM_H
#define __VM_H

#define PAGE_ENTRY_PRESENT 1
#define READ_WRITE_ENABLE 2
#define USER_MODE 4
#define WRITE_THROUGH_CACHING 8
#define DISABLE_CACHING 16
#define GLOBAL_PAGE_ENTRY 256
#define LMM_ANY_REGION_FLAG 0

#include <elf_410.h>

void vm_init();

void *create_page_directory();

void free_page_directory(void *pd_addr);

void setup_page_table(simple_elf_t *se_hdr, void *pd_addr);

void enable_paging();

#endif /* __VM_H */
