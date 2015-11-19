/** @file malloc_wrappers.c
 *
 *  Thread safe wrappers for malloc functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */

#include <stddef.h>
#include <malloc_internal.h>
#include <sync/mutex.h>

static mutex_t mutex;

/** @brief Initializes the thread safe malloc library
 *
 *  @return void
 */
void init_thr_safe_malloc_lib() {
	mutex_init(&mutex);
}

/** @brief Thread safe function for malloc
 *
 *  @param size Size of the memory to be allocated
 *
 *  @return void * Address of the memory allocated
 */
void *malloc(size_t size) {
	mutex_lock(&mutex);
    void *addr = _malloc(size);
	mutex_unlock(&mutex);
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
	mutex_lock(&mutex);
    void *addr = _memalign(alignment, size);
	mutex_unlock(&mutex);
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
	mutex_lock(&mutex);
    void *addr = _calloc(nelt, eltsize);
	mutex_unlock(&mutex);
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
	mutex_lock(&mutex);
    void *addr = _realloc(buf, new_size);
	mutex_unlock(&mutex);
    return addr;
}

/** @brief Thread safe function for free
 *
 *  @param buf Buffer to be free'd
 *
 *  @return void
 */
void free(void *buf) {
	mutex_lock(&mutex);
    _free(buf);
	mutex_unlock(&mutex);
}

/** @brief Thread safe function for smalloc
 *
 *  @param size Size of the memory to be allocated
 *
 *  @return void * Address of the memory allocated
 */
void *smalloc(size_t size) {
	mutex_lock(&mutex);
    void *addr = _smalloc(size);
	mutex_unlock(&mutex);
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
	mutex_lock(&mutex);
    void *addr = _smemalign(alignment, size);
	mutex_unlock(&mutex);
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
	mutex_lock(&mutex);
    _sfree(buf, size);
	mutex_unlock(&mutex);
}
