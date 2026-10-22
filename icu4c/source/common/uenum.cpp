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

    nếu (en->baseContext != nullptr) {
        nếu (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           sizeof(int32_t) + capacity);
            nếu (en->baseContext == nullptr) {
                trả_về nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } không_thì {
        capacity += PAD;
        en->baseContext = uprv_malloc(sizeof(int32_t) + capacity);
        nếu (en->baseContext == nullptr) {
            trả_về nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    trả_về static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    nếu (en) {
        nếu (en->close != nullptr) {
            nếu (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } không_thì { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    nếu (!en || U_FAILURE(*status)) {
        trả_về -1;
    }
    nếu (en->count != nullptr) {
        trả_về en->count(en, status);
    } không_thì {
        *status = U_UNSUPPORTED_ERROR;
        trả_về -1;
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
    nếu (en->next != nullptr) {
        const char *cstr = en->next(en, &len, status);
        nếu (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * sizeof(char16_t));
            nếu (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } không_thì {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } không_thì {
        *status = U_UNSUPPORTED_ERROR;
    }
    nếu (resultLength) {
        *resultLength = len;
    }
    trả_về ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const char* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    nếu (en->uNext != nullptr) {
        char *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        nếu (tempUCharVal == nullptr) {
            trả_về nullptr;
        }
        tempCharVal = (char*)
            _getBuffer(en, (*resultLength+1) * sizeof(char));
        nếu (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            trả_về nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        trả_về tempCharVal;
    } không_thì {
        *status = U_UNSUPPORTED_ERROR;
        trả_về nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    nếu (!en || U_FAILURE(*status)) {
        trả_về nullptr;
    }
    nếu (en->uNext != nullptr) {
        trả_về en->uNext(en, resultLength, status);
    } không_thì {
        *status = U_UNSUPPORTED_ERROR;
        trả_về nullptr;
    }
}

U_CAPI const char* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    nếu (!en || U_FAILURE(*status)) {
        trả_về nullptr;
    }
    nếu (en->next != nullptr) {
        nếu (resultLength != nullptr) {
            trả_về en->next(en, resultLength, status);
        }
        không_thì {
            int32_t dummyLength=0;
            trả_về en->next(en, &dummyLength, status);
        }
    } không_thì {
        *status = U_UNSUPPORTED_ERROR;
        trả_về nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    nếu (!en || U_FAILURE(*status)) {
        trả_về;
    }
    nếu (en->reset != nullptr) {
        en->reset(en, status);
    } không_thì {
        *status = U_UNSUPPORTED_ERROR;
    }
}
