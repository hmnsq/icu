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

#include "unicode/putil.h"
#include "uenumimp.h"
#include "cmemory.h"

/* Layout of the baseContext buffer. */
typedef struct {
    int32_t len;  /* number of bytes available starting at 'data' */
    char    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
static const int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
static void* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    если (en->baseContext != nullptr) {
        если (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           sizeof(int32_t) + capacity);
            если (en->baseContext == nullptr) {
                вернуть nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } иначе {
        capacity += PAD;
        en->baseContext = uprv_malloc(sizeof(int32_t) + capacity);
        если (en->baseContext == nullptr) {
            вернуть nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    вернуть static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    если (en) {
        если (en->close != nullptr) {
            если (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } иначе { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    если (!en || U_FAILURE(*status)) {
        вернуть -1;
    }
    если (en->count != nullptr) {
        вернуть en->count(en, status);
    } иначе {
        *status = U_UNSUPPORTED_ERROR;
        вернуть -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI const char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    если (en->next != nullptr) {
        const char *cstr = en->next(en, &len, status);
        если (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * sizeof(char16_t));
            если (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } иначе {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } иначе {
        *status = U_UNSUPPORTED_ERROR;
    }
    если (resultLength) {
        *resultLength = len;
    }
    вернуть ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const char* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    если (en->uNext != nullptr) {
        char *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        если (tempUCharVal == nullptr) {
            вернуть nullptr;
        }
        tempCharVal = (char*)
            _getBuffer(en, (*resultLength+1) * sizeof(char));
        если (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            вернуть nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        вернуть tempCharVal;
    } иначе {
        *status = U_UNSUPPORTED_ERROR;
        вернуть nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    если (!en || U_FAILURE(*status)) {
        вернуть nullptr;
    }
    если (en->uNext != nullptr) {
        вернуть en->uNext(en, resultLength, status);
    } иначе {
        *status = U_UNSUPPORTED_ERROR;
        вернуть nullptr;
    }
}

U_CAPI const char* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    если (!en || U_FAILURE(*status)) {
        вернуть nullptr;
    }
    если (en->next != nullptr) {
        если (resultLength != nullptr) {
            вернуть en->next(en, resultLength, status);
        }
        иначе {
            int32_t dummyLength=0;
            вернуть en->next(en, &dummyLength, status);
        }
    } иначе {
        *status = U_UNSUPPORTED_ERROR;
        вернуть nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    если (!en || U_FAILURE(*status)) {
        вернуть;
    }
    если (en->reset != nullptr) {
        en->reset(en, status);
    } иначе {
        *status = U_UNSUPPORTED_ERROR;
    }
}
