/** @file scheduler.c
 *  @brief implementation of scheduler functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <asm.h>
#include <core/thread.h>
#include <list/list.h>
#include <core/scheduler.h>
#include <sync/mutex.h>
#include <simics.h>

static thread_struct_t *curr_thread; /* The thread currently being run */

static list_head runnable_threads;    /* List of runnable threads */

static thread_struct_t *runq_get_head();
//static void print_runnable_list(); //TODO: REMOVE THIS

/** @brief initialize the scheduler data structures
 *
 *  @return void
 */
void init_scheduler() {
	init_head(&runnable_threads);
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

    return head;
}

thread_struct_t *runq_get_head() {
    list_head *head = get_first(&runnable_threads);
    if (head == NULL) {
        return NULL;
    }
    del_entry(head);
    thread_struct_t *head_thread = get_entry(head, thread_struct_t, runq_link);
    return head_thread;
}

void runq_add_thread(thread_struct_t *thr) {
    disable_interrupts();
    add_to_tail(&thr->runq_link, &runnable_threads);
    enable_interrupts();
}

void runq_add_thread_interruptible(thread_struct_t *thr) {
    add_to_tail(&thr->runq_link, &runnable_threads);
}

/** @brief get the currently running thread
 *
 *  @return thread_struct_t thread info of the currently running thread
 */
thread_struct_t *get_curr_thread() {
    return curr_thread;
}

/** @brief get the currently running task
 *
 *  @return task_struct_t Task info of the current task
 */
task_struct_t *get_curr_task() {
	return curr_thread->parent_task;
}

/** @brief Set the currently running thread
 *
 *  @param thr The thread struct for the thread to be set
 *  as the currently running thread
 *
 *  @return void
 */
void set_running_thread(thread_struct_t *thr) {
    curr_thread = thr;
}

void print_runnable_list() {
	list_head *temp = get_first(&runnable_threads);
	while(temp != NULL && temp != &runnable_threads) {
		thread_struct_t *thr = get_entry(temp, thread_struct_t, runq_link);
		lprintf("-------Thread %d-------", thr->id);
		temp = temp->next;
	}
	lprintf("--------End of runnable threads-------");
}
