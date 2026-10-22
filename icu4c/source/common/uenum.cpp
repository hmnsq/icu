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

#ضمن "unicode/putil.h"
#ضمن "uenumimp.h"
#ضمن "cmemory.h"

/* Layout of the baseContext buffer. */
تعريفنوع بنية {
    int32_t len;  /* number of bytes available starting at 'data' */
    حرف    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
ساكن ثابت int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ساكن فراغ* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    إذا (en->baseContext != nullptr) {
        إذا (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           حجم(int32_t) + capacity);
            إذا (en->baseContext == nullptr) {
                أرجع nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } وإلا {
        capacity += PAD;
        en->baseContext = uprv_malloc(حجم(int32_t) + capacity);
        إذا (en->baseContext == nullptr) {
            أرجع nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    أرجع static_cast<فراغ*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI فراغ U_EXPORT2
uenum_close(UEnumeration* en)
{
    إذا (en) {
        إذا (en->close != nullptr) {
            إذا (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } وإلا { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_عدد(UEnumeration* en, UErrorCode* status)
{
    إذا (!en || U_FAILURE(*status)) {
        أرجع -1;
    }
    إذا (en->count != nullptr) {
        أرجع en->count(en, status);
    } وإلا {
        *status = U_UNSUPPORTED_ERROR;
        أرجع -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ثابت char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    إذا (en->next != nullptr) {
        ثابت حرف *cstr = en->next(en, &len, status);
        إذا (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * حجم(char16_t));
            إذا (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } وإلا {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } وإلا {
        *status = U_UNSUPPORTED_ERROR;
    }
    إذا (resultLength) {
        *resultLength = len;
    }
    أرجع ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ثابت حرف* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    إذا (en->uNext != nullptr) {
        حرف *tempCharVal;
        ثابت char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        إذا (tempUCharVal == nullptr) {
            أرجع nullptr;
        }
        tempCharVal = (حرف*)
            _getBuffer(en, (*resultLength+1) * حجم(حرف));
        إذا (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            أرجع nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        أرجع tempCharVal;
    } وإلا {
        *status = U_UNSUPPORTED_ERROR;
        أرجع nullptr;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    إذا (!en || U_FAILURE(*status)) {
        أرجع nullptr;
    }
    إذا (en->uNext != nullptr) {
        أرجع en->uNext(en, resultLength, status);
    } وإلا {
        *status = U_UNSUPPORTED_ERROR;
        أرجع nullptr;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    إذا (!en || U_FAILURE(*status)) {
        أرجع nullptr;
    }
    إذا (en->next != nullptr) {
        إذا (resultLength != nullptr) {
            أرجع en->next(en, resultLength, status);
        }
        وإلا {
            int32_t dummyLength=0;
            أرجع en->next(en, &dummyLength, status);
        }
    } وإلا {
        *status = U_UNSUPPORTED_ERROR;
        أرجع nullptr;
    }
}

U_CAPI فراغ U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    إذا (!en || U_FAILURE(*status)) {
        أرجع;
    }
    إذا (en->reset != nullptr) {
        en->reset(en, status);
    } وإلا {
        *status = U_UNSUPPORTED_ERROR;
    }
}
