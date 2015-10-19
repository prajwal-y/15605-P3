/** @file task.c
 *  @brief task manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <core/task.h>
#include <lmm.h>
#include <seg.h>
#include <cr.h>
#include <vm/vm.h>
#include <simics.h>
#include <stddef.h>
#include <eflags.h>
#include <core/task.h>
#include <asm/asm.h>
#include <core/thread.h>
#include <global_state.h>
#include <loader/loader.h>
#include <ureg.h>
#include <malloc_internal.h>

static uint32_t setup_user_eflags();

/** @brief Creates a new task by copying the current
 *  page directory address.
 *
 *  @return Void
 */
void create_task() {
}

/** @brief start a bootstrap task
 *
 *  This process is started in a special way since it is one of
 *  the first processes to run on the system. We use an IRET
 *  to get to running this task.
 *  Multithreaded hazards: None (no other threads running)
 *  1) create a task struct for the task
 *  2) get a frame to hold page directory information
 *  3) parse ELF binary header
 *  4) set up paging information with the ELF binary
 *  5) enable paging
 *  6) load program contents into segments
 *  7) create a thread
 *  8) ??
 *  9) IRET!!
 *
 *  @return void
 */
void load_bootstrap_task(const char *prog_name) {
    /* Allocate memory for a task struct from kernel memory */
    task_struct_t *t = (task_struct_t *)lmm_alloc(&malloc_lmm, 
                       sizeof(task_struct_t), LMM_ANY_REGION_FLAG);
    if (t == NULL) {
        return;
    }
    /* ask vm to give us a zero filled frame for the page directory */
    void *pd_addr = create_page_directory();
    if (pd_addr == NULL) {
        return;
    }
    t->pdbr = pd_addr;

    /* Read the idle task header to set up VM */
    simple_elf_t *se_hdr = (simple_elf_t *)lmm_alloc(&malloc_lmm,
                            sizeof(simple_elf_t), LMM_ANY_REGION_FLAG);
    if (se_hdr == NULL) {
        return;
    }
    elf_load_helper(se_hdr, prog_name);
    
    /* Invoke VM to setup the page directory/page table for a given binary */
    setup_page_table(se_hdr, pd_addr);

    /* Paging enabled! */
	set_cur_pd(pd_addr);
    enable_paging();

    /* Copy program into memory */
    load_program(se_hdr);


    /* Create a thread */
    thread_struct_t *thr = create_thread(t);

	curr_thread = thr;

	uint32_t EFLAGS = setup_user_eflags();

	call_iret(EFLAGS, se_hdr->e_entry);
   	 
}

/* ------------ Static local functions --------------*/

/** @brief set user EFLAGS 
 *
 *  The EFLAGS for user have interrupts enabled (IF), IOPL set to 3
 *  (privilege level 3), alignment check off and bit 1 set.
 *
 *  @return void
 */
uint32_t setup_user_eflags() {
	uint32_t eflags = get_eflags();
	eflags |= 0x00000002;
	eflags |= 0x00003000;
	eflags |= 0x00000200;
	eflags &= 0xFFFbFFFF;
	return eflags;
}
