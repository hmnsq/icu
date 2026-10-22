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

#إذا U_DEBUG && defined(UPRV_MALLOC_شمار)
#ضمن <stdio.h>
ساكن صحيح n=0;
ساكن دراز b=0; 
#پایان_اگر

U_CAPI فراغ * U_EXPORT2
uprv_malloc(size_t s) {
#إذا U_DEBUG && defined(UPRV_MALLOC_شمار)
#إذا 1
  حرف_چاپ_کن('>');
  fflush(stdout);
#وإلا
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#پایان_اگر
#پایان_اگر
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
uprv_realloc(فراغ * میانگیر, size_t size) {
#إذا U_DEBUG && defined(UPRV_MALLOC_شمار)
  حرف_چاپ_کن('~');
  fflush(stdout);
#پایان_اگر
    إذا (میانگیر == zeroMem) {
        أرجع uprv_malloc(size);
    } وإلا إذا (size == 0) {
        إذا (pFree) {
            (*pFree)(pContext, میانگیر);
        } وإلا {
            uprv_default_free(میانگیر);
        }
        أرجع (فراغ *)zeroMem;
    } وإلا {
        إذا (pRealloc) {
            أرجع (*pRealloc)(pContext, میانگیر, size);
        } وإلا {
            أرجع uprv_default_realloc(میانگیر, size);
        }
    }
}

U_CAPI فراغ U_EXPORT2
uprv_free(فراغ *میانگیر) {
#إذا U_DEBUG && defined(UPRV_MALLOC_شمار)
  حرف_چاپ_کن('<');
  fflush(stdout);
#پایان_اگر
    إذا (میانگیر != zeroMem) {
        إذا (pFree) {
            (*pFree)(pContext, میانگیر);
        } وإلا {
            uprv_default_free(میانگیر);
        }
    }
}

U_CAPI فراغ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    فراغ *mem = اشاره_تهی;
    size *= num;
    mem = uprv_malloc(size);
    إذا (mem) {
        uprv_memset(mem, 0, size);
    }
    أرجع mem;
}

U_CAPI فراغ U_EXPORT2
u_مجموعه_حافظه_Functions(ثابت فراغ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UError_کد *وضعیت)
{
    إذا (U_FAILURE(*وضعیت)) {
        أرجع;
    }
    إذا (a==اشاره_تهی || r==اشاره_تهی || f==اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    pContext  = context;
    pAlloc    = a;
    pRealloc  = r;
    pFree     = f;
}


U_CFUNC UBool cmemory_cleanup() {
    pContext   = اشاره_تهی;
    pAlloc     = اشاره_تهی;
    pRealloc   = اشاره_تهی;
    pFree      = اشاره_تهی;
    أرجع درست;
}
