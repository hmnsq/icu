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
#ଅନ୍ତର୍ଭାଵ "unicode/uclean.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
ସ୍ଥିର ଅଚଳ int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
ସ୍ଥିର ଅଚଳ ଶୂନ୍ୟ     *pContext;
ସ୍ଥିର UMemAllocFn    *pAlloc;
ସ୍ଥିର UMemReallocFn  *pRealloc;
ସ୍ଥିର UMemFreeFn     *pFree;

#ଯଦି U_DEBUG && defined(UPRV_MALLOC_ଗଣନା)
#ଅନ୍ତର୍ଭାଵ <stdio.h>
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା n=0;
ସ୍ଥିର long b=0; 
#endif

U_CAPI ଶୂନ୍ୟ * U_EXPORT2
uprv_malloc(size_t s) {
#ଯଦି U_DEBUG && defined(UPRV_MALLOC_ଗଣନା)
#ଯଦି 1
  putchar('>');
  fflush(stdout);
#ଅନ୍ୟଥା
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    ଯଦି (s > 0) {
        ଯଦି (pAlloc) {
            ଫେରନ୍ତୁ (*pAlloc)(pContext, s);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ uprv_default_malloc(s);
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)zeroMem;
    }
}

U_CAPI ଶୂନ୍ୟ * U_EXPORT2
uprv_realloc(ଶୂନ୍ୟ * buffer, size_t size) {
#ଯଦି U_DEBUG && defined(UPRV_MALLOC_ଗଣନା)
  putchar('~');
  fflush(stdout);
#endif
    ଯଦି (buffer == zeroMem) {
        ଫେରନ୍ତୁ uprv_malloc(size);
    } ଅନ୍ୟଥା ଯଦି (size == 0) {
        ଯଦି (pFree) {
            (*pFree)(pContext, buffer);
        } ଅନ୍ୟଥା {
            uprv_default_free(buffer);
        }
        ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)zeroMem;
    } ଅନ୍ୟଥା {
        ଯଦି (pRealloc) {
            ଫେରନ୍ତୁ (*pRealloc)(pContext, buffer, size);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uprv_free(ଶୂନ୍ୟ *buffer) {
#ଯଦି U_DEBUG && defined(UPRV_MALLOC_ଗଣନା)
  putchar('<');
  fflush(stdout);
#endif
    ଯଦି (buffer != zeroMem) {
        ଯଦି (pFree) {
            (*pFree)(pContext, buffer);
        } ଅନ୍ୟଥା {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI ଶୂନ୍ୟ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    ଶୂନ୍ୟ *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    ଯଦି (mem) {
        uprv_memset(mem, 0, size);
    }
    ଫେରନ୍ତୁ mem;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
u_setMemoryFunctions(ଅଚଳ ଶୂନ୍ୟ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
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
    ଫେରନ୍ତୁ ସତ୍ୟ;
}
