/** @file frame_allocator.c
 *  @brief implement the frame_allocator functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#include <allocator/frame_allocator.h>
#include <common_kern.h>
#include <limits.h>
#include <simics.h>
#include <sync/mutex.h>
#include <page.h>

#define FREE_FRAME_LIST_END UINT_MAX

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
    int total_frames = machine_phys_frames();
    /* Start from byte 0 of the physical memory */
    void *frame_ptr = (void *) 0;
    int i;

    /* Set "header" in each frame to point to the next free frame */
    for (i = 0; i < total_frames - 1; i++) {
        lprintf("is is %d", i);
        *(int *)frame_ptr = (i + 1) * PAGE_SIZE;
        frame_ptr = (char *)frame_ptr + PAGE_SIZE;
    }
    *(int *)frame_ptr = FREE_FRAME_LIST_END;
}

/** @brief get a free physical frame
 *
 *  This function locks the frame list (which functions as a stack)
 *  and returns the top of the frame list. If there are no more free frames 
 *  the function returns 0xffffffff.
 *
 *  @return void * physical address of the free frame
 */
void *allocate_frame() {
    mutex_lock(&list_mut) ;
    if ((int)free_list_head == FREE_FRAME_LIST_END) {
        mutex_unlock(&list_mut);
        return (void *)FREE_FRAME_LIST_END;
    }   
    void *frame_addr = free_list_head;
    free_list_head = (void *)(*(int *)free_list_head);
    mutex_unlock(&list_mut);
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
    mutex_lock(&list_mut) ;
    *(int *)frame_addr = (int)free_list_head;
    free_list_head = frame_addr;
    mutex_unlock(&list_mut);
    return;
}

