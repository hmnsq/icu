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
#अन्तर्भाव "unicode/uclean.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
स्थिर ध्रुव int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
स्थिर ध्रुव शून्य     *pContext;
स्थिर UMemAllocFn    *pAlloc;
स्थिर UMemReallocFn  *pRealloc;
स्थिर UMemFreeFn     *pFree;

#सचे U_DEBUG && defined(UPRV_MALLOC_गणनम्)
#अन्तर्भाव <stdio.h>
स्थिर पूर्णाङ्क n=0;
स्थिर long b=0; 
#endif

U_CAPI शून्य * U_EXPORT2
uprv_malloc(size_t s) {
#सचे U_DEBUG && defined(UPRV_MALLOC_गणनम्)
#सचे 1
  putchar('>');
  fflush(stdout);
#अञ्ञथा
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    सचे (s > 0) {
        सचे (pAlloc) {
            पटिनिवत्त (*pAlloc)(pContext, s);
        } अञ्ञथा {
            पटिनिवत्त uprv_default_malloc(s);
        }
    } अञ्ञथा {
        पटिनिवत्त (शून्य *)zeroMem;
    }
}

U_CAPI शून्य * U_EXPORT2
uprv_realloc(शून्य * buffer, size_t size) {
#सचे U_DEBUG && defined(UPRV_MALLOC_गणनम्)
  putchar('~');
  fflush(stdout);
#endif
    सचे (buffer == zeroMem) {
        पटिनिवत्त uprv_malloc(size);
    } अञ्ञथा सचे (size == 0) {
        सचे (pFree) {
            (*pFree)(pContext, buffer);
        } अञ्ञथा {
            uprv_default_free(buffer);
        }
        पटिनिवत्त (शून्य *)zeroMem;
    } अञ्ञथा {
        सचे (pRealloc) {
            पटिनिवत्त (*pRealloc)(pContext, buffer, size);
        } अञ्ञथा {
            पटिनिवत्त uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI शून्य U_EXPORT2
uprv_free(शून्य *buffer) {
#सचे U_DEBUG && defined(UPRV_MALLOC_गणनम्)
  putchar('<');
  fflush(stdout);
#endif
    सचे (buffer != zeroMem) {
        सचे (pFree) {
            (*pFree)(pContext, buffer);
        } अञ्ञथा {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI शून्य * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    शून्य *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    सचे (mem) {
        uprv_memset(mem, 0, size);
    }
    पटिनिवत्त mem;
}

U_CAPI शून्य U_EXPORT2
u_setMemoryFunctions(ध्रुव शून्य *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त;
    }
    सचे (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
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
    पटिनिवत्त सच्च;
}
