/** @file frame_allocator.h
 *  @brief functions to manage the physical frames in the system
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __FRAME_ALLOCATOR_H
#define __FRAME_ALLOCATOR_H

#define FREE_FRAMES_COUNT (machine_phys_frames())-(USER_MEM_START / PAGE_SIZE)
#define FRAME_INDEX(addr) (((unsigned int)addr)-(USER_MEM_START))/(PAGE_SIZE)

void init_frame_allocator();

void *allocate_frame();

void deallocate_frame(void *frame_addr);

void check_physical_memory();

#endif /* __FRAME_ALLOCATOR_H */
