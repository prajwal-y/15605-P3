/** @file mutex_type.h
 *  @brief This file defines the type for mutexes.
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

#define MUTEX_VALID 1
#define MUTEX_INVALID 0

typedef struct mutex {
    int value;          /* Will be 0 or 1 */
} mutex_t;

#endif /* _MUTEX_TYPE_H */
