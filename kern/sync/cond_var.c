/** @file cond_var.c
 *
 *  @brief Implementation of sort of something like 
 *  condition variables, specifically designed for 
 *  waiting threads.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <sync/cond_var.h>
#include <asm.h>
#include <list/list.h>
#include <common/errors.h>
#include <common/assert.h>
#include <sync/mutex.h>
#include <core/thread.h>
#include <simics.h>
#include <core/context.h>
#include <core/scheduler.h>

/** @brief initialize a cond var
 *
 *  Set status of cond var to 1. It "initializes" the mutex pointed to 
 *  by cv. This also initializes the head of queue of waiting threads.
 *  Calling this function on an already initialized function can lead to
 *  undefined behavior.
 *
 *  @param cv a pointer to the condition variable
 *
 *  @return 0 on success and ERR_INVAL for invalid input
 */
int cond_init(cond_t *cv) {
    if (cv == NULL) {
        return ERR_INVAL;
    }
    cv->status = COND_VAR_VALID;
    if (mutex_init(&cv->queue_mutex) < 0) {
        return ERR_INVAL;
    }
    init_head(&cv->waiting);
    return 0;
}

/** @brief destroy a cond var
 *
 *  Sets the value of the status to 0 indicating that it is inactive.
 *  This function does not reclaim the space used by the cond var.
 *  Once a cond var is destroyed calling cond var functions can lead to
 *  undefined behavior.
 *
 *  @param cv a pointer to the condition variable
 *  @return void
 */
void cond_destroy(cond_t *cv) {
	thread_assert(cv != NULL);
    mutex_destroy(&cv->queue_mutex);
    cv->status = COND_VAR_INVALID;
}

/** @brief This function allows a thread to sleep on a signal issued on 
 *         some condition
 *
 *  This function adds thread to the queue of blocked threads and deschedules
 *  itself after unlocking the mutex associated with the cond var. When the 
 *  thread is woken up the thread is removed from the queue and the mutex is 
 *  locked before returning.
 *
 *  @pre the mutex pointed to by mp must be locked
 *  @post the mutex pointed to by mp is locked
 *
 *  @param cv a pointer to the condition variable
 *  @param mp a pointer to the mutex associated with the thread
 *  @param link a pointer to the "list_head" of the waiting list on this cv
 *  @return void
 */
void cond_wait(cond_t *cv, mutex_t *mp, list_head *link) { 
	thread_assert(cv != NULL);
	thread_assert(cv->status != COND_VAR_INVALID);

    /* Protect accesses to the queue */
    mutex_lock(&cv->queue_mutex);
    add_to_tail(link, &cv->waiting);
    mutex_unlock(&cv->queue_mutex);

	disable_interrupts();
	thread_struct_t *curr_thread = get_curr_thread();
	curr_thread->status = WAITING;
	mutex_unlock(mp);
	enable_interrupts();

	/* Release the mutex and call context switch */
	context_switch();

	/* Acquire the mutex again before returning */
	mutex_lock(mp);
}


/** @brief this function signals an event and wakes up a waiting thread
 *         if present
 *
 *  This function is called to signal a change in the state. This wakes up 
 *  any thread that may be waiting for the change in state. Between the change
 *  of state and the awakened thread getting the lock on the state, the state 
 *  may have changed to an invalid state again. In this case it is the
 *  responsibility of the programmer to check for the condition after being
 *  woken up. This function MUST be called while holding the mutex protecting
 *  the state. Calling cond_signal without holding the mutex can lead to 
 *  undefined behavior.
 *
 *  @pre the calling thread must hold the mutex
 *  @param cv a pointer to the condition variable
 *  @param link The "list_head" of the waiting list on this cv
 *  @return void
 */
void cond_signal(cond_t *cv) {
	thread_assert(cv != NULL);
	thread_assert(cv->status != COND_VAR_INVALID);

	mutex_lock(&cv->queue_mutex); 
    list_head *waiting_thread = get_first(&cv->waiting);
    if (waiting_thread != NULL) {
        thread_struct_t *thr = get_entry(waiting_thread, thread_struct_t, 
                                          cond_wait_link);
		thr->status = RUNNABLE;
		runq_add_thread(thr);
		del_entry(&thr->cond_wait_link);
    }
    mutex_unlock(&cv->queue_mutex);
}

/** @brief this function signals all threads waiting on this cond var
 *         
 *  This function MUST be called while holding the mutex to the shared state
 *  without which the behavior is undefined. Any thread which calls cond_wait
 *  after cond_broadcast has been called will not be signalled.
 *
 *  @pre the calling thread must hold the mutex
 *  @param cv a pointer to the condition variable
 *  @param link The "list_head" of the waiting list on this cv
 *  @return void
 */
void cond_broadcast(cond_t *cv) {
	thread_assert(cv != NULL);
	thread_assert(cv->status != COND_VAR_INVALID);
	
	mutex_lock(&cv->queue_mutex);
    list_head *waiting_thread = get_first(&cv->waiting);
	while(waiting_thread != NULL && waiting_thread != &cv->waiting) {
        thread_struct_t *thr = get_entry(waiting_thread, thread_struct_t, 
                                          cond_wait_link);
		thr->status = RUNNABLE;
		runq_add_thread(thr);
		waiting_thread = waiting_thread->next;
		del_entry(&thr->cond_wait_link);
	}
    mutex_unlock(&cv->queue_mutex);
}
