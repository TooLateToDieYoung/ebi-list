#ifndef __EBI_LIST_H
#define __EBI_LIST_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>
#include "ebi-memory-pool.h"

typedef struct list_s list_s;

typedef void (* listFree_pf)(void *);

list_s * 
listMake(
    pool_s * const cpsPool,
    const listFree_pf cpfFree
);

void
listFree(
    void * pvThis
);

void *
listAccess(
    list_s * const cpsThis,
    const size_t czIdx
);

list_s *
listInsert(
    list_s * const cpsThis,
    const size_t czIdx,
    void * const cpvVal
);

list_s *
listRemove(
    list_s * const cpsThis,
    const size_t czIdx
);

list_s *
listChange(
    list_s * const cpsThis,
    const size_t czIdx,
    void * const cpvVal
);

list_s *
listInvert(
    list_s * const cpsThis
);

size_t
listLength(
    const list_s * const cpcsThis
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EBI_LIST_H */
