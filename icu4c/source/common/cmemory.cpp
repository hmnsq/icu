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
#అన్తర్భావ "unicode/uclean.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
స్థిర స్థిరరాశి int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
స్థిర స్థిరరాశి శూన్య     *pContext;
స్థిర UMemAllocFn    *pAlloc;
స్థిర UMemReallocFn  *pRealloc;
స్థిర UMemFreeFn     *pFree;

#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
#అన్తర్భావ <stdio.h>
స్థిర పూర్ణాంకం n=0;
స్థిర దీర్ఘ b=0; 
#అయితే_ముగింపు

U_CAPI శూన్య * U_EXPORT2
uprv_malloc(size_t s) {
#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
#ఒకవేళ 1
  అక్షరం_రాయి('>');
  fflush(stdout);
#లేకపోతే
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#అయితే_ముగింపు
#అయితే_ముగింపు
    ఒకవేళ (s > 0) {
        ఒకవేళ (pAlloc) {
            తిరిగి (*pAlloc)(pContext, s);
        } లేకపోతే {
            తిరిగి uprv_default_malloc(s);
        }
    } లేకపోతే {
        తిరిగి (శూన్య *)zeroMem;
    }
}

U_CAPI శూన్య * U_EXPORT2
uprv_realloc(శూన్య * బఫరు, size_t size) {
#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
  అక్షరం_రాయి('~');
  fflush(stdout);
#అయితే_ముగింపు
    ఒకవేళ (బఫరు == zeroMem) {
        తిరిగి uprv_malloc(size);
    } లేకపోతే ఒకవేళ (size == 0) {
        ఒకవేళ (pFree) {
            (*pFree)(pContext, బఫరు);
        } లేకపోతే {
            uprv_default_free(బఫరు);
        }
        తిరిగి (శూన్య *)zeroMem;
    } లేకపోతే {
        ఒకవేళ (pRealloc) {
            తిరిగి (*pRealloc)(pContext, బఫరు, size);
        } లేకపోతే {
            తిరిగి uprv_default_realloc(బఫరు, size);
        }
    }
}

U_CAPI శూన్య U_EXPORT2
uprv_free(శూన్య *బఫరు) {
#ఒకవేళ U_DEBUG && defined(UPRV_MALLOC_లెక్క)
  అక్షరం_రాయి('<');
  fflush(stdout);
#అయితే_ముగింపు
    ఒకవేళ (బఫరు != zeroMem) {
        ఒకవేళ (pFree) {
            (*pFree)(pContext, బఫరు);
        } లేకపోతే {
            uprv_default_free(బఫరు);
        }
    }
}

U_CAPI శూన్య * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    శూన్య *mem = శూన్యసూచిక;
    size *= num;
    mem = uprv_malloc(size);
    ఒకవేళ (mem) {
        uprv_memset(mem, 0, size);
    }
    తిరిగి mem;
}

U_CAPI శూన్య U_EXPORT2
u_సమితి_జ్ఞాపకం_Functions(స్థిరరాశి శూన్య *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UError_సంకేతం *స్థితి_2)
{
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (a==శూన్యసూచిక || r==శూన్యసూచిక || f==శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    pContext  = context;
    pAlloc    = a;
    pRealloc  = r;
    pFree     = f;
}


U_CFUNC UBool cmemory_cleanup() {
    pContext   = శూన్యసూచిక;
    pAlloc     = శూన్యసూచిక;
    pRealloc   = శూన్యసూచిక;
    pFree      = శూన్యసూచిక;
    తిరిగి నిజం;
}
