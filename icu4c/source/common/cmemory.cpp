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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ     *pContext;
ಸ್ಥಿರ UMemAllocFn    *pAlloc;
ಸ್ಥಿರ UMemReallocFn  *pRealloc;
ಸ್ಥಿರ UMemFreeFn     *pFree;

#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
#ಅನ್ತರ್ಭಾವ <stdio.h>
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ n=0;
ಸ್ಥಿರ ದೀರ್ಘ b=0; 
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_malloc(size_t s) {
#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
#ಆದರೆ 1
  ಅಕ್ಷರ_ಬರೆ('>');
  fflush(stdout);
#ಇಲ್ಲದಿದ್ದರೆ
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (s > 0) {
        ಆದರೆ (pAlloc) {
            ಹಿಂತಿರುಗಿ (*pAlloc)(pContext, s);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ uprv_default_malloc(s);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)zeroMem;
    }
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_realloc(ಶೂನ್ಯ * ಬಫರ್, size_t size) {
#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
  ಅಕ್ಷರ_ಬರೆ('~');
  fflush(stdout);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಬಫರ್ == zeroMem) {
        ಹಿಂತಿರುಗಿ uprv_malloc(size);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (size == 0) {
        ಆದರೆ (pFree) {
            (*pFree)(pContext, ಬಫರ್);
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_default_free(ಬಫರ್);
        }
        ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)zeroMem;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (pRealloc) {
            ಹಿಂತಿರುಗಿ (*pRealloc)(pContext, ಬಫರ್, size);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ uprv_default_realloc(ಬಫರ್, size);
        }
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_free(ಶೂನ್ಯ *ಬಫರ್) {
#ಆದರೆ U_DEBUG && defined(UPRV_MALLOC_ಎಣಿಕೆ)
  ಅಕ್ಷರ_ಬರೆ('<');
  fflush(stdout);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಬಫರ್ != zeroMem) {
        ಆದರೆ (pFree) {
            (*pFree)(pContext, ಬಫರ್);
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_default_free(ಬಫರ್);
        }
    }
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    ಶೂನ್ಯ *mem = ಶೂನ್ಯಸೂಚಕ;
    size *= num;
    mem = uprv_malloc(size);
    ಆದರೆ (mem) {
        uprv_memset(mem, 0, size);
    }
    ಹಿಂತಿರುಗಿ mem;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_ಗಣ_ಸ್ಮರಣೆ_Functions(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (a==ಶೂನ್ಯಸೂಚಕ || r==ಶೂನ್ಯಸೂಚಕ || f==ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    pContext  = context;
    pAlloc    = a;
    pRealloc  = r;
    pFree     = f;
}


U_CFUNC UBool cmemory_cleanup() {
    pContext   = ಶೂನ್ಯಸೂಚಕ;
    pAlloc     = ಶೂನ್ಯಸೂಚಕ;
    pRealloc   = ಶೂನ್ಯಸೂಚಕ;
    pFree      = ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}
