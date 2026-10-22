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

#ပေႃး U_DEBUG && defined(UPRV_MALLOC_COUNT)
#include <stdio.h>
static puur_naa_nka n=0;
static long b=0; 
#endif

U_CAPI void * U_EXPORT2
uprv_malloc(size_t s) {
#ပေႃး U_DEBUG && defined(UPRV_MALLOC_COUNT)
#ပေႃး 1
  putchar('>');
  fflush(stdout);
#ပေႃးဢမ်ႇ
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    ပေႃး (s > 0) {
        ပေႃး (pAlloc) {
            ပွၵ်ႈ (*pAlloc)(pContext, s);
        } ပေႃးဢမ်ႇ {
            ပွၵ်ႈ uprv_default_malloc(s);
        }
    } ပေႃးဢမ်ႇ {
        ပွၵ်ႈ (void *)zeroMem;
    }
}

U_CAPI void * U_EXPORT2
uprv_realloc(void * buffer, size_t size) {
#ပေႃး U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('~');
  fflush(stdout);
#endif
    ပေႃး (buffer == zeroMem) {
        ပွၵ်ႈ uprv_malloc(size);
    } ပေႃးဢမ်ႇ ပေႃး (size == 0) {
        ပေႃး (pFree) {
            (*pFree)(pContext, buffer);
        } ပေႃးဢမ်ႇ {
            uprv_default_free(buffer);
        }
        ပွၵ်ႈ (void *)zeroMem;
    } ပေႃးဢမ်ႇ {
        ပေႃး (pRealloc) {
            ပွၵ်ႈ (*pRealloc)(pContext, buffer, size);
        } ပေႃးဢမ်ႇ {
            ပွၵ်ႈ uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI void U_EXPORT2
uprv_free(void *buffer) {
#ပေႃး U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('<');
  fflush(stdout);
#endif
    ပေႃး (buffer != zeroMem) {
        ပေႃး (pFree) {
            (*pFree)(pContext, buffer);
        } ပေႃးဢမ်ႇ {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI void * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    void *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    ပေႃး (mem) {
        uprv_memset(mem, 0, size);
    }
    ပွၵ်ႈ mem;
}

U_CAPI void U_EXPORT2
u_setMemoryFunctions(const void *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    ပေႃး (U_FAILURE(*status)) {
        ပွၵ်ႈ;
    }
    ပေႃး (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ;
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
    ပွၵ်ႈ true;
}
