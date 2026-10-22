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

#إذا U_DEBUG && defined(UPRV_MALLOC_شمېر)
#ضمن <stdio.h>
ساكن صحيح n=0;
ساكن اوږد b=0; 
#د_که_پای

U_CAPI فراغ * U_EXPORT2
uprv_malloc(size_t s) {
#إذا U_DEBUG && defined(UPRV_MALLOC_شمېر)
#إذا 1
  توری_چاپ_کړه('>');
  fflush(stdout);
#وإلا
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#د_که_پای
#د_که_پای
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
uprv_realloc(فراغ * بفر, size_t size) {
#إذا U_DEBUG && defined(UPRV_MALLOC_شمېر)
  توری_چاپ_کړه('~');
  fflush(stdout);
#د_که_پای
    إذا (بفر == zeroMem) {
        أرجع uprv_malloc(size);
    } وإلا إذا (size == 0) {
        إذا (pFree) {
            (*pFree)(pContext, بفر);
        } وإلا {
            uprv_default_free(بفر);
        }
        أرجع (فراغ *)zeroMem;
    } وإلا {
        إذا (pRealloc) {
            أرجع (*pRealloc)(pContext, بفر, size);
        } وإلا {
            أرجع uprv_default_realloc(بفر, size);
        }
    }
}

U_CAPI فراغ U_EXPORT2
uprv_free(فراغ *بفر) {
#إذا U_DEBUG && defined(UPRV_MALLOC_شمېر)
  توری_چاپ_کړه('<');
  fflush(stdout);
#د_که_پای
    إذا (بفر != zeroMem) {
        إذا (pFree) {
            (*pFree)(pContext, بفر);
        } وإلا {
            uprv_default_free(بفر);
        }
    }
}

U_CAPI فراغ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    فراغ *mem = تش_اشاره;
    size *= num;
    mem = uprv_malloc(size);
    إذا (mem) {
        uprv_memset(mem, 0, size);
    }
    أرجع mem;
}

U_CAPI فراغ U_EXPORT2
u_سېټ_حافظه_Functions(ثابت فراغ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UError_کوډ *حالت)
{
    إذا (U_FAILURE(*حالت)) {
        أرجع;
    }
    إذا (a==تش_اشاره || r==تش_اشاره || f==تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    pContext  = context;
    pAlloc    = a;
    pRealloc  = r;
    pFree     = f;
}


U_CFUNC UBool cmemory_cleanup() {
    pContext   = تش_اشاره;
    pAlloc     = تش_اشاره;
    pRealloc   = تش_اشاره;
    pFree      = تش_اشاره;
    أرجع ریښتیا;
}
