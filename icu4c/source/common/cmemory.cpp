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

#यदि U_DEBUG && defined(UPRV_MALLOC_गिनती)
#अन्तर्भाव <stdio.h>
स्थिर पूर्णाङ्क n=0;
स्थिर दीर्घ b=0; 
#endif

U_CAPI शून्य * U_EXPORT2
uprv_malloc(माप_प्रकार s) {
#यदि U_DEBUG && defined(UPRV_MALLOC_गिनती)
#यदि 1
  अक्षर_दो('>');
  ख_साफ(मानक_निकास);
#अन्यथा
  ख_लिखो(मानक_त्रुटि,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); ख_साफ(मानक_त्रुटि);
#endif
#endif
    यदि (s > 0) {
        यदि (pAlloc) {
            प्रत्यागम (*pAlloc)(pContext, s);
        } अन्यथा {
            प्रत्यागम uprv_default_malloc(s);
        }
    } अन्यथा {
        प्रत्यागम (शून्य *)zeroMem;
    }
}

U_CAPI शून्य * U_EXPORT2
uprv_realloc(शून्य * buffer, माप_प्रकार size) {
#यदि U_DEBUG && defined(UPRV_MALLOC_गिनती)
  अक्षर_दो('~');
  ख_साफ(मानक_निकास);
#endif
    यदि (buffer == zeroMem) {
        प्रत्यागम uprv_malloc(size);
    } अन्यथा यदि (size == 0) {
        यदि (pFree) {
            (*pFree)(pContext, buffer);
        } अन्यथा {
            uprv_default_free(buffer);
        }
        प्रत्यागम (शून्य *)zeroMem;
    } अन्यथा {
        यदि (pRealloc) {
            प्रत्यागम (*pRealloc)(pContext, buffer, size);
        } अन्यथा {
            प्रत्यागम uprv_default_realloc(buffer, size);
        }
    }
}

U_CAPI शून्य U_EXPORT2
uprv_free(शून्य *buffer) {
#यदि U_DEBUG && defined(UPRV_MALLOC_गिनती)
  अक्षर_दो('<');
  ख_साफ(मानक_निकास);
#endif
    यदि (buffer != zeroMem) {
        यदि (pFree) {
            (*pFree)(pContext, buffer);
        } अन्यथा {
            uprv_default_free(buffer);
        }
    }
}

U_CAPI शून्य * U_EXPORT2
uprv_calloc(माप_प्रकार num, माप_प्रकार size) {
    शून्य *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    यदि (mem) {
        uprv_memset(mem, 0, size);
    }
    प्रत्यागम mem;
}

U_CAPI शून्य U_EXPORT2
u_setMemoryFunctions(ध्रुव शून्य *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    यदि (U_FAILURE(*status)) {
        प्रत्यागम;
    }
    यदि (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
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
    प्रत्यागम सत्य;
}
