#include "ebi-list.h"
#include <assert.h>

typedef struct {
    void * pvXor;
    void * pvVal;
} node_s;

struct list_s {
    pool_s * psPool;
    listFree_pf pfFree;

    node_s * psHead;
    node_s * psTail;
    size_t zLength;

    node_s * psPrev;
    node_s * psCurr;
    node_s * psNext;
    size_t zRecord;
};

list_s * 
listMake(
    pool_s * const cpsPool,
    const listFree_pf cpfFree
) {
assert( NULL != cpfFree );

    list_s * const cpsThis = ( NULL != cpsPool ) ? 
        (list_s *)poolMalloc(cpsPool, sizeof(list_s)) :
        (list_s *)malloc(sizeof(list_s)) ;

    if ( NULL != cpsThis )
    {
        cpsThis->psPool = cpsPool;
        cpsThis->pfFree = cpfFree;
        cpsThis->psHead = NULL;
        cpsThis->psTail = NULL;
        cpsThis->zLength = 0;
        cpsThis->psPrev = NULL;
        cpsThis->psCurr = NULL;
        cpsThis->psNext = NULL;
        cpsThis->zRecord = 0;
    }

    return cpsThis;
}

void
listFree(
    void * pvThis
) {
    list_s * const cpsThis = (list_s *)( pvThis );

    while ( listLength(cpsThis) > 0 )
    {
        (void)listRemove(cpsThis, 0);
    }

    if ( NULL != cpsThis->psPool )
    {
        poolReturn(cpsThis);
    }
    else
    {
        free(cpsThis);
    }
}

void *
listAccess(
    list_s * const cpsThis,
    const size_t czIdx
) {
    if ( listLength(cpsThis) <= czIdx )
    {
        return NULL;
    }

    if ( 0 == czIdx )
    {
        cpsThis->psPrev = NULL;
        cpsThis->psCurr = cpsThis->psHead;
        cpsThis->psNext = cpsThis->psCurr->pvXor;
        cpsThis->zRecord = czIdx;
    }
    else if ( czIdx == listLength(cpsThis) - 1 )
    {
        cpsThis->psPrev = cpsThis->psCurr->pvXor;
        cpsThis->psCurr = cpsThis->psTail;
        cpsThis->psNext = NULL;
        cpsThis->zRecord = czIdx;
    }
    else
    {
        for ( ; cpsThis->zRecord < czIdx; cpsThis->zRecord++ ) 
        {
            cpsThis->psPrev = cpsThis->psCurr;
            cpsThis->psCurr = cpsThis->psNext;
            cpsThis->psNext = (node_s *)( (size_t)( cpsThis->psNext->pvXor ) ^ (size_t)( cpsThis->psPrev ) );
        }

        for ( ; cpsThis->zRecord > czIdx; cpsThis->zRecord-- ) 
        {
            cpsThis->psNext = cpsThis->psCurr;
            cpsThis->psCurr = cpsThis->psPrev;
            cpsThis->psPrev = (node_s *)( (size_t)( cpsThis->psPrev->pvXor ) ^ (size_t)( cpsThis->psNext ) );
        }
    }

    return cpsThis->psCurr->pvVal;
}

list_s *
listInsert(
    list_s * const cpsThis,
    const size_t czIdx,
    void * const cpvVal
) {
    node_s * psNode = NULL;

    if ( NULL == cpsThis )
    {
        return NULL;
    }

    psNode = ( NULL != cpsThis->psPool ) ? 
        (node_s *)poolMalloc(cpsThis->psPool, sizeof(node_s)) :
        (node_s *)malloc(sizeof(node_s)) ;

    if ( NULL == psNode )
    {
        return NULL;
    }
    else
    {
        psNode->pvVal = cpvVal;
    }

    if ( 0 == czIdx )
    {
        psNode->pvXor = cpsThis->psHead;
        cpsThis->psHead->pvXor = (node_s *)( (size_t)( cpsThis->psHead->pvXor ) ^ (size_t)( psNode ) );
        cpsThis->psPrev = NULL;
        cpsThis->psCurr = psNode;
        cpsThis->psNext = cpsThis->psHead;
        cpsThis->psHead = psNode;
        cpsThis->zRecord = 0;

    }
    else if ( listLength(cpsThis) <= czIdx )
    {
        psNode->pvXor = cpsThis->psTail;
        cpsThis->psTail->pvXor = (node_s *)( (size_t)( cpsThis->psTail->pvXor ) ^ (size_t)( psNode ) );
        cpsThis->psPrev = cpsThis->psTail;
        cpsThis->psCurr = psNode;
        cpsThis->psNext = NULL;
        cpsThis->psTail = psNode;
        cpsThis->zRecord = listLength(cpsThis);
    }
    else
    {
        (void)listAccess(cpsThis, czIdx);

        psNode->pvXor = (node_s *)( (size_t)( cpsThis->psPrev ) ^ (size_t)( cpsThis->psCurr ) );
        cpsThis->psPrev->pvXor = (void *)( (size_t)( cpsThis->psPrev->pvXor ) ^ (size_t)( cpsThis->psCurr ) ^ (size_t)( psNode ) );
        cpsThis->psCurr->pvXor = (void *)( (size_t)( cpsThis->psCurr->pvXor ) ^ (size_t)( cpsThis->psPrev ) ^ (size_t)( psNode ) );
        cpsThis->psNext = cpsThis->psCurr;
        cpsThis->psCurr = psNode;
        // cpsThis->zRecord = cpsThis->zRecord;
    }

    cpsThis->zLength++;
    return cpsThis;
}

