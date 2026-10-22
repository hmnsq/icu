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

#නම් U_DEBUG && defined(UPRV_MALLOC_COUNT)
#include <stdio.h>
static පූර්ණ_සංඛ්‍යා n=0;
static long b=0; 
#endif

U_CAPI void * U_EXPORT2
uprv_malloc(size_t s) {
#නම් U_DEBUG && defined(UPRV_MALLOC_COUNT)
#නම් 1
  putchar('>');
  fflush(stdout);
#නැතහොත්
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    නම් (s > 0) {
        නම් (pAlloc) {
            ආපසු (*pAlloc)(pContext, s);
        } නැතහොත් {
            ආපසු uprv_default_malloc(s);
        }
    } නැතහොත් {
        ආපසු (void *)zeroMem;
    }
}

U_CAPI void * U_EXPORT2
uprv_realloc(void * buffer, size_t size) {
#නම් U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('~');
  fflush(stdout);
#endif
    නම් (buffer == zeroMem) {
        ආපසු uprv_malloc(size);
    } නැතහොත් නම් (size == 0) {
        නම් (pFree) {
            (*pFree)(pContext, buffer);
        } නැතහොත් {
            uprv_default_free(buffer);
        }
        ආපසු (void *)zeroMem;
    } නැතහොත් {
        නම් (pRealloc) {
            ආපසු (*pRealloc)(pContext, buffer, size);
        } නැතහොත් {
            ආපසු uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI void U_EXPORT2
uprv_free(void *buffer) {
#නම් U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('<');
  fflush(stdout);
#endif
    නම් (buffer != zeroMem) {
        නම් (pFree) {
            (*pFree)(pContext, buffer);
        } නැතහොත් {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI void * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    void *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    නම් (mem) {
        uprv_memset(mem, 0, size);
    }
    ආපසු mem;
}

U_CAPI void U_EXPORT2
u_setMemoryFunctions(const void *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    නම් (U_FAILURE(*status)) {
        ආපසු;
    }
    නම් (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ආපසු;
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
    ආපසු true;
}
