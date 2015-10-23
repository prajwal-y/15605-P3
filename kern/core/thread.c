/** @file thread.c
 *  @brief thread manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <common/lmm_wrappers.h>
#include <malloc_internal.h>
#include <vm/vm.h>
#include <loader/loader.h>
#include <core/thread.h>
#include <sync/mutex.h>
#include <cr.h>
#include <seg.h>
#include <syscall.h>

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
    mutex_lock(&mutex);
    /* Create the register set */
    ureg_t *reg = (ureg_t *)lmm_alloc_safe(&malloc_lmm, sizeof(ureg_t),
                                      LMM_ANY_REGION_FLAG);
    if(reg == NULL) {
        mutex_unlock(&mutex);
        return NULL;
    }

    set_user_thread_regs(reg);

    /* Create the thread struct */
    thread_struct_t *thr = (thread_struct_t *)lmm_alloc_safe(&malloc_lmm, 
                            sizeof(thread_struct_t), 0);
    if(thr == NULL) {
        mutex_unlock(&mutex);
        return NULL;
    }
	/* Allocate space for thread's kernel stack */
	void *stack = lmm_alloc_page_safe(&malloc_lmm, LMM_ANY_REGION_FLAG);
    if(stack == NULL) {
        lmm_free_safe(&malloc_lmm, thr, sizeof(thread_struct_t));
        lmm_free_safe(&malloc_lmm, reg, sizeof(ureg_t));
        mutex_unlock(&mutex);
        return NULL;
    }
    thr->id = ++next_tid;
    mutex_unlock(&mutex);

    thr->regs = reg;
    thr->parent_task = task;
    thr->k_stack = stack + PAGE_SIZE;
    return thr;
}

/* --------------- Static local function ----------------*/
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

