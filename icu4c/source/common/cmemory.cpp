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
#include "unicode/uclean.h"
#include "cmemory.h"
#include "putilimp.h"
#include "uassert.h"
#include <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
static const int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
static const void     *pContext;
static UMemAllocFn    *pAlloc;
static UMemReallocFn  *pRealloc;
static UMemFreeFn     *pFree;

#ถ้า U_DEBUG && defined(UPRV_MALLOC_COUNT)
#include <stdio.h>
static จำนวนเต็ม n=0;
static long b=0; 
#endif

U_CAPI void * U_EXPORT2
uprv_malloc(size_t s) {
#ถ้า U_DEBUG && defined(UPRV_MALLOC_COUNT)
#ถ้า 1
  putchar('>');
  fflush(stdout);
#มิฉะนั้น
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    ถ้า (s > 0) {
        ถ้า (pAlloc) {
            คืน (*pAlloc)(pContext, s);
        } มิฉะนั้น {
            คืน uprv_default_malloc(s);
        }
    } มิฉะนั้น {
        คืน (void *)zeroMem;
    }
}

U_CAPI void * U_EXPORT2
uprv_realloc(void * buffer, size_t size) {
#ถ้า U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('~');
  fflush(stdout);
#endif
    ถ้า (buffer == zeroMem) {
        คืน uprv_malloc(size);
    } มิฉะนั้น ถ้า (size == 0) {
        ถ้า (pFree) {
            (*pFree)(pContext, buffer);
        } มิฉะนั้น {
            uprv_default_free(buffer);
        }
        คืน (void *)zeroMem;
    } มิฉะนั้น {
        ถ้า (pRealloc) {
            คืน (*pRealloc)(pContext, buffer, size);
        } มิฉะนั้น {
            คืน uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI void U_EXPORT2
uprv_free(void *buffer) {
#ถ้า U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('<');
  fflush(stdout);
#endif
    ถ้า (buffer != zeroMem) {
        ถ้า (pFree) {
            (*pFree)(pContext, buffer);
        } มิฉะนั้น {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI void * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    void *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    ถ้า (mem) {
        uprv_memset(mem, 0, size);
    }
    คืน mem;
}

U_CAPI void U_EXPORT2
u_setMemoryFunctions(const void *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    ถ้า (U_FAILURE(*status)) {
        คืน;
    }
    ถ้า (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        คืน;
    }
    pContext  = context;
    pAlloc    = a;
    pRealloc  = r;
    pFree     = f;
}


U_CFUNC UBool cmemory_cleanup() {
    pContext   = nullptr;
    pAlloc     = nullptr;
    pRealloc   = nullptr;
    pFree      = nullptr;
    คืน true;
}
