/** @file frame_allocator.h
 *  @brief functions to manage the physical frames in the system
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __FRAME_ALLOCATOR_H
#define __FRAME_ALLOCATOR_H

void init_frame_allocator();

void *allocate_frame();

void deallocate_frame(void *frame_addr);

#endif /* __FRAME_ALLOCATOR_H */
