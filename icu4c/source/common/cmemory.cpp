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
#समावेश "unicode/uclean.h"
#समावेश "cmemory.h"
#समावेश "putilimp.h"
#समावेश "uassert.h"
#समावेश <stdlib.h>

/* uprv_malloc(0) returns a pointer to this read-only data. */
अटल स्थिर int32_t zeroMem[] = {0, 0, 0, 0, 0, 0};

/* Function Pointers for user-supplied heap functions  */
अटल स्थिर व्योम     *pContext;
अटल UMemAllocFn    *pAlloc;
अटल UMemReallocFn  *pRealloc;
अटल UMemFreeFn     *pFree;

#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
#समावेश <stdio.h>
अटल पूर्णांक n=0;
अटल दीर्घ b=0; 
#पूर्ण_अगर

U_CAPI व्योम * U_EXPORT2
uprv_malloc(माप_प्रकार s) {
#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
#अगर 1
  अक्षर_दो('>');
  ख_साफ(मानक_निकास);
#अन्यथा
  ख_लिखो(मानक_त्रुटि,"MALLOC\t#%d\t%ul bytes\t%ul total\n", ++n,s,(b+=s)); ख_साफ(मानक_त्रुटि);
#पूर्ण_अगर
#पूर्ण_अगर
    अगर (s > 0) {
        अगर (pAlloc) {
            वापस (*pAlloc)(pContext, s);
        } अन्यथा {
            वापस uprv_default_malloc(s);
        }
    } अन्यथा {
        वापस (व्योम *)zeroMem;
    }
}

U_CAPI व्योम * U_EXPORT2
uprv_realloc(व्योम * बफर, माप_प्रकार size) {
#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
  अक्षर_दो('~');
  ख_साफ(मानक_निकास);
#पूर्ण_अगर
    अगर (बफर == zeroMem) {
        वापस uprv_malloc(size);
    } अन्यथा अगर (size == 0) {
        अगर (pFree) {
            (*pFree)(pContext, बफर);
        } अन्यथा {
            uprv_default_free(बफर);
        }
        वापस (व्योम *)zeroMem;
    } अन्यथा {
        अगर (pRealloc) {
            वापस (*pRealloc)(pContext, बफर, size);
        } अन्यथा {
            वापस uprv_default_realloc(बफर, size);
        }
    }
}

U_CAPI व्योम U_EXPORT2
uprv_free(व्योम *बफर) {
#अगर U_DEBUG && defined(UPRV_MALLOC_गिनती)
  अक्षर_दो('<');
  ख_साफ(मानक_निकास);
#पूर्ण_अगर
    अगर (बफर != zeroMem) {
        अगर (pFree) {
            (*pFree)(pContext, बफर);
        } अन्यथा {
            uprv_default_free(बफर);
        }
    }
}

U_CAPI व्योम * U_EXPORT2
uprv_calloc(माप_प्रकार num, माप_प्रकार size) {
    व्योम *mem = nullptr;
    size *= num;
    mem = uprv_malloc(size);
    अगर (mem) {
        uprv_memset(mem, 0, size);
    }
    वापस mem;
}

U_CAPI व्योम U_EXPORT2
u_setMemoryFunctions(स्थिर व्योम *context, UMemAllocFn *a, UMemReallocFn *r, UMemFreeFn *f,  UErrorCode *status)
{
    अगर (U_FAILURE(*status)) {
        वापस;
    }
    अगर (a==nullptr || r==nullptr || f==nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
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
    वापस true;
}
