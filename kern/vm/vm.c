/** @file vm.c
 *  @brief implementation of virtual memory functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <vm/vm.h>
#include <common/malloc_wrappers.h>
#include <util.h>
#include <string.h>
#include <elf_410.h>
#include <common_kern.h>
#include <page.h>
#include <cr.h>
#include <stddef.h>
#include <simics.h>
#include <seg.h>
#include <asm/asm.h>
#include <common/errors.h>
#include <common/assert.h>
#include <allocator/frame_allocator.h>

#define USER_PD_ENTRY_FLAGS PAGE_ENTRY_PRESENT | READ_WRITE_ENABLE | USER_MODE

static void zero_fill(void *addr, int size);
static void direct_map_kernel_pages(void *pd_addr);
static void setup_direct_map();
static int map_text_segment(simple_elf_t *se_hdr, void *pd_addr);
static int map_data_segment(simple_elf_t *se_hdr, void *pd_addr);
static int map_rodata_segment(simple_elf_t *se_hdr, void *pd_addr);
static int map_bss_segment(simple_elf_t *se_hdr, void *pd_addr);
static int map_stack_segment(void *pd_addr);
static int map_segment(void *start_addr, unsigned int length, int *pd_addr, int flags);
static void *direct_map[USER_MEM_START / (PAGE_SIZE * NUM_PAGE_TABLE_ENTRIES)];

static void *create_page_table();
//static void free_page_table(void *pt_addr);

//static void set_segment_selectors(int type);

/** @brief initialize the virtual memory system
 *
 *  Set up the direct map for kernel memory and any other
 *  initialization routines
 *
 *  @return void
 */
void vm_init() {
    setup_direct_map();
}

/** @brief Sets the control register %cr3 with the given
 * address of the page directory address.
 *
 * @param pd_addr The page directory address that needs to
 * be set in %cr3
 *
 * @return Void
 */
void set_cur_pd(void *pd_addr) {
	set_cr3((uint32_t)pd_addr);
}

/** @brief enable VM 
 *
 *  Set bit 31 of cr0
 *
 *  @return void
 */
void enable_paging() {
    unsigned int cr0 = get_cr0();
    cr0 = cr0 | CR0_PG;
    set_cr0(cr0);
}

void disable_paging() {
	unsigned int cr0 = get_cr0();
    cr0 = cr0 & (~CR0_PG);
    set_cr0(cr0);
}

/** @brief create a new page directory 
 *
 *  This function will allocate a new kernel physical frame for 
 *  a page directory and return the address of this physical frame.
 *  The frame will be initialized with default flags. If there are 
 *  no more physical frames available this function will return null.
 *  
 *  @return address of the frame containing the page directory.
 *          NULL on failure
 */
void *create_page_directory() {
	int *frame_addr = (int *) smemalign(PAGE_SIZE, PAGE_SIZE);
    if(frame_addr == NULL) {
        return NULL;
    }
    direct_map_kernel_pages(frame_addr);
	int i;
	for(i=KERNEL_MAP_NUM_ENTRIES; i<NUM_PAGE_TABLE_ENTRIES; i++) {
		frame_addr[i] = PAGE_DIR_ENTRY_DEFAULT;
	}
    return (void *)frame_addr;
}
        
/** @brief free a page directory 
 *
 *  frees the specified page directory using sfree
 *  
 *  @return void
 */

void free_page_directory(void *pd_addr) {
    if (pd_addr == NULL) {
        return;
    }
	sfree(pd_addr, PAGE_SIZE);
}

/** @brief create a new page table
 *
 *  This function will allocate a new kernel physical frame for 
 *  a page table and return the address of this physical frame.
 *  The frame will be initialized with default flags. If there are 
 *  no more physical frames available this function will return null.
 *  
 *  @return address of the frame containing the page table. NULL on failure
 */
void *create_page_table() {
	int *frame_addr = (int *) smemalign(PAGE_SIZE, PAGE_SIZE);
    if(frame_addr == NULL) {
        return NULL;
    }
	int i;
	for(i=0; i<NUM_PAGE_TABLE_ENTRIES; i++) {
		frame_addr[i] = PAGE_TABLE_ENTRY_DEFAULT;
	}
    return (void *)frame_addr;
}
        
/** @brief free a page table
 *
 *  frees the specified page table using sfree
 *  
 *  @return void
 */
/*void free_page_table(void *pt_addr) {
    if (pt_addr == NULL) {
        return;
    }
	sfree(pt_addr, PAGE_SIZE);
}*/

