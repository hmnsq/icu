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
typedef sa_nracanaa {
    int32_t len;  /* number of bytes available starting at 'data' */
    var_na    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
static const int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
static void* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    ꦪꦼꦤ꧀ (en->baseContext != nullptr) {
        ꦪꦼꦤ꧀ (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           parimaa_na(int32_t) + capacity);
            ꦪꦼꦤ꧀ (en->baseContext == nullptr) {
                ꦮꦭꦶ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } ꦭꦶꦪ {
        capacity += PAD;
        en->baseContext = uprv_malloc(parimaa_na(int32_t) + capacity);
        ꦪꦼꦤ꧀ (en->baseContext == nullptr) {
            ꦮꦭꦶ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ꦮꦭꦶ static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    ꦪꦼꦤ꧀ (en) {
        ꦪꦼꦤ꧀ (en->close != nullptr) {
            ꦪꦼꦤ꧀ (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } ꦭꦶꦪ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    ꦪꦼꦤ꧀ (!en || U_FAILURE(*status)) {
        ꦮꦭꦶ -1;
    }
    ꦪꦼꦤ꧀ (en->count != nullptr) {
        ꦮꦭꦶ en->count(en, status);
    } ꦭꦶꦪ {
        *status = U_UNSUPPORTED_ERROR;
        ꦮꦭꦶ -1;
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
    ꦪꦼꦤ꧀ (en->next != nullptr) {
        const var_na *cstr = en->next(en, &len, status);
        ꦪꦼꦤ꧀ (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * parimaa_na(char16_t));
            ꦪꦼꦤ꧀ (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } ꦭꦶꦪ {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ꦭꦶꦪ {
        *status = U_UNSUPPORTED_ERROR;
    }
    ꦪꦼꦤ꧀ (resultLength) {
        *resultLength = len;
    }
    ꦮꦭꦶ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const var_na* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    ꦪꦼꦤ꧀ (en->uNext != nullptr) {
        var_na *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        ꦪꦼꦤ꧀ (tempUCharVal == nullptr) {
            ꦮꦭꦶ nullptr;
        }
        tempCharVal = (var_na*)
            _getBuffer(en, (*resultLength+1) * parimaa_na(var_na));
        ꦪꦼꦤ꧀ (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ꦮꦭꦶ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ꦮꦭꦶ tempCharVal;
    } ꦭꦶꦪ {
        *status = U_UNSUPPORTED_ERROR;
        ꦮꦭꦶ nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    ꦪꦼꦤ꧀ (!en || U_FAILURE(*status)) {
        ꦮꦭꦶ nullptr;
    }
    ꦪꦼꦤ꧀ (en->uNext != nullptr) {
        ꦮꦭꦶ en->uNext(en, resultLength, status);
    } ꦭꦶꦪ {
        *status = U_UNSUPPORTED_ERROR;
        ꦮꦭꦶ nullptr;
    }
}

U_CAPI const var_na* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    ꦪꦼꦤ꧀ (!en || U_FAILURE(*status)) {
        ꦮꦭꦶ nullptr;
    }
    ꦪꦼꦤ꧀ (en->next != nullptr) {
        ꦪꦼꦤ꧀ (resultLength != nullptr) {
            ꦮꦭꦶ en->next(en, resultLength, status);
        }
        ꦭꦶꦪ {
            int32_t dummyLength=0;
            ꦮꦭꦶ en->next(en, &dummyLength, status);
        }
    } ꦭꦶꦪ {
        *status = U_UNSUPPORTED_ERROR;
        ꦮꦭꦶ nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    ꦪꦼꦤ꧀ (!en || U_FAILURE(*status)) {
        ꦮꦭꦶ;
    }
    ꦪꦼꦤ꧀ (en->reset != nullptr) {
        en->reset(en, status);
    } ꦭꦶꦪ {
        *status = U_UNSUPPORTED_ERROR;
    }
}
