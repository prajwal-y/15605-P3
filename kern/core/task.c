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
#include <loader/loader.h>
#include <ureg.h>
#include <malloc_internal.h>

static void set_bootstrap_regs(ureg_t *reg);
static uint32_t setup_eflags();

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
        //Oh noes! no moar mamory! Do something!
        return;
    }
    /* ask vm to give us a zero filled frame for the page directory */
    void *pd_addr = create_page_directory();
    lprintf("Page directory is at %p", pd_addr);
    if (pd_addr == NULL) {
        //free_page_directory();
        //Oh noes! no moar mamory! Do something!
        return;
    }
    t->pdbr = pd_addr;

    /* Read the idle task header to set up VM */
    simple_elf_t *se_hdr = (simple_elf_t *)lmm_alloc(&malloc_lmm,
                            sizeof(simple_elf_t), LMM_ANY_REGION_FLAG);
    elf_load_helper(se_hdr, prog_name);
    lprintf("fnished loading elf");
    
    /* Invoke VM to setup the page directory/page table for a given binary */
    setup_page_table(se_hdr, pd_addr);
    lprintf("Set up the page table!");

    /* Paging enabled! */
	set_cur_pd(pd_addr);
    enable_paging();
    lprintf("enabled paging");

    /* Copy program into memory */
    load_program(se_hdr);

    /* Create the register set */
    ureg_t *reg = (ureg_t *)lmm_alloc(&malloc_lmm, sizeof(ureg_t),
                                      LMM_ANY_REGION_FLAG);
    set_bootstrap_regs(reg);

    /* Create a thread */
	thread_init();
    thread_struct_t *thr = create_thread(t, reg);

	current_thread = thr;

	uint32_t EFLAGS = setup_eflags();

	lprintf("Going to call IRET to enter: %p", (void *)se_hdr->e_entry);

	/*Set up esp0*/
	void *stack = lmm_alloc(&malloc_lmm, 1024, LMM_ANY_REGION_FLAG);
	if(stack != NULL) {
		lprintf("Setting stack for returning later");
		set_esp0((uint32_t)stack);
	}

	call_iret(EFLAGS, se_hdr->e_entry);
   	 
}

/* ------------ Static local functions --------------*/

/** @brief Creates the register set for the bootstrap task
 *
 *  @param reg Register set which needs to be initialized
 *
 *  @return Void
 */
void set_bootstrap_regs(ureg_t *reg) {
    reg->cause = 0;
    reg->cr2 = 0;
    reg->ds = SEGSEL_USER_DS;
    reg->es = SEGSEL_USER_DS;
    reg->fs = SEGSEL_USER_DS;
    reg->gs = SEGSEL_USER_DS;
    reg->ss = SEGSEL_USER_DS;
    reg->cs = SEGSEL_USER_CS;
    reg->eax = 0;
}

uint32_t setup_eflags() {
	uint32_t eflags = get_eflags();
	eflags |= 0x00000002;
	eflags |= 0x00003000;
	eflags |= 0x00000200;
	eflags &= 0xFFFbFFFF;
	return eflags;
}
