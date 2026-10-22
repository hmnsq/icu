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

    만약 (en->baseContext != nullptr) {
        만약 (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           sizeof(int32_t) + capacity);
            만약 (en->baseContext == nullptr) {
                반환 nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } 아니면 {
        capacity += PAD;
        en->baseContext = uprv_malloc(sizeof(int32_t) + capacity);
        만약 (en->baseContext == nullptr) {
            반환 nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    반환 static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    만약 (en) {
        만약 (en->close != nullptr) {
            만약 (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } 아니면 { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    만약 (!en || U_FAILURE(*status)) {
        반환 -1;
    }
    만약 (en->count != nullptr) {
        반환 en->count(en, status);
    } 아니면 {
        *status = U_UNSUPPORTED_ERROR;
        반환 -1;
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
    만약 (en->next != nullptr) {
        const char *cstr = en->next(en, &len, status);
        만약 (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * sizeof(char16_t));
            만약 (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } 아니면 {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } 아니면 {
        *status = U_UNSUPPORTED_ERROR;
    }
    만약 (resultLength) {
        *resultLength = len;
    }
    반환 ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const char* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    만약 (en->uNext != nullptr) {
        char *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        만약 (tempUCharVal == nullptr) {
            반환 nullptr;
        }
        tempCharVal = (char*)
            _getBuffer(en, (*resultLength+1) * sizeof(char));
        만약 (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            반환 nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        반환 tempCharVal;
    } 아니면 {
        *status = U_UNSUPPORTED_ERROR;
        반환 nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    만약 (!en || U_FAILURE(*status)) {
        반환 nullptr;
    }
    만약 (en->uNext != nullptr) {
        반환 en->uNext(en, resultLength, status);
    } 아니면 {
        *status = U_UNSUPPORTED_ERROR;
        반환 nullptr;
    }
}

U_CAPI const char* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    만약 (!en || U_FAILURE(*status)) {
        반환 nullptr;
    }
    만약 (en->next != nullptr) {
        만약 (resultLength != nullptr) {
            반환 en->next(en, resultLength, status);
        }
        아니면 {
            int32_t dummyLength=0;
            반환 en->next(en, &dummyLength, status);
        }
    } 아니면 {
        *status = U_UNSUPPORTED_ERROR;
        반환 nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    만약 (!en || U_FAILURE(*status)) {
        반환;
    }
    만약 (en->reset != nullptr) {
        en->reset(en, status);
    } 아니면 {
        *status = U_UNSUPPORTED_ERROR;
    }
}
