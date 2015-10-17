/** @file vm.c
 *  @brief implementation of virtual memory functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <vm/vm.h>
#include <lmm.h>
#include <util.h>
#include <string.h>
#include <elf_410.h>
#include <common_kern.h>
#include <page.h>
#include <cr.h>
#include <stddef.h>
#include <simics.h>
#include <common/errors.h>
#include <allocator/frame_allocator.h>
#include <malloc/malloc_internal.h>

#define PAGE_TABLE_ENTRY_SIZE 4
#define PAGE_DIRECTORY_MASK 0xFFC0000
#define PAGE_TABLE_MASK 0x003ff000
#define PAGE_ROUND_DOWN 0xfffff000
#define NUM_PAGE_TABLE_ENTRIES (PAGE_SIZE / PAGE_TABLE_ENTRY_SIZE)
#define DEFAULT_STACK_SIZE 1 * 1024 * 1024
#define STACK_START 0xc0000000

#define GET_PD_INDEX(addr) ((unsigned int)((int)(addr) & PAGE_DIRECTORY_MASK) >> 22)
#define GET_PT_INDEX(addr) ((unsigned int)((int)(addr) & PAGE_TABLE_MASK) >> 12)
#define KERNEL_MAP_NUM_ENTRIES (sizeof(direct_map) / sizeof(direct_map[0]))


static void zero_fill(void *addr, int size);
static void direct_map_kernel_pages(void *pd_addr);
static void setup_direct_map();
static int map_text_segment(simple_elf_t *se_hdr, void *pd_addr);
static int map_data_segment(simple_elf_t *se_hdr, void *pd_addr);
static int map_rodata_segment(simple_elf_t *se_hdr, void *pd_addr);
static int map_bss_segment(simple_elf_t *se_hdr, void *pd_addr);
static int map_stack_segment(void *pd_addr);
static int map_segment(void *start_addr, unsigned int length, int *pd_addr);
static void *direct_map[USER_MEM_START / (PAGE_SIZE * NUM_PAGE_TABLE_ENTRIES)];

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

/** @brief enable VM 
 *
 *  Set bit 31 of cr0
 *
 *  @return void
 */
void enable_paging() {
    unsigned int cr0 = get_cr0();
    lprintf("cr0 is %d and CR0_PG is %d",cr0, CR0_PG);
    cr0 = cr0 | CR0_PG;
    lprintf("cr0 after setting is %d",cr0);
    set_cr0(cr0);
    lprintf("cr0 is set");
}

/** @brief create a new page directory 
 *
 *  This function will allocate a new kernel physical frame for 
 *  a page directory and return the address of this physical frame.
 *  The frame will be zero filled before returning. If there are 
 *  no more physical frames available this function will return null.
 *  
 *  @return address of the frame containing the page directory
 */
void *create_page_directory() {
    void *frame_addr = lmm_alloc_page(&malloc_lmm, LMM_ANY_REGION_FLAG);
    if (frame_addr != NULL) {
        zero_fill(frame_addr, PAGE_SIZE);
    }
    return frame_addr;
}
        
/** @brief free a page directory 
 *
 *  frees the specified page directory using lmm_free
 *  
 *  @return void
 */

void free_page_directory(void *pd_addr) {
    if (pd_addr == NULL) {
        return;
    }
    lmm_free(&malloc_lmm, pd_addr, PAGE_SIZE);
    return;
}

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
 */
