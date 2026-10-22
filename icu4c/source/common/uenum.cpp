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

#شامل "unicode/putil.h"
#شامل "uenumimp.h"
#شامل "cmemory.h"

/* Layout of the baseContext buffer. */
تعریفنوع ساختار {
    int32_t len;  /* number of bytes available starting at 'data' */
    نویسه    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
ایستا ثابت int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ایستا تهی* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    اگر (en->baseContext != nullptr) {
        اگر (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           اندازه(int32_t) + capacity);
            اگر (en->baseContext == nullptr) {
                بازگشت nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } وگرنه {
        capacity += PAD;
        en->baseContext = uprv_malloc(اندازه(int32_t) + capacity);
        اگر (en->baseContext == nullptr) {
            بازگشت nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    بازگشت static_cast<تهی*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI تهی U_EXPORT2
uenum_close(UEnumeration* en)
{
    اگر (en) {
        اگر (en->close != nullptr) {
            اگر (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } وگرنه { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_شمار(UEnumeration* en, UErrorCode* status)
{
    اگر (!en || U_FAILURE(*status)) {
        بازگشت -1;
    }
    اگر (en->count != nullptr) {
        بازگشت en->count(en, status);
    } وگرنه {
        *status = U_UNSUPPORTED_ERROR;
        بازگشت -1;
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
    اگر (en->next != nullptr) {
        ثابت نویسه *cstr = en->next(en, &len, status);
        اگر (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * اندازه(char16_t));
            اگر (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } وگرنه {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } وگرنه {
        *status = U_UNSUPPORTED_ERROR;
    }
    اگر (resultLength) {
        *resultLength = len;
    }
    بازگشت ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ثابت نویسه* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    اگر (en->uNext != nullptr) {
        نویسه *tempCharVal;
        ثابت char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        اگر (tempUCharVal == nullptr) {
            بازگشت nullptr;
        }
        tempCharVal = (نویسه*)
            _getBuffer(en, (*resultLength+1) * اندازه(نویسه));
        اگر (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            بازگشت nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        بازگشت tempCharVal;
    } وگرنه {
        *status = U_UNSUPPORTED_ERROR;
        بازگشت nullptr;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    اگر (!en || U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر (en->uNext != nullptr) {
        بازگشت en->uNext(en, resultLength, status);
    } وگرنه {
        *status = U_UNSUPPORTED_ERROR;
        بازگشت nullptr;
    }
}

U_CAPI ثابت نویسه* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    اگر (!en || U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر (en->next != nullptr) {
        اگر (resultLength != nullptr) {
            بازگشت en->next(en, resultLength, status);
        }
        وگرنه {
            int32_t dummyLength=0;
            بازگشت en->next(en, &dummyLength, status);
        }
    } وگرنه {
        *status = U_UNSUPPORTED_ERROR;
        بازگشت nullptr;
    }
}

U_CAPI تهی U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    اگر (!en || U_FAILURE(*status)) {
        بازگشت;
    }
    اگر (en->reset != nullptr) {
        en->reset(en, status);
    } وگرنه {
        *status = U_UNSUPPORTED_ERROR;
    }
}
