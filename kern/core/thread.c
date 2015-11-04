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
#include <list/list.h>
#include <common/assert.h>

#define HASHMAP_SIZE (PAGE_SIZE * 2)

static int next_tid;
static mutex_t mutex;
static mutex_t map_mutex;
static list_head *thread_map[HASHMAP_SIZE];

static void set_user_thread_regs(ureg_t *reg);
static void init_thread_map();
static void add_thread_to_map(thread_struct_t *thr);

/** @brief Initializes the thread creation module
 *
 *  @return Void
 */
void kernel_threads_init() {
    next_tid = 0;
    mutex_init(&mutex);
    init_thread_map();
}

/** @brief create a new thread.
 *
 *  Create thread and add mapping in the hash map
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
    /* Assign thread id to thread and add it to task's thread list*/
    mutex_lock(&mutex);
    thr->id = ++next_tid;
    add_to_tail(&thr->task_thread_link, &task->thread_head);
    mutex_unlock(&mutex);

    /* Add thread tCB to hash map */
    add_thread_to_map(thr);

	/* Allocate space for thread's kernel stack */
	void *stack = smemalign(PAGE_SIZE, KERNEL_STACK_SIZE);
    if(stack == NULL) {
        sfree(thr, sizeof(thread_struct_t));
        sfree(reg, sizeof(ureg_t));
        return NULL;
    }

    thr->regs = reg;
    thr->parent_task = task;
    thr->k_stack = stack;
	thr->k_stack_base = (uint32_t)((char *)stack + PAGE_SIZE);
	thr->cur_esp = thr->k_stack_base;
	thr->cur_ebp = thr->k_stack_base;
	thr->status = RUNNABLE; /* Default value */
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

/** @brief Initialize the buckets of the hash map
 *
 *  Hash map mapping thread id to tCB. The super complicated hash function
 *  used is thread_id % HASHMAP_SIZE. Each bucket is a linked list to handle
 *  collisions. Since thread IDs are in increasing order we will not be having
 *  any collision till the first 2048 threads have been created.
 *  
 *  @return void
 */
void init_thread_map() {
    int i;
    for (i = 0; i < HASHMAP_SIZE; i++) {
        thread_map[i] = (list_head *)smalloc(sizeof(list_head));
        kernel_assert(thread_map[i] != NULL);
        init_head(thread_map[i]);
    }
    mutex_init(&map_mutex);
}

/** @brief add a thread to the hashmap
 *
 *  calculate the index of the thread using thr->id % HASHMAP_SIZE
 *  Add it to tail of the bucket
 *
 *  @param thr thread_struct_t of the thread to be added
 *  @return void
 */
void add_thread_to_map(thread_struct_t *thr) {
    int index = thr->id % HASHMAP_SIZE;
    mutex_lock(&map_mutex);
    add_to_tail(&thr->thread_map_link, thread_map[index]);
    mutex_unlock(&map_mutex);
}

/** @brief return thread struct for a given thread id
 *
 *  @param thr_id thread id of thread struct we wih to retrieve
 *  @return thread_struct_t* thread struct corresponding to the thread id
 *                            null if not found
 */
thread_struct_t *get_thread_from_id(int thr_id) {
    int index = thr_id % HASHMAP_SIZE;
    list_head *bucket_head = thread_map[index];
    mutex_lock(&map_mutex);
    list_head *thr_node = get_first(bucket_head);
	while(thr_node != NULL && thr_node != bucket_head) {
        thread_struct_t *thr = get_entry(thr_node, thread_struct_t, 
                                          thread_map_link);
        if (thr->id == thr_id) {
            mutex_unlock(&map_mutex);
            return thr;
        }
		thr_node = thr_node->next;
	}
    mutex_unlock(&map_mutex);
    return NULL;
}

/** @brief remove thread struct from hashmap for given thread id
 *
 *  @param thr_id thread id of thread to be removed from thread map
 *  @return void
 */
void remove_thread_from_map(int thr_id) {
    int index = thr_id % HASHMAP_SIZE;
    list_head *bucket_head = thread_map[index];
    mutex_lock(&map_mutex);
    list_head *thr_node = get_first(bucket_head);
	while(thr_node != NULL && thr_node != bucket_head) {
        thread_struct_t *thr = get_entry(thr_node, thread_struct_t, 
                                          thread_map_link);
        if (thr->id == thr_id) {
            del_entry(thr_node);
            mutex_unlock(&map_mutex);
            return;
        }
		thr_node = thr_node->next;
	}
    mutex_unlock(&map_mutex);
}