void setup_page_table(simple_elf_t *se_hdr, void *pd_addr) {
    direct_map_kernel_pages(pd_addr);
    lprintf("mapped kernel pages");
    // TODO: If any of the mappings fail we nee to fail in some graceful way
    map_text_segment(se_hdr, pd_addr);
    lprintf("mapped text segment");
    map_data_segment(se_hdr, pd_addr);
    lprintf("mapped data segment");
    map_rodata_segment(se_hdr, pd_addr);
    lprintf("mapped rodata segment");
    map_bss_segment(se_hdr, pd_addr);
    lprintf("mapped bss segment");
    map_stack_segment(pd_addr);
    lprintf("mapped stack segment");
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
 * @param pd_addr the page directory where the direct mapping has to be performed
 *
 * @return void
 */
void direct_map_kernel_pages(void *pd_addr) {
    int flags = PAGE_ENTRY_PRESENT;
    int i;
    lprintf("sizeof direct-map is %d", sizeof(direct_map));
    for (i = 0; i < KERNEL_MAP_NUM_ENTRIES; i++) {
        lprintf("i is %d", i);
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
    int flags = PAGE_ENTRY_PRESENT | GLOBAL_PAGE_ENTRY;
    int i = 0, j = 0, mem_start = 0, page_table_entry;

    lprintf("sizeof direct-map is %d", sizeof(direct_map));
    for (; i < KERNEL_MAP_NUM_ENTRIES; i++) {
        lprintf("i is %d", i);
        int *frame_addr = (int *)lmm_alloc_page(&malloc_lmm, LMM_ANY_REGION_FLAG);
        if (frame_addr != NULL) {
            zero_fill(frame_addr, PAGE_SIZE);
            for (j = 0; j < NUM_PAGE_TABLE_ENTRIES; j++) {
                //lprintf("j is %d", j);
                page_table_entry = mem_start | flags;
                mem_start += PAGE_SIZE;
                frame_addr[j] = page_table_entry;
            }                
            //MAGIC_BREAK;
            direct_map[i] = frame_addr;
        } 
        else {
            //TODO: Error unrecoverable, kill OS
        }
    }
    lprintf("Allocated a page");
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
    lprintf("text segment startung at %p with length %d", (void *)se_hdr->e_txtstart, (int)se_hdr->e_txtlen);
    return map_segment((void *)se_hdr->e_txtstart, se_hdr->e_txtlen, pd_addr);
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
    lprintf("data segment startung at %p with length %d", (void *)se_hdr->e_datstart, (int)se_hdr->e_datlen);
    return map_segment((void *)se_hdr->e_datstart, se_hdr->e_datlen, pd_addr);
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
    lprintf("rodata segment startung at %p with length %d", (void *)se_hdr->e_rodatstart, (int)se_hdr->e_rodatlen);
    return map_segment((void *)se_hdr->e_rodatstart, se_hdr->e_rodatlen, pd_addr);
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
    lprintf("BSS segment startung at %p with length %d", (void *)se_hdr->e_bssstart, (int)se_hdr->e_bsslen);
    return map_segment((void *)se_hdr->e_bssstart, se_hdr->e_bsslen, pd_addr);
}

/** @brief map the stack segment into virtual memory
 *
 *  @param se_hdr the parsed elf header
 *  @param pd_addr the address of the page directory frame
 *  @return int error code, 0 on success negative integer on failure
 */
int map_stack_segment(void *pd_addr) {
    lprintf("stack segment startung at %p with length %d", (void *)STACK_START, DEFAULT_STACK_SIZE);
    return map_segment((void *)STACK_START - DEFAULT_STACK_SIZE, DEFAULT_STACK_SIZE, pd_addr);   
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
int map_segment(void *start_addr, unsigned int length, int *pd_addr) {
    void *end_addr = (char *)start_addr + length;
    int flags = PAGE_ENTRY_PRESENT;
    int pd_index, pt_index;
    int *pt_addr;

    start_addr = (void *)((int)start_addr & PAGE_ROUND_DOWN);
    /*if (start_addr == (void *)0x01002000) {
        lprintf("End address is %p", end_addr);
    }*/
    while (start_addr < end_addr) {
        pd_index = GET_PD_INDEX(start_addr);
        pt_index = GET_PT_INDEX(start_addr);
        if (pd_addr[pd_index] == 0) { /* Page directory entry absent */
            void *new_pt = lmm_alloc_page(&malloc_lmm, LMM_ANY_REGION_FLAG);
            if (new_pt != NULL) {
                zero_fill(new_pt, PAGE_SIZE);
                pd_addr[pd_index] = (unsigned int)new_pt | flags;
            }
            else {
                return ERR_NOMEM;
            }
        }
        pt_addr = (int *)pd_addr[pd_index];
        if (pt_addr[pt_index] == 0) { /* Page table entry absent */
            /* Need to allocate frame from user free frame pool */
            void *new_frame = allocate_frame(); 
            lprintf("new frame is %p", new_frame);
            if (new_frame != NULL) {
                zero_fill(new_frame, PAGE_SIZE);
                //lprintf("Zero filled physical frame");
                pt_addr[pt_index] = (unsigned int)new_frame | flags;
                //lprintf("Set page teable entry");
            }
            else {
                //TODO: Free the allocated PTE maybe???
                return ERR_NOMEM;
            }
        }
        start_addr = (char *)start_addr + PAGE_SIZE;
    }
    return 0;
}
