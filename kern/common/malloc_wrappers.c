/** @file malloc_wrappers.c
 *
 *  Thread safe wrappers for malloc functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <stddef.h>
#include <malloc_internal.h>
#include <sync/sem.h>

static sem_t sem;

/** @brief Initializes the thread safe malloc library
 *
 *  @return void
 */
void init_thr_safe_malloc_lib() {
	sem_init(&sem, 1);
}

/** @brief Thread safe function for malloc
 *
 *  @param size Size of the memory to be allocated
 *
 *  @return void * Address of the memory allocated
 */
void *malloc(size_t size) {
    sem_wait(&sem);
    void *addr = _malloc(size);
    sem_signal(&sem);
    return addr;
}

/** @brief Thread safe version of smemalign
 *
 *  @param alignment alignment of the memory to be allocated
 *  @size size of the buffer to be allocated
 *
 *  @return void * Address of the buffer allocated
 */
void *memalign(size_t alignment, size_t size) {
    sem_wait(&sem);
    void *addr = _memalign(alignment, size);
    sem_signal(&sem);
    return addr;
}

/** @brief Thread safe function for calloc
 *
 *  @param nelt
 *  @param eltsize
 *
 *  @return void * Address of the memory allocated
 */
void *calloc(size_t nelt, size_t eltsize) {
    sem_wait(&sem);
    void *addr = _calloc(nelt, eltsize);
    sem_signal(&sem);
    return addr;
}

/** @brief Thread safe function for realloc
 *
 *  @param buf Address of the original buffer
 *  @param new_size New size for the buffer
 *
 *  @return void * Address of the memory allocated
 */
void *realloc(void *buf, size_t new_size) {
    sem_wait(&sem);
    void *addr = _realloc(buf, new_size);
    sem_signal(&sem);
    return addr;
}

/** @brief Thread safe function for free
 *
 *  @param buf Buffer to be free'd
 *
 *  @return void
 */
void free(void *buf) {
    sem_wait(&sem);
    _free(buf);
    sem_signal(&sem);
}

/** @brief Thread safe function for smalloc
 *
 *  @param size Size of the memory to be allocated
 *
 *  @return void * Address of the memory allocated
 */
void *smalloc(size_t size) {
    sem_wait(&sem);
    void *addr = _smalloc(size);
    sem_signal(&sem);
    return addr;
}

/** @brief Thread safe version of smemalign
 *
 *  @param alignment alignment of the memory to be allocated
 *  @size size of the buffer to be allocated
 *
 *  @return void * Address of the buffer allocated
 */
void *smemalign(size_t alignment, size_t size) {
    sem_wait(&sem);
    void *addr = _smemalign(alignment, size);
    sem_signal(&sem);
    return addr;
}

/** @brief Thread safe version of sfree()
 *
 *  @param buf Buffer to be free'd
 *  @param size Size of the buffer
 *
 *  @return void
 */
void sfree(void *buf, size_t size) {
    sem_wait(&sem);
    _sfree(buf, size);
    sem_signal(&sem);
}
