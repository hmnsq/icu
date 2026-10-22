// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2002-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File cmemory.c      ICU Heap allocation.
*                     All ICU heap allocation, both for C and C++ new of ICU
*                     class types, comes through these functions.
*
*                     If you have a need to replace ICU allocation, this is the
*                     place to do it.
*
*                     Note that uprv_malloc(0) returns a non-nullptr pointer,
*                     and that a subsequent free of that pointer value is a NOP.
*
******************************************************************************
*/
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uclean.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
ਸਥਿਰ ਅਚਲ int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
ਸਥਿਰ ਅਚਲ ਖਾਲੀ     *pContext;
ਸਥਿਰ UMemAllocFn    *pAlloc;
ਸਥਿਰ UMemReallocFn  *pRealloc;
ਸਥਿਰ UMemFreeFn     *pFree;

#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
ਸਥਿਰ ਪੂਰਨ n=0;
ਸਥਿਰ ਲੰਮਾ b=0; 
#ਜੇ_ਅੰਤ

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_malloc(size_t s) {
#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
#ਜੇ 1
  ਅੱਖਰ_ਲਿਖੋ('>');
  fflush(stdout);
#ਨਹੀਂਤਾਂ
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
    ਜੇ (s > 0) {
        ਜੇ (pAlloc) {
            ਵਾਪਸ (*pAlloc)(pContext, s);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ uprv_default_malloc(s);
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ (ਖਾਲੀ *)zeroMem;
    }
}

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_realloc(ਖਾਲੀ * ਬਫਰ, size_t size) {
#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
  ਅੱਖਰ_ਲਿਖੋ('~');
  fflush(stdout);
#ਜੇ_ਅੰਤ
    ਜੇ (ਬਫਰ == zeroMem) {
        ਵਾਪਸ uprv_malloc(size);
    } ਨਹੀਂਤਾਂ ਜੇ (size == 0) {
        ਜੇ (pFree) {
            (*pFree)(pContext, ਬਫਰ);
        } ਨਹੀਂਤਾਂ {
            uprv_default_free(ਬਫਰ);
        }
        ਵਾਪਸ (ਖਾਲੀ *)zeroMem;
    } ਨਹੀਂਤਾਂ {
        ਜੇ (pRealloc) {
            ਵਾਪਸ (*pRealloc)(pContext, ਬਫਰ, size);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ uprv_default_realloc(ਬਫਰ, size);
        }
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
uprv_free(ਖਾਲੀ *ਬਫਰ) {
#ਜੇ U_DEBUG && defined(UPRV_MALLOC_ਗਿਣਤੀ)
  ਅੱਖਰ_ਲਿਖੋ('<');
  fflush(stdout);
#ਜੇ_ਅੰਤ
    ਜੇ (ਬਫਰ != zeroMem) {
        ਜੇ (pFree) {
            (*pFree)(pContext, ਬਫਰ);
        } ਨਹੀਂਤਾਂ {
            uprv_default_free(ਬਫਰ);
        }
    }
}

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    ਖਾਲੀ *mem = ਖਾਲੀ_ਸੂਚਕ;
    size *= num;
    mem = uprv_malloc(size);
    ਜੇ (mem) {
        uprv_memset(mem, 0, size);
    }
    ਵਾਪਸ mem;
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_ਸੈੱਟ_ਸਮ੍ਰਿਤੀ_Functions(ਅਚਲ ਖਾਲੀ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (a==ਖਾਲੀ_ਸੂਚਕ || r==ਖਾਲੀ_ਸੂਚਕ || f==ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    pContext  = context;
    pAlloc    = a;
    pRealloc  = r;
    pFree     = f;
}


U_CFUNC UBool cmemory_cleanup() {
    pContext   = ਖਾਲੀ_ਸੂਚਕ;
    pAlloc     = ਖਾਲੀ_ਸੂਚਕ;
    pRealloc   = ਖਾਲੀ_ਸੂਚਕ;
    pFree      = ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ ਸੱਚ;
}
