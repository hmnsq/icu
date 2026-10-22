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
#અન્તર્ભાવ "unicode/uclean.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
સ્થિર અચલ int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
સ્થિર અચલ શૂન્ય     *pContext;
સ્થિર UMemAllocFn    *pAlloc;
સ્થિર UMemReallocFn  *pRealloc;
સ્થિર UMemFreeFn     *pFree;

#જો U_DEBUG && defined(UPRV_MALLOC_ગણતરી)
#અન્તર્ભાવ <stdio.h>
સ્થિર પૂર્ણાંક n=0;
સ્થિર long b=0; 
#endif

U_CAPI શૂન્ય * U_EXPORT2
uprv_malloc(size_t s) {
#જો U_DEBUG && defined(UPRV_MALLOC_ગણતરી)
#જો 1
  putchar('>');
  fflush(stdout);
#નહીંતર
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    જો (s > 0) {
        જો (pAlloc) {
            પાછા (*pAlloc)(pContext, s);
        } નહીંતર {
            પાછા uprv_default_malloc(s);
        }
    } નહીંતર {
        પાછા (શૂન્ય *)zeroMem;
    }
}

U_CAPI શૂન્ય * U_EXPORT2
uprv_realloc(શૂન્ય * buffer, size_t size) {
#જો U_DEBUG && defined(UPRV_MALLOC_ગણતરી)
  putchar('~');
  fflush(stdout);
#endif
    જો (buffer == zeroMem) {
        પાછા uprv_malloc(size);
    } નહીંતર જો (size == 0) {
        જો (pFree) {
            (*pFree)(pContext, buffer);
        } નહીંતર {
            uprv_default_free(buffer);
        }
        પાછા (શૂન્ય *)zeroMem;
    } નહીંતર {
        જો (pRealloc) {
            પાછા (*pRealloc)(pContext, buffer, size);
        } નહીંતર {
            પાછા uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI શૂન્ય U_EXPORT2
uprv_free(શૂન્ય *buffer) {
#જો U_DEBUG && defined(UPRV_MALLOC_ગણતરી)
  putchar('<');
  fflush(stdout);
#endif
    જો (buffer != zeroMem) {
        જો (pFree) {
            (*pFree)(pContext, buffer);
        } નહીંતર {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI શૂન્ય * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    શૂન્ય *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    જો (mem) {
        uprv_memset(mem, 0, size);
    }
    પાછા mem;
}

U_CAPI શૂન્ય U_EXPORT2
u_setMemoryFunctions(અચલ શૂન્ય *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    જો (U_FAILURE(*status)) {
        પાછા;
    }
    જો (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
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
    પાછા સાચું;
}
