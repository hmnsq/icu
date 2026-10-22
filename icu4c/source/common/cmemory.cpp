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
#அந்தர்பாவ "unicode/uclean.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
நிலையான மாறிலி int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
நிலையான மாறிலி வெறுமை     *pContext;
நிலையான UMemAllocFn    *pAlloc;
நிலையான UMemReallocFn  *pRealloc;
நிலையான UMemFreeFn     *pFree;

#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
#அந்தர்பாவ <stdio.h>
நிலையான முழு n=0;
நிலையான long b=0; 
#endif

U_CAPI வெறுமை * U_EXPORT2
uprv_malloc(size_t s) {
#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
#எனில் 1
  putchar('>');
  fflush(stdout);
#இல்லையெனில்
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    எனில் (s > 0) {
        எனில் (pAlloc) {
            திரும்பு (*pAlloc)(pContext, s);
        } இல்லையெனில் {
            திரும்பு uprv_default_malloc(s);
        }
    } இல்லையெனில் {
        திரும்பு (வெறுமை *)zeroMem;
    }
}

U_CAPI வெறுமை * U_EXPORT2
uprv_realloc(வெறுமை * buffer, size_t size) {
#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
  putchar('~');
  fflush(stdout);
#endif
    எனில் (buffer == zeroMem) {
        திரும்பு uprv_malloc(size);
    } இல்லையெனில் எனில் (size == 0) {
        எனில் (pFree) {
            (*pFree)(pContext, buffer);
        } இல்லையெனில் {
            uprv_default_free(buffer);
        }
        திரும்பு (வெறுமை *)zeroMem;
    } இல்லையெனில் {
        எனில் (pRealloc) {
            திரும்பு (*pRealloc)(pContext, buffer, size);
        } இல்லையெனில் {
            திரும்பு uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI வெறுமை U_EXPORT2
uprv_free(வெறுமை *buffer) {
#எனில் U_DEBUG && defined(UPRV_MALLOC_மொத்தம்)
  putchar('<');
  fflush(stdout);
#endif
    எனில் (buffer != zeroMem) {
        எனில் (pFree) {
            (*pFree)(pContext, buffer);
        } இல்லையெனில் {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI வெறுமை * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    வெறுமை *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    எனில் (mem) {
        uprv_memset(mem, 0, size);
    }
    திரும்பு mem;
}

U_CAPI வெறுமை U_EXPORT2
u_setMemoryFunctions(மாறிலி வெறுமை *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    எனில் (U_FAILURE(*status)) {
        திரும்பு;
    }
    எனில் (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
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
    திரும்பு உண்மை;
}
