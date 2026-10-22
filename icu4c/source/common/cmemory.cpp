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
#অন্তর্ভাৱ "unicode/uclean.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "putilimp.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
স্থির ধ্রুৱ int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
স্থির ধ্রুৱ শূন্য     *pContext;
স্থির UMemAllocFn    *pAlloc;
স্থির UMemReallocFn  *pRealloc;
স্থির UMemFreeFn     *pFree;

#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
#অন্তর্ভাৱ <stdio.h>
স্থির পূর্ণাঙ্ক n=0;
স্থির long b=0; 
#endif

U_CAPI শূন্য * U_EXPORT2
uprv_malloc(size_t s) {
#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
#যদি 1
  putchar('>');
  fflush(stdout);
#অন্যথা
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    যদি (s > 0) {
        যদি (pAlloc) {
            ঘূৰাই (*pAlloc)(pContext, s);
        } অন্যথা {
            ঘূৰাই uprv_default_malloc(s);
        }
    } অন্যথা {
        ঘূৰাই (শূন্য *)zeroMem;
    }
}

U_CAPI শূন্য * U_EXPORT2
uprv_realloc(শূন্য * buffer, size_t size) {
#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
  putchar('~');
  fflush(stdout);
#endif
    যদি (buffer == zeroMem) {
        ঘূৰাই uprv_malloc(size);
    } অন্যথা যদি (size == 0) {
        যদি (pFree) {
            (*pFree)(pContext, buffer);
        } অন্যথা {
            uprv_default_free(buffer);
        }
        ঘূৰাই (শূন্য *)zeroMem;
    } অন্যথা {
        যদি (pRealloc) {
            ঘূৰাই (*pRealloc)(pContext, buffer, size);
        } অন্যথা {
            ঘূৰাই uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI শূন্য U_EXPORT2
uprv_free(শূন্য *buffer) {
#যদি U_DEBUG && defined(UPRV_MALLOC_গুনতি)
  putchar('<');
  fflush(stdout);
#endif
    যদি (buffer != zeroMem) {
        যদি (pFree) {
            (*pFree)(pContext, buffer);
        } অন্যথা {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI শূন্য * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    শূন্য *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    যদি (mem) {
        uprv_memset(mem, 0, size);
    }
    ঘূৰাই mem;
}

U_CAPI শূন্য U_EXPORT2
u_setMemoryFunctions(ধ্রুৱ শূন্য *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    যদি (U_FAILURE(*status)) {
        ঘূৰাই;
    }
    যদি (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
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
    ঘূৰাই সঁচা;
}
