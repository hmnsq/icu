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
#شامل "unicode/uclean.h"
#شامل "cmemory.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
ایستا ثابت int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
ایستا ثابت تهی     *pContext;
ایستا UMemAllocFn    *pAlloc;
ایستا UMemReallocFn  *pRealloc;
ایستا UMemFreeFn     *pFree;

#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
#شامل <stdio.h>
ایستا صحیح n=0;
ایستا long b=0; 
#endif

U_CAPI تهی * U_EXPORT2
uprv_malloc(size_t s) {
#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
#اگر 1
  putchar('>');
  fflush(stdout);
#وگرنه
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    اگر (s > 0) {
        اگر (pAlloc) {
            بازگشت (*pAlloc)(pContext, s);
        } وگرنه {
            بازگشت uprv_default_malloc(s);
        }
    } وگرنه {
        بازگشت (تهی *)zeroMem;
    }
}

U_CAPI تهی * U_EXPORT2
uprv_realloc(تهی * buffer, size_t size) {
#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
  putchar('~');
  fflush(stdout);
#endif
    اگر (buffer == zeroMem) {
        بازگشت uprv_malloc(size);
    } وگرنه اگر (size == 0) {
        اگر (pFree) {
            (*pFree)(pContext, buffer);
        } وگرنه {
            uprv_default_free(buffer);
        }
        بازگشت (تهی *)zeroMem;
    } وگرنه {
        اگر (pRealloc) {
            بازگشت (*pRealloc)(pContext, buffer, size);
        } وگرنه {
            بازگشت uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI تهی U_EXPORT2
uprv_free(تهی *buffer) {
#اگر U_DEBUG && defined(UPRV_MALLOC_شمار)
  putchar('<');
  fflush(stdout);
#endif
    اگر (buffer != zeroMem) {
        اگر (pFree) {
            (*pFree)(pContext, buffer);
        } وگرنه {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI تهی * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    تهی *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    اگر (mem) {
        uprv_memset(mem, 0, size);
    }
    بازگشت mem;
}

U_CAPI تهی U_EXPORT2
u_setMemoryFunctions(ثابت تهی *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    اگر (U_FAILURE(*status)) {
        بازگشت;
    }
    اگر (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
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
    بازگشت درست;
}
