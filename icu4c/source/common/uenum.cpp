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

    ຖ້າ (en->baseContext != nullptr) {
        ຖ້າ (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           parimaa_na(int32_t) + capacity);
            ຖ້າ (en->baseContext == nullptr) {
                ສົ່ງຄືນ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } ບໍ່ດັ່ງນັ້ນ {
        capacity += PAD;
        en->baseContext = uprv_malloc(parimaa_na(int32_t) + capacity);
        ຖ້າ (en->baseContext == nullptr) {
            ສົ່ງຄືນ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ສົ່ງຄືນ static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    ຖ້າ (en) {
        ຖ້າ (en->close != nullptr) {
            ຖ້າ (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } ບໍ່ດັ່ງນັ້ນ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    ຖ້າ (!en || U_FAILURE(*status)) {
        ສົ່ງຄືນ -1;
    }
    ຖ້າ (en->count != nullptr) {
        ສົ່ງຄືນ en->count(en, status);
    } ບໍ່ດັ່ງນັ້ນ {
        *status = U_UNSUPPORTED_ERROR;
        ສົ່ງຄືນ -1;
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
    ຖ້າ (en->next != nullptr) {
        const var_na *cstr = en->next(en, &len, status);
        ຖ້າ (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * parimaa_na(char16_t));
            ຖ້າ (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } ບໍ່ດັ່ງນັ້ນ {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ບໍ່ດັ່ງນັ້ນ {
        *status = U_UNSUPPORTED_ERROR;
    }
    ຖ້າ (resultLength) {
        *resultLength = len;
    }
    ສົ່ງຄືນ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const var_na* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    ຖ້າ (en->uNext != nullptr) {
        var_na *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        ຖ້າ (tempUCharVal == nullptr) {
            ສົ່ງຄືນ nullptr;
        }
        tempCharVal = (var_na*)
            _getBuffer(en, (*resultLength+1) * parimaa_na(var_na));
        ຖ້າ (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ສົ່ງຄືນ tempCharVal;
    } ບໍ່ດັ່ງນັ້ນ {
        *status = U_UNSUPPORTED_ERROR;
        ສົ່ງຄືນ nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    ຖ້າ (!en || U_FAILURE(*status)) {
        ສົ່ງຄືນ nullptr;
    }
    ຖ້າ (en->uNext != nullptr) {
        ສົ່ງຄືນ en->uNext(en, resultLength, status);
    } ບໍ່ດັ່ງນັ້ນ {
        *status = U_UNSUPPORTED_ERROR;
        ສົ່ງຄືນ nullptr;
    }
}

U_CAPI const var_na* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    ຖ້າ (!en || U_FAILURE(*status)) {
        ສົ່ງຄືນ nullptr;
    }
    ຖ້າ (en->next != nullptr) {
        ຖ້າ (resultLength != nullptr) {
            ສົ່ງຄືນ en->next(en, resultLength, status);
        }
        ບໍ່ດັ່ງນັ້ນ {
            int32_t dummyLength=0;
            ສົ່ງຄືນ en->next(en, &dummyLength, status);
        }
    } ບໍ່ດັ່ງນັ້ນ {
        *status = U_UNSUPPORTED_ERROR;
        ສົ່ງຄືນ nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    ຖ້າ (!en || U_FAILURE(*status)) {
        ສົ່ງຄືນ;
    }
    ຖ້າ (en->reset != nullptr) {
        en->reset(en, status);
    } ບໍ່ດັ່ງນັ້ນ {
        *status = U_UNSUPPORTED_ERROR;
    }
}