list_s *
listRemove(
    list_s * const cpsThis,
    const size_t czIdx
) {
    node_s * psDrop = NULL;

    if ( listLength(cpsThis) > czIdx )
    {
        (void)listAccess(cpsThis, czIdx);

        psDrop = cpsThis->psCurr;

        if ( cpsThis->psCurr == cpsThis->psHead )
        {
            cpsThis->psNext = (node_s *)( (size_t)( cpsThis->psNext->pvXor ) ^ (size_t)( cpsThis->psCurr ) );
            cpsThis->psCurr = cpsThis->psCurr->pvXor;
            cpsThis->psCurr->pvXor = (void *)( (size_t)( cpsThis->psCurr->pvXor ) ^ (size_t)( cpsThis->psHead ) );
            cpsThis->psHead = cpsThis->psCurr;
            // cpsThis->zRecord = 0;
        }
        else if ( cpsThis->psCurr == cpsThis->psTail )
        {
            cpsThis->psPrev = (node_s *)( (size_t)( cpsThis->psPrev->pvXor ) ^ (size_t)( cpsThis->psCurr ) );
            cpsThis->psCurr = cpsThis->psCurr->pvXor;
            cpsThis->psCurr->pvXor = (void *)( (size_t)( cpsThis->psCurr->pvXor ) ^ (size_t)( cpsThis->psTail ) );
            cpsThis->psTail = cpsThis->psCurr;
            cpsThis->zRecord--;
        }
        else
        {
            cpsThis->psPrev->pvXor = (void *)( (size_t)( cpsThis->psPrev->pvXor ) ^ (size_t)( cpsThis->psCurr ) ^ (size_t)( cpsThis->psNext ) );
            cpsThis->psNext->pvXor = (void *)( (size_t)( cpsThis->psNext->pvXor ) ^ (size_t)( cpsThis->psCurr ) ^ (size_t)( cpsThis->psPrev ) );
            cpsThis->psCurr = cpsThis->psNext;
            cpsThis->psNext = (node_s *)( (size_t)( cpsThis->psNext->pvXor ) ^ (size_t)( cpsThis->psPrev ) );
            // cpsThis->zRecord = cpsThis->zRecord;
        }

        cpsThis->pfFree(psDrop->pvVal);
        if ( NULL != cpsThis->psPool )
        {
            poolReturn(psDrop);
        }
        else
        {
            free(psDrop);
        }
        cpsThis->zLength--;
    }

    return cpsThis;
}

list_s *
listChange(
    list_s * const cpsThis,
    const size_t czIdx,
    void * const cpvVal
) {
    if ( listLength(cpsThis) <= czIdx )
    {
        return listInsert(cpsThis, czIdx, cpvVal);
    }

    (void)listAccess(cpsThis, czIdx);

    cpsThis->pfFree(cpsThis->psCurr->pvVal);
    cpsThis->psCurr->pvVal = cpvVal;

    return cpsThis;
}

list_s *
listInvert(
    list_s * const cpsThis
) {
    if ( NULL != cpsThis )
    {
        cpsThis->psCurr = cpsThis->psTail;
        cpsThis->psTail = cpsThis->psHead;
        cpsThis->psHead = cpsThis->psCurr;
        cpsThis->psPrev = NULL;
        cpsThis->psNext = cpsThis->psCurr->pvXor;
        cpsThis->zRecord = 0;
    }

    return cpsThis;
}

size_t
listLength(
    const list_s * const cpcsThis
) {
    return ( NULL != cpcsThis ) ? ( cpcsThis->zLength ) : ( 0 ) ;
}
