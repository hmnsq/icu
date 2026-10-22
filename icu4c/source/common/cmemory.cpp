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

#အကယ်၍ U_DEBUG && defined(UPRV_MALLOC_COUNT)
#include <stdio.h>
static ကိန်းပြည့် n=0;
static long b=0; 
#endif

U_CAPI void * U_EXPORT2
uprv_malloc(size_t s) {
#အကယ်၍ U_DEBUG && defined(UPRV_MALLOC_COUNT)
#အကယ်၍ 1
  putchar('>');
  fflush(stdout);
#မဟုတ်ပါက
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    အကယ်၍ (s > 0) {
        အကယ်၍ (pAlloc) {
            ပြန်ပေး (*pAlloc)(pContext, s);
        } မဟုတ်ပါက {
            ပြန်ပေး uprv_default_malloc(s);
        }
    } မဟုတ်ပါက {
        ပြန်ပေး (void *)zeroMem;
    }
}

U_CAPI void * U_EXPORT2
uprv_realloc(void * buffer, size_t size) {
#အကယ်၍ U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('~');
  fflush(stdout);
#endif
    အကယ်၍ (buffer == zeroMem) {
        ပြန်ပေး uprv_malloc(size);
    } မဟုတ်ပါက အကယ်၍ (size == 0) {
        အကယ်၍ (pFree) {
            (*pFree)(pContext, buffer);
        } မဟုတ်ပါက {
            uprv_default_free(buffer);
        }
        ပြန်ပေး (void *)zeroMem;
    } မဟုတ်ပါက {
        အကယ်၍ (pRealloc) {
            ပြန်ပေး (*pRealloc)(pContext, buffer, size);
        } မဟုတ်ပါက {
            ပြန်ပေး uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI void U_EXPORT2
uprv_free(void *buffer) {
#အကယ်၍ U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('<');
  fflush(stdout);
#endif
    အကယ်၍ (buffer != zeroMem) {
        အကယ်၍ (pFree) {
            (*pFree)(pContext, buffer);
        } မဟုတ်ပါက {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI void * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    void *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    အကယ်၍ (mem) {
        uprv_memset(mem, 0, size);
    }
    ပြန်ပေး mem;
}

U_CAPI void U_EXPORT2
u_setMemoryFunctions(const void *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    အကယ်၍ (U_FAILURE(*status)) {
        ပြန်ပေး;
    }
    အကယ်၍ (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ပြန်ပေး;
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
    ပြန်ပေး true;
}
