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
static int initialized = 0;

void init_mutex() {
	if(initialized) {
        return;
	}
	mutex_init(&mutex);
	initialized = 1;
}

/* safe versions of malloc functions */
void *malloc(size_t size) {
    init_mutex();
    mutex_lock(&mutex);
    void *addr = _malloc(size);
    mutex_unlock(&mutex);
    return addr;
}

void *memalign(size_t alignment, size_t size) {
    init_mutex();
    mutex_lock(&mutex);
    void *addr = _memalign(alignment, size);
    mutex_unlock(&mutex);
    return addr;
}

void *calloc(size_t nelt, size_t eltsize) {
    init_mutex();
    mutex_lock(&mutex);
    void *addr = _calloc(nelt, eltsize);
    mutex_unlock(&mutex);
    return addr;
}

void *realloc(void *buf, size_t new_size) {
    init_mutex();
    mutex_lock(&mutex);
    void *addr = _realloc(buf, new_size);
    mutex_unlock(&mutex);
    return addr;
}

void free(void *buf) {
    init_mutex();
    mutex_lock(&mutex);
    _free(buf);
    mutex_unlock(&mutex);
}

void *smalloc(size_t size) {
    init_mutex();
    mutex_lock(&mutex);
    void *addr = _smalloc(size);
    mutex_unlock(&mutex);
    return addr;
}

void *smemalign(size_t alignment, size_t size) {
    init_mutex();
    mutex_lock(&mutex);
    void *addr = _smemalign(alignment, size);
    mutex_unlock(&mutex);
    return addr;
}

void sfree(void *buf, size_t size) {
    init_mutex();
    mutex_lock(&mutex);
    _sfree(buf, size);
    mutex_unlock(&mutex);
}
