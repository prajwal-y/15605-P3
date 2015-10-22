/** @file lmm_wrappers.h
 *  @brief thread safe versions of lmm functions
 *
 *  @author Rohit Upadhyaya (rjupadhy)
 *  @author Prajwal Yadapadithaya (pyadapad)
 */
#ifndef __LMM_WRAPPERS_H
#define __LMM_WRAPPERS_H
#include <lmm.h>

void *lmm_alloc_safe(lmm_t *lmm, vm_size_t size, lmm_flags_t flags);

void *lmm_alloc_page_safe(lmm_t *lmm, lmm_flags_t flags);

void lmm_free_safe(lmm_t *lmm, void *block, vm_size_t size);

void lmm_free_page_safe(lmm_t *lmm, void *block);

#endif  /* __LMM_WRAPPERS_H */