/** @brief setup paging for a program
 *
 *  this function reads a simple_elf_t and creates mappings in the 
 *  page directory/page table for the regions found in the elf header.
 *  This function DOES NOT copy the data from the binary to these
 *  regions. That is the responsisbility of the loader. 
 *
 *  @param se_hdr pointer to a simple_elf_t containing info about the 
 *                program to be loaded
 *  @param pd_addr the address of the page directory obtained through
 *                 a previous call to create_page_directory
 *
 *  @return 0 on success. Negative number on failure
 */
int setup_page_table(simple_elf_t *se_hdr, void *pd_addr) {
    int retval;
    if((retval = map_text_segment(se_hdr, pd_addr)) < 0) {
        return retval;
    }
    if((retval = map_data_segment(se_hdr, pd_addr)) < 0) {
        return retval;
    }
    if((retval = map_rodata_segment(se_hdr, pd_addr)) < 0) {
        return retval;
    }
    if((retval = map_bss_segment(se_hdr, pd_addr)) < 0) {
        return retval;
    }
    if((retval = map_stack_segment(pd_addr)) < 0) {
        return retval;
    }
    return 0;
}

/* ---------- Static local functions ----------- */

/** @brief zero fill a memory location
 *
 * @param addr the starting address of the memory location to be zeroed
 * @param size the number of bytes to be zeroed
 *
 * @return void
 */
void zero_fill(void *addr, int size) {
    if (addr == NULL) {
        return;
    }
    memset(addr, 0, size);
    return;
}

/** @brief direct map the kernel memory space
 *
 *  maps the lower 16 MB of the virtual memory to the lower 16 MB of
 *  physical memory. These pages are neither readable nor writable.
 *  All processes share the same page tables for the bottom 16 MB (i.e
 *  the page directory for all processes point to the same page tables for
 *  kernel memory). Therefore this function should be called once the mapping
 *  has been setup (setup_direct_map) and this simply copies the value in the
 *  direct_map array to the page directory.
 *
 * @param pd_addr the page directory where the direct mapping has to be 
 * 			performed
 *
 * @return void
 */
void direct_map_kernel_pages(void *pd_addr) {
    int flags = PAGE_ENTRY_PRESENT | READ_WRITE_ENABLE;
    int i;
    for (i = 0; i < KERNEL_MAP_NUM_ENTRIES; i++) {
        ((int *)pd_addr)[i] = (int)direct_map[i] | flags;
    }
}

/** @brief set up the direct map for kernel memory
 *
 *  get 4 frames required to map the bottom 16 MB of physical address space
 *  and write the page table entries corresponding to the 16 MB along with a
 *  no read/write protection policy. These 4 frames are stored in the array 
 *  direct_map which are used whenever a new page directory is initialized. 
 *  The page directory points to these 4 page tables
 *
 *  @return void
 */
void setup_direct_map() {
    int flags = PAGE_ENTRY_PRESENT | GLOBAL_PAGE_ENTRY | READ_WRITE_ENABLE;
    int i = 0, j = 0, mem_start = 0, page_table_entry;

    for (i = 0; i < KERNEL_MAP_NUM_ENTRIES; i++) {
        int *frame_addr = (int *)create_page_table();

        kernel_assert(frame_addr != NULL);

        for (j = 0; j < NUM_PAGE_TABLE_ENTRIES; j++) {
            page_table_entry = mem_start | flags;
            frame_addr[j] = page_table_entry;
            mem_start += PAGE_SIZE;
        }                
        direct_map[i] = frame_addr;
    }
}

/** @brief map the text segment into virtual memory
 *
 *  This function checks the address of the start of the text
 *  segment and length to define the number of physical frames
 *  required for the text segment. Then call a function which allocates
 *  free frames from the frame pool, sets up the mapping in the page directory.
 *
 *  @param se_hdr the parsed elf header
 *  @param pd_addr the address of the page directory frame
 *  @return int error code, 0 on success negative integer on failure
 */
int map_text_segment(simple_elf_t *se_hdr, void *pd_addr) {
    int flags = PAGE_ENTRY_PRESENT | USER_MODE;
    return map_segment((void *)se_hdr->e_txtstart, se_hdr->e_txtlen, 
						pd_addr, flags);
}

