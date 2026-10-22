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

#ຖ້າ U_DEBUG && defined(UPRV_MALLOC_COUNT)
#include <stdio.h>
static ຈຳນວນເຕັມ n=0;
static long b=0; 
#endif

U_CAPI void * U_EXPORT2
uprv_malloc(size_t s) {
#ຖ້າ U_DEBUG && defined(UPRV_MALLOC_COUNT)
#ຖ້າ 1
  putchar('>');
  fflush(stdout);
#ບໍ່ດັ່ງນັ້ນ
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    ຖ້າ (s > 0) {
        ຖ້າ (pAlloc) {
            ສົ່ງຄືນ (*pAlloc)(pContext, s);
        } ບໍ່ດັ່ງນັ້ນ {
            ສົ່ງຄືນ uprv_default_malloc(s);
        }
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ (void *)zeroMem;
    }
}

U_CAPI void * U_EXPORT2
uprv_realloc(void * buffer, size_t size) {
#ຖ້າ U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('~');
  fflush(stdout);
#endif
    ຖ້າ (buffer == zeroMem) {
        ສົ່ງຄືນ uprv_malloc(size);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (size == 0) {
        ຖ້າ (pFree) {
            (*pFree)(pContext, buffer);
        } ບໍ່ດັ່ງນັ້ນ {
            uprv_default_free(buffer);
        }
        ສົ່ງຄືນ (void *)zeroMem;
    } ບໍ່ດັ່ງນັ້ນ {
        ຖ້າ (pRealloc) {
            ສົ່ງຄືນ (*pRealloc)(pContext, buffer, size);
        } ບໍ່ດັ່ງນັ້ນ {
            ສົ່ງຄືນ uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI void U_EXPORT2
uprv_free(void *buffer) {
#ຖ້າ U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('<');
  fflush(stdout);
#endif
    ຖ້າ (buffer != zeroMem) {
        ຖ້າ (pFree) {
            (*pFree)(pContext, buffer);
        } ບໍ່ດັ່ງນັ້ນ {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI void * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    void *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    ຖ້າ (mem) {
        uprv_memset(mem, 0, size);
    }
    ສົ່ງຄືນ mem;
}

U_CAPI void U_EXPORT2
u_setMemoryFunctions(const void *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    ຖ້າ (U_FAILURE(*status)) {
        ສົ່ງຄືນ;
    }
    ຖ້າ (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ;
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
    ສົ່ງຄືນ true;
}
