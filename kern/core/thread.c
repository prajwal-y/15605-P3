/** @file thread.c
 *  @brief thread manipulation routines
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <thread.h>
#include <lmm.h>
#include <vm/vm.h>
#include <loader/loader.h>
#include <common/thread_struct.h>
#include <sync/mutex.h>

static int next_tid;
static mutex_t mutex;

/** @brief Initializes the thread creation module
 *
 *  @return Void
 */
void thread_init() {
    next_tid = 0;
    mutex_init(&mutex);
}

/** @brief create a new thread.
 *
 *  @param task the task under which this thread will run
 *  @param regs the register values this thread must take
 *  @return thread_struct_t the newly created thread.
 */
thread_struct_t *create_thread(task_struct_t *task, ureg_t *regs) {
    if(task == NULL) {
        return;
    }
    mutex_lock(&mutex);
    thread_struct_t *thr = (thread_struct_t *)lmm_alloc(malloc_lmm, 
                            sizeof(thread_struct_t), 0);
    if(thr == NULL) {
        mutex_unlock(&mutex);
        return;
    }
    thr->id = ++next_tid;
    thr->regs = regs;
    thr->parent_task = task;
    mutex_unlock(&mutex);
    return thr;
}

