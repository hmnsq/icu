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

    གལ་ཏེ (en->baseContext != nullptr) {
        གལ་ཏེ (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           parimaa_na(int32_t) + capacity);
            གལ་ཏེ (en->baseContext == nullptr) {
                ཕྱིར nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } དེ་མིན {
        capacity += PAD;
        en->baseContext = uprv_malloc(parimaa_na(int32_t) + capacity);
        གལ་ཏེ (en->baseContext == nullptr) {
            ཕྱིར nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ཕྱིར static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    གལ་ཏེ (en) {
        གལ་ཏེ (en->close != nullptr) {
            གལ་ཏེ (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } དེ་མིན { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    གལ་ཏེ (!en || U_FAILURE(*status)) {
        ཕྱིར -1;
    }
    གལ་ཏེ (en->count != nullptr) {
        ཕྱིར en->count(en, status);
    } དེ་མིན {
        *status = U_UNSUPPORTED_ERROR;
        ཕྱིར -1;
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
    གལ་ཏེ (en->next != nullptr) {
        const var_na *cstr = en->next(en, &len, status);
        གལ་ཏེ (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * parimaa_na(char16_t));
            གལ་ཏེ (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } དེ་མིན {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } དེ་མིན {
        *status = U_UNSUPPORTED_ERROR;
    }
    གལ་ཏེ (resultLength) {
        *resultLength = len;
    }
    ཕྱིར ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const var_na* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    གལ་ཏེ (en->uNext != nullptr) {
        var_na *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        གལ་ཏེ (tempUCharVal == nullptr) {
            ཕྱིར nullptr;
        }
        tempCharVal = (var_na*)
            _getBuffer(en, (*resultLength+1) * parimaa_na(var_na));
        གལ་ཏེ (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ཕྱིར nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ཕྱིར tempCharVal;
    } དེ་མིན {
        *status = U_UNSUPPORTED_ERROR;
        ཕྱིར nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    གལ་ཏེ (!en || U_FAILURE(*status)) {
        ཕྱིར nullptr;
    }
    གལ་ཏེ (en->uNext != nullptr) {
        ཕྱིར en->uNext(en, resultLength, status);
    } དེ་མིན {
        *status = U_UNSUPPORTED_ERROR;
        ཕྱིར nullptr;
    }
}

U_CAPI const var_na* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    གལ་ཏེ (!en || U_FAILURE(*status)) {
        ཕྱིར nullptr;
    }
    གལ་ཏེ (en->next != nullptr) {
        གལ་ཏེ (resultLength != nullptr) {
            ཕྱིར en->next(en, resultLength, status);
        }
        དེ་མིན {
            int32_t dummyLength=0;
            ཕྱིར en->next(en, &dummyLength, status);
        }
    } དེ་མིན {
        *status = U_UNSUPPORTED_ERROR;
        ཕྱིར nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    གལ་ཏེ (!en || U_FAILURE(*status)) {
        ཕྱིར;
    }
    གལ་ཏེ (en->reset != nullptr) {
        en->reset(en, status);
    } དེ་མིན {
        *status = U_UNSUPPORTED_ERROR;
    }
}
