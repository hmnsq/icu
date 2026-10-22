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

#إذا U_DEBUG && defined(UPRV_MALLOC_عدد)
#ضمن <stdio.h>
ساكن صحيح n=0;
ساكن long b=0; 
#endif

U_CAPI فراغ * U_EXPORT2
uprv_malloc(size_t s) {
#إذا U_DEBUG && defined(UPRV_MALLOC_عدد)
#إذا 1
  putchar('>');
  fflush(stdout);
#وإلا
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    إذا (s > 0) {
        إذا (pAlloc) {
            أرجع (*pAlloc)(pContext, s);
        } وإلا {
            أرجع uprv_default_malloc(s);
        }
    } وإلا {
        أرجع (فراغ *)zeroMem;
    }
}

U_CAPI فراغ * U_EXPORT2
uprv_realloc(فراغ * buffer, size_t size) {
#إذا U_DEBUG && defined(UPRV_MALLOC_عدد)
  putchar('~');
  fflush(stdout);
#endif
    إذا (buffer == zeroMem) {
        أرجع uprv_malloc(size);
    } وإلا إذا (size == 0) {
        إذا (pFree) {
            (*pFree)(pContext, buffer);
        } وإلا {
            uprv_default_free(buffer);
        }
        أرجع (فراغ *)zeroMem;
    } وإلا {
        إذا (pRealloc) {
            أرجع (*pRealloc)(pContext, buffer, size);
        } وإلا {
            أرجع uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI فراغ U_EXPORT2
uprv_free(فراغ *buffer) {
#إذا U_DEBUG && defined(UPRV_MALLOC_عدد)
  putchar('<');
  fflush(stdout);
#endif
    إذا (buffer != zeroMem) {
        إذا (pFree) {
            (*pFree)(pContext, buffer);
        } وإلا {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI فراغ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    فراغ *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    إذا (mem) {
        uprv_memset(mem, 0, size);
    }
    أرجع mem;
}

U_CAPI فراغ U_EXPORT2
u_setMemoryFunctions(ثابت فراغ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    إذا (U_FAILURE(*status)) {
        أرجع;
    }
    إذا (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
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
    أرجع true;
}
