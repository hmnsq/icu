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

    ប្រសិនបើ (en->baseContext != nullptr) {
        ប្រសិនបើ (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           parimaa_na(int32_t) + capacity);
            ប្រសិនបើ (en->baseContext == nullptr) {
                ត្រឡប់ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } បើមិនដូច្នេះ {
        capacity += PAD;
        en->baseContext = uprv_malloc(parimaa_na(int32_t) + capacity);
        ប្រសិនបើ (en->baseContext == nullptr) {
            ត្រឡប់ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ត្រឡប់ static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    ប្រសិនបើ (en) {
        ប្រសិនបើ (en->close != nullptr) {
            ប្រសិនបើ (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } បើមិនដូច្នេះ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    ប្រសិនបើ (!en || U_FAILURE(*status)) {
        ត្រឡប់ -1;
    }
    ប្រសិនបើ (en->count != nullptr) {
        ត្រឡប់ en->count(en, status);
    } បើមិនដូច្នេះ {
        *status = U_UNSUPPORTED_ERROR;
        ត្រឡប់ -1;
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
    ប្រសិនបើ (en->next != nullptr) {
        const var_na *cstr = en->next(en, &len, status);
        ប្រសិនបើ (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * parimaa_na(char16_t));
            ប្រសិនបើ (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } បើមិនដូច្នេះ {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } បើមិនដូច្នេះ {
        *status = U_UNSUPPORTED_ERROR;
    }
    ប្រសិនបើ (resultLength) {
        *resultLength = len;
    }
    ត្រឡប់ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const var_na* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    ប្រសិនបើ (en->uNext != nullptr) {
        var_na *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        ប្រសិនបើ (tempUCharVal == nullptr) {
            ត្រឡប់ nullptr;
        }
        tempCharVal = (var_na*)
            _getBuffer(en, (*resultLength+1) * parimaa_na(var_na));
        ប្រសិនបើ (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ត្រឡប់ tempCharVal;
    } បើមិនដូច្នេះ {
        *status = U_UNSUPPORTED_ERROR;
        ត្រឡប់ nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    ប្រសិនបើ (!en || U_FAILURE(*status)) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ (en->uNext != nullptr) {
        ត្រឡប់ en->uNext(en, resultLength, status);
    } បើមិនដូច្នេះ {
        *status = U_UNSUPPORTED_ERROR;
        ត្រឡប់ nullptr;
    }
}

U_CAPI const var_na* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    ប្រសិនបើ (!en || U_FAILURE(*status)) {
        ត្រឡប់ nullptr;
    }
    ប្រសិនបើ (en->next != nullptr) {
        ប្រសិនបើ (resultLength != nullptr) {
            ត្រឡប់ en->next(en, resultLength, status);
        }
        បើមិនដូច្នេះ {
            int32_t dummyLength=0;
            ត្រឡប់ en->next(en, &dummyLength, status);
        }
    } បើមិនដូច្នេះ {
        *status = U_UNSUPPORTED_ERROR;
        ត្រឡប់ nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    ប្រសិនបើ (!en || U_FAILURE(*status)) {
        ត្រឡប់;
    }
    ប្រសិនបើ (en->reset != nullptr) {
        en->reset(en, status);
    } បើមិនដូច្នេះ {
        *status = U_UNSUPPORTED_ERROR;
    }
}
