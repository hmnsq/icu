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

#אם U_DEBUG && defined(UPRV_MALLOC_COUNT)
#ضمن <stdio.h>
ساكن صحيح n=0;
ساكن long b=0; 
#endif

U_CAPI فراغ * U_EXPORT2
uprv_malloc(size_t s) {
#אם U_DEBUG && defined(UPRV_MALLOC_COUNT)
#אם 1
  putchar('>');
  fflush(stdout);
#אחרת
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    אם (s > 0) {
        אם (pAlloc) {
            החזר (*pAlloc)(pContext, s);
        } אחרת {
            החזר uprv_default_malloc(s);
        }
    } אחרת {
        החזר (فراغ *)zeroMem;
    }
}

U_CAPI فراغ * U_EXPORT2
uprv_realloc(فراغ * buffer, size_t size) {
#אם U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('~');
  fflush(stdout);
#endif
    אם (buffer == zeroMem) {
        החזר uprv_malloc(size);
    } אחרת אם (size == 0) {
        אם (pFree) {
            (*pFree)(pContext, buffer);
        } אחרת {
            uprv_default_free(buffer);
        }
        החזר (فراغ *)zeroMem;
    } אחרת {
        אם (pRealloc) {
            החזר (*pRealloc)(pContext, buffer, size);
        } אחרת {
            החזר uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI فراغ U_EXPORT2
uprv_free(فراغ *buffer) {
#אם U_DEBUG && defined(UPRV_MALLOC_COUNT)
  putchar('<');
  fflush(stdout);
#endif
    אם (buffer != zeroMem) {
        אם (pFree) {
            (*pFree)(pContext, buffer);
        } אחרת {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI فراغ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    فراغ *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    אם (mem) {
        uprv_memset(mem, 0, size);
    }
    החזר mem;
}

U_CAPI فراغ U_EXPORT2
u_setMemoryFunctions(ثابت فراغ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    אם (U_FAILURE(*status)) {
        החזר;
    }
    אם (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        החזר;
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
    החזר אמת;
}
