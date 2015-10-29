/** @file thread.c
 *  @brief thread manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <common/malloc_wrappers.h>
#include <vm/vm.h>
#include <loader/loader.h>
#include <core/thread.h>
#include <sync/mutex.h>
#include <cr.h>
#include <seg.h>
#include <syscall.h>
#include <stddef.h>
#include <simics.h>

static int next_tid;
static mutex_t mutex;
static void set_user_thread_regs(ureg_t *reg);

/** @brief Initializes the thread creation module
 *
 *  @return Void
 */
void kernel_threads_init() {
    next_tid = 0;
    mutex_init(&mutex);
}

/** @brief create a new thread.
 *
 *  @param task the task under which this thread will run
 *  @param regs the register values this thread must take
 *  @return thread_struct_t the newly created thread or null on failure.
 */
thread_struct_t *create_thread(task_struct_t *task) {
    if(task == NULL) {
        return NULL;
    }
    /* Create the register set */
	ureg_t *reg = (ureg_t *)smalloc(sizeof(ureg_t));
    if(reg == NULL) {
        return NULL;
    }

    set_user_thread_regs(reg);

    /* Create the thread struct */
	thread_struct_t *thr = (thread_struct_t *)smalloc(sizeof(thread_struct_t));
    if(thr == NULL) {
		sfree(reg, sizeof(ureg_t));
        return NULL;
    }
	/* Allocate space for thread's kernel stack */
	void *stack = smemalign(PAGE_SIZE, PAGE_SIZE);
    if(stack == NULL) {
        sfree(thr, sizeof(thread_struct_t));
        sfree(reg, sizeof(ureg_t));
        return NULL;
    }
    mutex_lock(&mutex);
    thr->id = ++next_tid;
    mutex_unlock(&mutex);

    thr->regs = reg;
    thr->parent_task = task;
    thr->k_stack = stack;
	thr->k_stack_base = (uint32_t)((char *)stack + PAGE_SIZE);
	thr->cur_esp = thr->k_stack_base;
	thr->cur_ebp = thr->k_stack_base;
    return thr;
}

/* --------------- Static local functions ----------------*/
/** @brief Creates the register set for the thread
 *
 *  @param reg Register set which needs to be initialized
 *
 *  @return Void
 */
void set_user_thread_regs(ureg_t *reg) {
    reg->ds = SEGSEL_USER_DS;
    reg->es = SEGSEL_USER_DS;
    reg->fs = SEGSEL_USER_DS;
    reg->gs = SEGSEL_USER_DS;
    reg->ss = SEGSEL_USER_DS;
    reg->cs = SEGSEL_USER_CS;
    reg->eax = 0;
}
