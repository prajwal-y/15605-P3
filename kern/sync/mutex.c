/** @file mutex.c
 *  @brief Implementation of mutex calls
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <asm.h>
#include <sync/mutex.h>
#include <sync/mutex_asm.h>
#include <list/list.h>
#include <core/thread.h>
#include <core/scheduler.h>
#include <core/context.h>
#include <syscall.h>
#include <simics.h>
#include <common/assert.h>
#include <common/errors.h>

static int enable = 0;
static void disable_interrupts_mutex();
static void enable_interrupts_mutex();

/** @brief initialize a mutex
 *
 *  Set the mutex value to 1 indicating that it is unlocked
 *  and initialize the wait queue. Initializing a mutex after
 *  initializing it sets the value to 1 and "unlocks" it. Depending
 *  on if another thread holds the lock currently, this can lead to
 *  undefined behavior. 
 *
 *  @return 0 on success and -1 for invalid input
 */
int mutex_init(mutex_t *mp) {
    if (mp == NULL) {
        return ERR_INVAL;
    }
    mp->value = MUTEX_VALID;
	init_head(&mp->waiting);
    return 0;
}

/** @brief Function to allow enabling interrupts in mutex lock
 *  and unlock. This function is called before the bootstrap
 *  task is about to enter user space
 *
 *  @return void
 */
void enable_mutex_lib() {
	enable = 1;
}

/** @brief Wrapper for disabling interrupts.
 *
 *  Before the first task is loaded, there is only
 *  one thread running, and we do not disable interrupts.
 *
 *  @return void
 */
void disable_interrupts_mutex() {
	if(enable) {
		disable_interrupts();
	}
}

/** @brief Wrapper for enabling interrupts.
 *
 *  Before the first task is loaded, there is only
 *  one thread running, and we do not enable interrupts.
 *
 *  @return void
 */
void enable_interrupts_mutex() {
	if(enable) {
		enable_interrupts();
	}
}

/** @brief destroy a mutex
 *
 *  Sets the value of the mutex to -1 indicating that it is inactive.
 *  This function does not reclaim the space used by the mutex.
 *
 *  @return void
 */
void mutex_destroy(mutex_t *mp) {
	thread_assert(mp != NULL);
	thread_assert(mp->value != MUTEX_INVALID);
    mp->value = MUTEX_INVALID;
}

/** @brief attempt to acquire the lock
 *
 *  This function disables interrupts to check for the lock.
 *  If the lock is present, then the value of the lock is
 *  0 (lock is aquired), and the function returns after enabling 
 *  interrupts. Otherwise, the thread is added to waiting queue
 *  and context_switch() is called.
 *
 *  If the mutex is corrupted or destroyed, calling this function will result 
 *  in undefined behaviour
 *
 *  @return void
 */
void mutex_lock(mutex_t *mp) {
	thread_assert(mp != NULL);
	thread_assert(mp->value != MUTEX_INVALID);
	disable_interrupts_mutex();
	if(mp->value == 0) {
		thread_struct_t *curr_thread = get_curr_thread();
		curr_thread->status = WAITING;
		add_to_tail(&curr_thread->mutex_link, &mp->waiting);
		context_switch();
	}
	mp->value = 0;
	enable_interrupts_mutex();
}

/** @brief release a lock
 *
 *  When the lock is released, the lock is given to the first thread
 *  in the waiting queue (by making it runnable). If no threads are present, 
 *  the value of the lock is set to 1 (lock is available)
 *  
 *  If the mutex is corrupted or destroyed, calling this function will result 
 *  in undefined behaviour
 *
 *  @return void
 */
void mutex_unlock(mutex_t *mp) {
	thread_assert(mp != NULL);
	thread_assert(mp->value != MUTEX_INVALID);
    disable_interrupts_mutex();
	list_head *waiting_thread = get_first(&mp->waiting);
	if(waiting_thread != NULL) {
		thread_struct_t *thr = get_entry(waiting_thread, thread_struct_t,
                                          mutex_link);
		thr->status = RUNNABLE;
		runq_add_thread_interruptible(thr);
		del_entry(&thr->mutex_link);
	}
	mp->value = 1;
	enable_interrupts_mutex();
}
