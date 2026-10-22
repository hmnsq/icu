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
#അന്തര്ഭാവ "unicode/uclean.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
സ്ഥിതം സ്ഥിരം int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
സ്ഥിതം സ്ഥിരം ശൂന്യം     *pContext;
സ്ഥിതം UMemAllocFn    *pAlloc;
സ്ഥിതം UMemReallocFn  *pRealloc;
സ്ഥിതം UMemFreeFn     *pFree;

#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
#അന്തര്ഭാവ <stdio.h>
സ്ഥിതം പൂർണ്ണസംഖ്യ n=0;
സ്ഥിതം long b=0; 
#endif

U_CAPI ശൂന്യം * U_EXPORT2
uprv_malloc(size_t s) {
#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
#എങ്കിൽ 1
  putchar('>');
  fflush(stdout);
#അല്ലെങ്കിൽ
  fprintf(stderr,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); fflush(stderr);
#endif
#endif
    എങ്കിൽ (s > 0) {
        എങ്കിൽ (pAlloc) {
            തിരികെ (*pAlloc)(pContext, s);
        } അല്ലെങ്കിൽ {
            തിരികെ uprv_default_malloc(s);
        }
    } അല്ലെങ്കിൽ {
        തിരികെ (ശൂന്യം *)zeroMem;
    }
}

U_CAPI ശൂന്യം * U_EXPORT2
uprv_realloc(ശൂന്യം * buffer, size_t size) {
#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
  putchar('~');
  fflush(stdout);
#endif
    എങ്കിൽ (buffer == zeroMem) {
        തിരികെ uprv_malloc(size);
    } അല്ലെങ്കിൽ എങ്കിൽ (size == 0) {
        എങ്കിൽ (pFree) {
            (*pFree)(pContext, buffer);
        } അല്ലെങ്കിൽ {
            uprv_default_free(buffer);
        }
        തിരികെ (ശൂന്യം *)zeroMem;
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (pRealloc) {
            തിരികെ (*pRealloc)(pContext, buffer, size);
        } അല്ലെങ്കിൽ {
            തിരികെ uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI ശൂന്യം U_EXPORT2
uprv_free(ശൂന്യം *buffer) {
#എങ്കിൽ U_DEBUG && defined(UPRV_MALLOC_എണ്ണം)
  putchar('<');
  fflush(stdout);
#endif
    എങ്കിൽ (buffer != zeroMem) {
        എങ്കിൽ (pFree) {
            (*pFree)(pContext, buffer);
        } അല്ലെങ്കിൽ {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI ശൂന്യം * U_EXPORT2
uprv_calloc(size_t num, size_t size) {
    ശൂന്യം *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    എങ്കിൽ (mem) {
        uprv_memset(mem, 0, size);
    }
    തിരികെ mem;
}

U_CAPI ശൂന്യം U_EXPORT2
u_setMemoryFunctions(സ്ഥിരം ശൂന്യം *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ;
    }
    എങ്കിൽ (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
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
    തിരികെ സത്യം;
}
