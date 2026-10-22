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
#ضمن "unicode/uclean.h"
#ضمن "cmemory.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"
#ضمن <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
ساكن ثابت int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
ساكن ثابت فراغ     *pContext;
ساكن UMemAllocFn    *pAlloc;
ساكن UMemReallocFn  *pRealloc;
ساكن UMemFreeFn     *pFree;

#ܐܢ U_DEBUG && defined(UPRV_MALLOC_COUNT)
#ضمن <stdio.h>
ساكن صحيح n=0;
ساكن long b=0; 
#endif

U_CAPI فراغ * U_EXPORT2
uprv_malloc(size_t s) {
#ܐܢ U_DEBUG && defined(UPRV_MALLOC_COUNT)
#ܐܢ 1
  putchar('>');
  fflush(stdout);
#ܐܚܪܢܐ
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    ܐܢ (s > 0) {
        ܐܢ (pAlloc) {
            ܦܢܐ (*pAlloc)(pContext, s);
        } ܐܚܪܢܐ {
            ܦܢܐ uprv_default_malloc(s);
        }
    } ܐܚܪܢܐ {
        ܦܢܐ (فراغ *)zeroMem;
    }
}

U_CAPI فراغ * U_EXPORT2
uprv_realloc(فراغ * buffer, size_t size) {
#ܐܢ U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('~');
  fflush(stdout);
#endif
    ܐܢ (buffer == zeroMem) {
        ܦܢܐ uprv_malloc(size);
    } ܐܚܪܢܐ ܐܢ (size == 0) {
        ܐܢ (pFree) {
            (*pFree)(pContext, buffer);
        } ܐܚܪܢܐ {
            uprv_default_free(buffer);
        }
        ܦܢܐ (فراغ *)zeroMem;
    } ܐܚܪܢܐ {
        ܐܢ (pRealloc) {
            ܦܢܐ (*pRealloc)(pContext, buffer, size);
        } ܐܚܪܢܐ {
            ܦܢܐ uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI فراغ U_EXPORT2
uprv_free(فراغ *buffer) {
#ܐܢ U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('<');
  fflush(stdout);
#endif
    ܐܢ (buffer != zeroMem) {
        ܐܢ (pFree) {
            (*pFree)(pContext, buffer);
        } ܐܚܪܢܐ {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI فراغ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    فراغ *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    ܐܢ (mem) {
        uprv_memset(mem, 0, size);
    }
    ܦܢܐ mem;
}

U_CAPI فراغ U_EXPORT2
u_setMemoryFunctions(ثابت فراغ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    ܐܢ (U_FAILURE(*status)) {
        ܦܢܐ;
    }
    ܐܢ (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ;
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
    ܦܢܐ ܫܪܪܐ;
}