/** @brief map the data segment into virtual memory
 *
 *  This function checks the address of the start of the data
 *  segment and length to define the number of physical frames
 *  required for the text segment. Then call a function which allocates
 *  free frames from the frame pool, sets up the mapping in the page directory.
 *
 *  @param se_hdr the parsed elf header
 *  @param pd_addr the address of the page directory frame
 *  @return int error code, 0 on success negative integer on failure
 */
int map_data_segment(simple_elf_t *se_hdr, void *pd_addr) {
    //int flags = PAGE_ENTRY_PRESENT | READ_WRITE_ENABLE | USER_MODE;
    int flags = PAGE_ENTRY_PRESENT | USER_MODE;
    return map_segment((void *)se_hdr->e_datstart, se_hdr->e_datlen, 
						pd_addr, flags);
}

/** @brief map the rodata segment into virtual memory
 *
 *  This function checks the address of the start of the rodata
 *  segment and length to define the number of physical frames
 *  required for the text segment. Then call a function which allocates
 *  free frames from the frame pool, sets up the mapping in the page directory.
 *
 *  @param se_hdr the parsed elf header
 *  @param pd_addr the address of the page directory frame
 *  @return int error code, 0 on success negative integer on failure
 */
int map_rodata_segment(simple_elf_t *se_hdr, void *pd_addr) {
    int flags = PAGE_ENTRY_PRESENT | USER_MODE;
    return map_segment((void *)se_hdr->e_rodatstart, 
						se_hdr->e_rodatlen, pd_addr, flags);
}

/** @brief map the bss segment into virtual memory
 *
 *  This function checks the address of the start of the bss
 *  segment and length to define the number of physical frames
 *  required for the text segment. Then call a function which allocates
 *  free frames from the frame pool, sets up the mapping in the page directory.
 *
 *  @param se_hdr the parsed elf header
 *  @param pd_addr the address of the page directory frame
 *  @return int error code, 0 on success negative integer on failure
 */
int map_bss_segment(simple_elf_t *se_hdr, void *pd_addr) {
    int flags = PAGE_ENTRY_PRESENT | READ_WRITE_ENABLE | USER_MODE;
    return map_segment((void *)se_hdr->e_bssstart, se_hdr->e_bsslen, 
						pd_addr, flags);
}

/** @brief map the stack segment into virtual memory
 *
 *  @param se_hdr the parsed elf header
 *  @param pd_addr the address of the page directory frame
 *  @return int error code, 0 on success negative integer on failure
 */
int map_stack_segment(void *pd_addr) {
    int flags = PAGE_ENTRY_PRESENT | READ_WRITE_ENABLE | USER_MODE;
    return map_segment((char *)STACK_START - DEFAULT_STACK_SIZE, 
						DEFAULT_STACK_SIZE, pd_addr, flags); 
}

/** @brief map a segment into memory
 *
 *  This function takes the starting virtual address and the length. Then
 *  calculate the total number of frames required for this range of memory 
 *  allocate those many frames and set up the mapping in the page directory.
 *
 *  @param start_addr the start of the virtual address
 *  @param length the length of this memory segment
 *  @param pd_addr the address of the page directory
 *
 *  @return int error code, 0 on success negative integer on failure
 */
int map_segment(void *start_addr, unsigned int length, int *pd_addr, int flags) {
    void *end_addr = (char *)start_addr + length;
    int pd_index, pt_index;
    int *pt_addr;

    start_addr = (void *)((int)start_addr & PAGE_ROUND_DOWN);
    while (start_addr <= end_addr) {
        pd_index = GET_PD_INDEX(start_addr);
        pt_index = GET_PT_INDEX(start_addr);
        if (pd_addr[pd_index] == PAGE_DIR_ENTRY_DEFAULT) { /* Page directory entry absent */
            void *new_pt = create_page_table();
            if (new_pt != NULL) {
                pd_addr[pd_index] = (unsigned int)new_pt | USER_PD_ENTRY_FLAGS;
            }
            else {
                return ERR_NOMEM;
            }
        }
        pt_addr = (int *)GET_ADDR_FROM_ENTRY(pd_addr[pd_index]);
        if (pt_addr[pt_index] == PAGE_TABLE_ENTRY_DEFAULT) { /* Page table entry absent */
            /* Need to allocate frame from user free frame pool */
            void *new_frame = allocate_frame(); 
            if (new_frame != NULL) {
                pt_addr[pt_index] = (unsigned int)new_frame | flags;
                zero_fill(start_addr, PAGE_SIZE);
            }
            else {
                return ERR_NOMEM;
            }
        }
        start_addr = (char *)start_addr + PAGE_SIZE;
    }
    return 0;
}
