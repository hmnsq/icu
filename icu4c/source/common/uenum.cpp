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

    ပေႃး (en->baseContext != nullptr) {
        ပေႃး (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           parimaa_na(int32_t) + capacity);
            ပေႃး (en->baseContext == nullptr) {
                ပွၵ်ႈ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } ပေႃးဢမ်ႇ {
        capacity += PAD;
        en->baseContext = uprv_malloc(parimaa_na(int32_t) + capacity);
        ပေႃး (en->baseContext == nullptr) {
            ပွၵ်ႈ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ပွၵ်ႈ static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    ပေႃး (en) {
        ပေႃး (en->close != nullptr) {
            ပေႃး (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } ပေႃးဢမ်ႇ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    ပေႃး (!en || U_FAILURE(*status)) {
        ပွၵ်ႈ -1;
    }
    ပေႃး (en->count != nullptr) {
        ပွၵ်ႈ en->count(en, status);
    } ပေႃးဢမ်ႇ {
        *status = U_UNSUPPORTED_ERROR;
        ပွၵ်ႈ -1;
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
    ပေႃး (en->next != nullptr) {
        const var_na *cstr = en->next(en, &len, status);
        ပေႃး (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * parimaa_na(char16_t));
            ပေႃး (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } ပေႃးဢမ်ႇ {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ပေႃးဢမ်ႇ {
        *status = U_UNSUPPORTED_ERROR;
    }
    ပေႃး (resultLength) {
        *resultLength = len;
    }
    ပွၵ်ႈ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const var_na* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    ပေႃး (en->uNext != nullptr) {
        var_na *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        ပေႃး (tempUCharVal == nullptr) {
            ပွၵ်ႈ nullptr;
        }
        tempCharVal = (var_na*)
            _getBuffer(en, (*resultLength+1) * parimaa_na(var_na));
        ပေႃး (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ပွၵ်ႈ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ပွၵ်ႈ tempCharVal;
    } ပေႃးဢမ်ႇ {
        *status = U_UNSUPPORTED_ERROR;
        ပွၵ်ႈ nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    ပေႃး (!en || U_FAILURE(*status)) {
        ပွၵ်ႈ nullptr;
    }
    ပေႃး (en->uNext != nullptr) {
        ပွၵ်ႈ en->uNext(en, resultLength, status);
    } ပေႃးဢမ်ႇ {
        *status = U_UNSUPPORTED_ERROR;
        ပွၵ်ႈ nullptr;
    }
}

U_CAPI const var_na* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    ပေႃး (!en || U_FAILURE(*status)) {
        ပွၵ်ႈ nullptr;
    }
    ပေႃး (en->next != nullptr) {
        ပေႃး (resultLength != nullptr) {
            ပွၵ်ႈ en->next(en, resultLength, status);
        }
        ပေႃးဢမ်ႇ {
            int32_t dummyLength=0;
            ပွၵ်ႈ en->next(en, &dummyLength, status);
        }
    } ပေႃးဢမ်ႇ {
        *status = U_UNSUPPORTED_ERROR;
        ပွၵ်ႈ nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    ပေႃး (!en || U_FAILURE(*status)) {
        ပွၵ်ႈ;
    }
    ပေႃး (en->reset != nullptr) {
        en->reset(en, status);
    } ပေႃးဢမ်ႇ {
        *status = U_UNSUPPORTED_ERROR;
    }
}
