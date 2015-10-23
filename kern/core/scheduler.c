/** @file scheduler.c
 *  @brief implementation of scheduler functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <core/thread.h>
#include <list/list.h>
#include <core/scheduler.h>
#include <sync/mutex.h>

static thread_struct_t *curr_thread; /* The thread currently being run */

static list_head runnable_threads;    /* List of runnable threads */
static mutex_t runq_mutex;

static thread_struct_t *runq_get_head();

/** @brief initialize the scheduler data structures
 *
 *  @return void
 */
void init_scheduler() {
    mutex_init(&runq_mutex);
}

/** @brief return the next thread to be run
 *
 *  Implements a round robin scheduling strategy. Returns the next thread
 *  to be run. This will be invoked by context switching code ONLY.
 *
 *  @return thread_struct_t a struct containing scheduling information 
 *                          for the next thread
 */
thread_struct_t *next_thread() {
    /* Get the thread at the head of the runqueue */
    thread_struct_t *head = runq_get_head();
    if (head == NULL) {
        return NULL;
    }

    /* Add the current thread to the end of the run queue */
    if (curr_thread->id != 3) {
        runq_add_thread(curr_thread);
    }

    /* Set new curr_thread (currently running thread) */
    curr_thread = head;

    return head;
}

thread_struct_t *runq_get_head() {
    mutex_lock(&runq_mutex);
    list_head *head = get_first(&runnable_threads);
    if (head == NULL) {
        mutex_unlock(&runq_mutex);
        return NULL;
    }
    del_entry(head);
    mutex_unlock(&runq_mutex);
    thread_struct_t *head_thread = get_entry(head, thread_struct_t, runq_link);
    return head_thread;
}

void runq_add_thread(thread_struct_t *thr) {
    mutex_lock(&runq_mutex);
    add_to_tail(&thr->runq_link, &runnable_threads);
    mutex_unlock(&runq_mutex);
}

/** @brief get the currently running thread
 *
 *  @return thread_struct_t thread info of the currently running thread
 */
thread_struct_t *get_curr_thread() {
    return curr_thread;
}

void set_running_thread(thread_struct_t *thr) {
    curr_thread = thr;
}
