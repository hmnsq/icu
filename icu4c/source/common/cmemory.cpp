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
#ಅನ್ತರ್ಭಾವ "unicode/uclean.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
ಸ್ಥಿರ ಧ್ರುವ int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
ಸ್ಥಿರ ಧ್ರುವ ಶೂನ್ಯ     *pContext;
ಸ್ಥಿರ UMemAllocFn    *pAlloc;
ಸ್ಥಿರ UMemReallocFn  *pRealloc;
ಸ್ಥಿರ UMemFreeFn     *pFree;

#ಯದಿ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ n=0;
ಸ್ಥಿರ long b=0; 
#endif

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_malloc(size_t s) {
#ಯದಿ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
#ಯದಿ 1
  putchar('>');
  fflush(stdout);
#ಅನ್ಯಥಾ
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    ಯದಿ (s > 0) {
        ಯದಿ (pAlloc) {
            ಪ್ರತ್ಯಾಗಮ (*pAlloc)(pContext, s);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ uprv_default_malloc(s);
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)zeroMem;
    }
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_realloc(ಶೂನ್ಯ * buffer, size_t size) {
#ಯದಿ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
  putchar('~');
  fflush(stdout);
#endif
    ಯದಿ (buffer == zeroMem) {
        ಪ್ರತ್ಯಾಗಮ uprv_malloc(size);
    } ಅನ್ಯಥಾ ಯದಿ (size == 0) {
        ಯದಿ (pFree) {
            (*pFree)(pContext, buffer);
        } ಅನ್ಯಥಾ {
            uprv_default_free(buffer);
        }
        ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)zeroMem;
    } ಅನ್ಯಥಾ {
        ಯದಿ (pRealloc) {
            ಪ್ರತ್ಯಾಗಮ (*pRealloc)(pContext, buffer, size);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_free(ಶೂನ್ಯ *buffer) {
#ಯದಿ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
  putchar('<');
  fflush(stdout);
#endif
    ಯದಿ (buffer != zeroMem) {
        ಯದಿ (pFree) {
            (*pFree)(pContext, buffer);
        } ಅನ್ಯಥಾ {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    ಶೂನ್ಯ *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    ಯದಿ (mem) {
        uprv_memset(mem, 0, size);
    }
    ಪ್ರತ್ಯಾಗಮ mem;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_setMemoryFunctions(ಧ್ರುವ ಶೂನ್ಯ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
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
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}
