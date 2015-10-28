/** @file task.c
 *  @brief task manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <core/task.h>
#include <common/malloc_wrappers.h>
#include <seg.h>
#include <cr.h>
#include <vm/vm.h>
#include <simics.h>
#include <stddef.h>
#include <eflags.h>
#include <core/task.h>
#include <asm/asm.h>
#include <core/thread.h>
#include <core/scheduler.h>
#include <loader/loader.h>
#include <ureg.h>
#include <syscall.h>
#include <string.h>
#include <common/assert.h>

#define EFLAGS_RESERVED 0x00000002
#define EFLAGS_IOPL 0x00003000 
#define EFLAGS_IF 0x00000200 
#define EFLAGS_ALIGNMENT_CHECK 0xFFFbFFFF

static uint32_t setup_user_eflags();
static void set_task_stack(void *kernel_stack_base, int entry_addr);

/** @brief Create a new task
 *  
 *  @return task_struct_t The reference to the new task
 *  NULL if task creation failed
 */
task_struct_t *create_task() {
	task_struct_t *t = (task_struct_t *)smalloc(sizeof(task_struct_t));
	if(t == NULL) {
		return NULL;
	}
    thread_struct_t *thr = create_thread(t);
	if(thr == NULL) {
		sfree(t, sizeof(task_struct_t));
		return NULL;
	}
	t->thr = thr;
	t->id = thr->id;
    return t;
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

    int retval;
    /* ask vm to give us a zero filled frame for the page directory */
    void *pd_addr = create_page_directory();

    kernel_assert(pd_addr != NULL);

    /* Paging enabled! */
	set_cur_pd(pd_addr);
	enable_paging();

    /* Allocate memory for a task struct from kernel memory */
	task_struct_t *t = create_task();

    kernel_assert(t != NULL);
    t->pdbr = pd_addr;

    /* Read the idle task header to set up VM */
	simple_elf_t *se_hdr = (simple_elf_t *)smalloc(sizeof(simple_elf_t));

    kernel_assert(se_hdr != NULL);

    elf_load_helper(se_hdr, prog_name);
    
    /* Invoke VM to setup the page directory/page table for a given binary */
    retval = setup_page_table(se_hdr, pd_addr);
    kernel_assert(retval == 0);

    /* Copy program into memory */
    retval = load_program(se_hdr);
    kernel_assert(retval == 0);

	set_running_thread(t->thr);
    set_esp0(t->thr->k_stack_base);

	uint32_t EFLAGS = setup_user_eflags();

	lprintf("About to call iret at entry: %p", (void *)se_hdr->e_entry);

	call_iret(EFLAGS, se_hdr->e_entry);
   	 
}

void load_task(const char *prog_name) {

	int retval;
    /* ask vm to give us a zero filled frame for the page directory */
    void *pd_addr = create_page_directory();

    kernel_assert(pd_addr != NULL);

    /* Paging enabled! */
	set_cur_pd(pd_addr);
	enable_paging();

    /* Allocate memory for a task struct from kernel memory */
	task_struct_t *t = create_task();
    kernel_assert(t != NULL);

    t->pdbr = pd_addr;

    /* Read the idle task header to set up VM */
	simple_elf_t *se_hdr = (simple_elf_t *)smalloc(sizeof(simple_elf_t));

    kernel_assert(se_hdr != NULL);

    elf_load_helper(se_hdr, prog_name);
    
    /* Invoke VM to setup the page directory/page table for a given binary */
    retval = setup_page_table(se_hdr, pd_addr);
    kernel_assert(retval == 0);

    /* Copy program into memory */
    retval = load_program(se_hdr);
    kernel_assert(retval == 0);

	set_task_stack((void *)t->thr->k_stack_base, se_hdr->e_entry);
	t->thr->cur_esp = (t->thr->k_stack_base - DEFAULT_STACK_OFFSET);
    runq_add_thread(t->thr);
}

void set_task_stack(void *kernel_stack_base, int entry_addr) {
    /* Hand craft the kernel stack for these tasks */
    /* Add the registers required for IRET */
	uint32_t EFLAGS = setup_user_eflags();

    *((int *)(kernel_stack_base) - 1) = SEGSEL_USER_DS;
    *((int *)(kernel_stack_base) - 2) = STACK_START;
    *((int *)(kernel_stack_base) - 3) = EFLAGS;
    *((int *)(kernel_stack_base) - 4) = SEGSEL_USER_CS;
    *((int *)(kernel_stack_base) - 5) = entry_addr;

    /* Simulate a pusha for the 8 registers that are pushed */
    memset(((int *)(kernel_stack_base) - 13), 0, 32);

	*((int *)(kernel_stack_base) - 14) = (int)iret_fun;
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
    eflags |= EFLAGS_RESERVED;          /*Reserved bit*/
	eflags |= EFLAGS_IOPL;              /*IOPL*/
	eflags |= EFLAGS_IF;                /*IF*/
	eflags &= EFLAGS_ALIGNMENT_CHECK;   /*Alignment check off*/
	return eflags;
}
