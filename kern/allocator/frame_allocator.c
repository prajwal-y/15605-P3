/** @file frame_allocator.c
 *  @brief implement the frame_allocator functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <allocator/frame_allocator.h>
#include <common/malloc_wrappers.h>
#include <common_kern.h>
#include <limits.h>
#include <simics.h>
#include <sync/mutex.h>
#include <page.h>
#include <stddef.h>
#include <common/assert.h>
#include <sync/mutex_asm.h>

#define FRAME_LOCK 1

#define FREE_FRAME_LIST_END UINT_MAX
#define PAGE_ALIGNMENT_CHECK 0x00000fff
#define GET_FRAME_LOCK(x) ((x) & 1)

static int *free_frames_arr;
static mutex_t *free_frames_lock;

static void *free_list_head; /* Head of free list,UINT_MAX => no free frames */
static mutex_t list_mut;     /* Mutex to synchronize access to free frame list */

static void init_free_list();

/** @brief initialize the free frame allocator
 *
 *  initialize the free list and also the mutex to synchronize access to the 
 *  free frame list.
 *
 *  @return void
 */
void init_frame_allocator() {
    /* create the free list of frames */
    init_free_list();

    mutex_init(&list_mut);
}

/** @brief initialize the free frame list on system startup
 *
 *  Each free frame has a header indicating the address of the next free
 *  frame. The header is very simplistic and just contains the address
 *  of the next free frame. The head of the free frame list is maintained 
 *  in free_list_head.
 *
 *  @param frame_addr the address of the frame to be freed.
 *  @return void
 */
void init_free_list() {

	/*Initialize the free list array*/
	free_frames_arr = (int *)smalloc(FREE_FRAMES_COUNT*sizeof(int));
	kernel_assert(free_frames_arr != NULL);
	free_frames_lock = (mutex_t *)smalloc(FREE_FRAMES_COUNT*sizeof(mutex_t));
	kernel_assert(free_frames_lock != NULL);

	int i;
	for(i = 0; i < FREE_FRAMES_COUNT - 1; i++) {
		free_frames_arr[i] = (USER_MEM_START + ((i+1) * PAGE_SIZE));
		mutex_init(&free_frames_lock[i]);
	}
	free_frames_arr[FREE_FRAMES_COUNT - 1] = FREE_FRAME_LIST_END;
	mutex_init(&free_frames_lock[FREE_FRAMES_COUNT - 1]);

	free_list_head = (void *)USER_MEM_START;
}

/** @brief get a free physical frame
 *
 *  This function locks the frame list (which functions as a stack)
 *  and returns the top of the frame list. If there are no more free frames 
 *  the function returns null.
 *
 *  @return void * physical address of the free frame
 */
void *allocate_frame() {
    mutex_lock(&list_mut);
	if ((int)free_list_head == FREE_FRAME_LIST_END) {
        mutex_unlock(&list_mut);
        return NULL;
    }
    void *frame_addr = free_list_head; 
	free_list_head = (void *)free_frames_arr[FRAME_INDEX(frame_addr)];
	mutex_unlock(&list_mut);

    kernel_assert(((int)frame_addr & PAGE_ALIGNMENT_CHECK) == 0);

    return frame_addr;
}

/** @brief return a physical frame to the free frame list
 *
 *  This function locks the frame list (which functions as a stack)
 *  adds a frame back to the top of the frame list.
 *
 *  @param frame_addr the address of the frame to be freed.
 *  @return void
 */
void deallocate_frame(void *frame_addr) {
    kernel_assert(((int)frame_addr & PAGE_ALIGNMENT_CHECK) == 0);
    kernel_assert(frame_addr != NULL);

    mutex_lock(&list_mut);
	int index = FRAME_INDEX(frame_addr);
	free_frames_arr[index] = (unsigned int)free_list_head;
	free_list_head = frame_addr;
    mutex_unlock(&list_mut);
}

/** @brief Function to lock a particular physical frame
 *
 *  This function uses the last bit in the free_frames_arr to
 *  obtain a lock on the frame_addr if the refernce count needs
 *  to be modified
 *
 *  @param frame_addr The address of the frame that must be locked
 *
 *  @return void
 */
void lock_frame(void *frame_addr) {
	kernel_assert(frame_addr != NULL);
	mutex_lock(&free_frames_lock[FRAME_INDEX(frame_addr)]);
}

/** @brief Function to unlock a particular physical frame
 *
 *  This function uses the last bit in the free_frames_arr to
 *  obtain a lock on the frame_addr if the refernce count needs
 *  to be modified
 *
 *  @param frame_addr The address of the frame that must be unlocked
 *
 *  @return void
 */
void unlock_frame(void *frame_addr) {
	kernel_assert(frame_addr != NULL);
	mutex_unlock(&free_frames_lock[FRAME_INDEX(frame_addr)]);
}

/** @brief Function used to check the physical frames status
 *
 *  Used for debugging
 * 
 *  @return int Number of free physical frames
 */
int check_physical_memory() {
    int free_count = 0;
    void *first = free_list_head;

    while ((int)first != FREE_FRAME_LIST_END) {
		kernel_assert((unsigned int)first >= USER_MEM_START);
		kernel_assert((unsigned int)first <= FREE_FRAME_LIST_END);
	    first = (void *)(free_frames_arr[FRAME_INDEX(first)]);
        free_count++;
    }
    lprintf("Total free physical frames: %d, next free frame %p",free_count, free_list_head);
    return free_count;	
}

