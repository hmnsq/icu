// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uenum.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2002jul08
*   created by: Vladimir Weinstein
*/

#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "cmemory.h"

/* Layout of the baseContext buffer. */
प्रकारनिर्देश संरचना {
    int32_t len;  /* number of bytes available starting at 'data' */
    वर्ण    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
स्थिर ध्रुव int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
स्थिर शून्य* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    सचे (en->baseContext != nullptr) {
        सचे (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           परिमाण(int32_t) + capacity);
            सचे (en->baseContext == nullptr) {
                पटिनिवत्त nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } अञ्ञथा {
        capacity += PAD;
        en->baseContext = uprv_malloc(परिमाण(int32_t) + capacity);
        सचे (en->baseContext == nullptr) {
            पटिनिवत्त nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    पटिनिवत्त static_cast<शून्य*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI शून्य U_EXPORT2
uenum_close(UEnumeration* en)
{
    सचे (en) {
        सचे (en->close != nullptr) {
            सचे (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } अञ्ञथा { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_गणनम्(UEnumeration* en, UErrorCode* status)
{
    सचे (!en || U_FAILURE(*status)) {
        पटिनिवत्त -1;
    }
    सचे (en->count != nullptr) {
        पटिनिवत्त en->count(en, status);
    } अञ्ञथा {
        *status = U_UNSUPPORTED_ERROR;
        पटिनिवत्त -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ध्रुव char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    सचे (en->next != nullptr) {
        ध्रुव वर्ण *cstr = en->next(en, &len, status);
        सचे (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * परिमाण(char16_t));
            सचे (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } अञ्ञथा {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } अञ्ञथा {
        *status = U_UNSUPPORTED_ERROR;
    }
    सचे (resultLength) {
        *resultLength = len;
    }
    पटिनिवत्त ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ध्रुव वर्ण* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    सचे (en->uNext != nullptr) {
        वर्ण *tempCharVal;
        ध्रुव char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        सचे (tempUCharVal == nullptr) {
            पटिनिवत्त nullptr;
        }
        tempCharVal = (वर्ण*)
            _getBuffer(en, (*resultLength+1) * परिमाण(वर्ण));
        सचे (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        पटिनिवत्त tempCharVal;
    } अञ्ञथा {
        *status = U_UNSUPPORTED_ERROR;
        पटिनिवत्त nullptr;
    }
}

U_CAPI ध्रुव char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    सचे (!en || U_FAILURE(*status)) {
        पटिनिवत्त nullptr;
    }
    सचे (en->uNext != nullptr) {
        पटिनिवत्त en->uNext(en, resultLength, status);
    } अञ्ञथा {
        *status = U_UNSUPPORTED_ERROR;
        पटिनिवत्त nullptr;
    }
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    सचे (!en || U_FAILURE(*status)) {
        पटिनिवत्त nullptr;
    }
    सचे (en->next != nullptr) {
        सचे (resultLength != nullptr) {
            पटिनिवत्त en->next(en, resultLength, status);
        }
        अञ्ञथा {
            int32_t dummyLength=0;
            पटिनिवत्त en->next(en, &dummyLength, status);
        }
    } अञ्ञथा {
        *status = U_UNSUPPORTED_ERROR;
        पटिनिवत्त nullptr;
    }
}

U_CAPI शून्य U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    सचे (!en || U_FAILURE(*status)) {
        पटिनिवत्त;
    }
    सचे (en->reset != nullptr) {
        en->reset(en, status);
    } अञ्ञथा {
        *status = U_UNSUPPORTED_ERROR;
    }
}
