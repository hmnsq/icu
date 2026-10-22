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
ساکن ثابت int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
ساکن ثابت خالی     *pContext;
ساکن UMemAllocFn    *pAlloc;
ساکن UMemReallocFn  *pRealloc;
ساکن UMemFreeFn     *pFree;

#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
#شامل <stdio.h>
ساکن صحیح n=0;
ساکن طویل b=0; 
#اگر_ختم

U_CAPI خالی * U_EXPORT2
uprv_malloc(size_t s) {
#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
#اگر 1
  حرف_لکھو('>');
  fflush(stdout);
#ورنہ
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#اگر_ختم
#اگر_ختم
    اگر (s > 0) {
        اگر (pAlloc) {
            واپس (*pAlloc)(pContext, s);
        } ورنہ {
            واپس uprv_default_malloc(s);
        }
    } ورنہ {
        واپس (خالی *)zeroMem;
    }
}

U_CAPI خالی * U_EXPORT2
uprv_realloc(خالی * بفر, size_t size) {
#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
  حرف_لکھو('~');
  fflush(stdout);
#اگر_ختم
    اگر (بفر == zeroMem) {
        واپس uprv_malloc(size);
    } ورنہ اگر (size == 0) {
        اگر (pFree) {
            (*pFree)(pContext, بفر);
        } ورنہ {
            uprv_default_free(بفر);
        }
        واپس (خالی *)zeroMem;
    } ورنہ {
        اگر (pRealloc) {
            واپس (*pRealloc)(pContext, بفر, size);
        } ورنہ {
            واپس uprv_default_realloc(بفر, size);
        }
    }
}

U_CAPI خالی U_EXPORT2
uprv_free(خالی *بفر) {
#اگر U_DEBUG && defined(UPRV_MALLOC_گنتی)
  حرف_لکھو('<');
  fflush(stdout);
#اگر_ختم
    اگر (بفر != zeroMem) {
        اگر (pFree) {
            (*pFree)(pContext, بفر);
        } ورنہ {
            uprv_default_free(بفر);
        }
    }
}

U_CAPI خالی * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    خالی *mem = خالی_اشاریہ;
    size *= num;
    mem = uprv_malloc(size);
    اگر (mem) {
        uprv_memset(mem, 0, size);
    }
    واپس mem;
}

U_CAPI خالی U_EXPORT2
u_سیٹ_حافظہ_Functions(ثابت خالی *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UError_رمز *حالت_2)
{
    اگر (U_FAILURE(*حالت_2)) {
        واپس;
    }
    اگر (a==خالی_اشاریہ || r==خالی_اشاریہ || f==خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    pContext  = context;
    pAlloc    = a;
    pRealloc  = r;
    pFree     = f;
}


U_CFUNC UBool cmemory_cleanup() {
    pContext   = خالی_اشاریہ;
    pAlloc     = خالی_اشاریہ;
    pRealloc   = خالی_اشاریہ;
    pFree      = خالی_اشاریہ;
    واپس سچ;
}
