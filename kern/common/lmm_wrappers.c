#include <stddef.h>
#include <lmm.h>
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

void *lmm_alloc_safe(lmm_t *lmm, vm_size_t size, lmm_flags_t flags) {
    init_mutex();
    mutex_lock(&mutex);
    void *addr = lmm_alloc(lmm, size, flags);
    mutex_unlock(&mutex);
    return addr;
}

void *lmm_alloc_page_safe(lmm_t *lmm, lmm_flags_t flags) {
	init_mutex();
    mutex_lock(&mutex);
    void *addr = lmm_alloc_page(lmm, flags);
    mutex_unlock(&mutex);
    return addr;
}

void lmm_free_safe(lmm_t *lmm, void *block, vm_size_t size) {
	init_mutex();
    mutex_lock(&mutex);
    lmm_free(lmm, block, size);
    mutex_unlock(&mutex);
}

void lmm_free_page_safe(lmm_t *lmm, void *block) {
	init_mutex();
    mutex_lock(&mutex);
    lmm_free_page(lmm, block);
    mutex_unlock(&mutex);
}
