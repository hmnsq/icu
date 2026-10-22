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

    ܐܢ (en->baseContext != nullptr) {
        ܐܢ (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           حجم(int32_t) + capacity);
            ܐܢ (en->baseContext == nullptr) {
                ܦܢܐ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } ܐܚܪܢܐ {
        capacity += PAD;
        en->baseContext = uprv_malloc(حجم(int32_t) + capacity);
        ܐܢ (en->baseContext == nullptr) {
            ܦܢܐ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ܦܢܐ static_cast<فراغ*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI فراغ U_EXPORT2
uenum_close(UEnumeration* en)
{
    ܐܢ (en) {
        ܐܢ (en->close != nullptr) {
            ܐܢ (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } ܐܚܪܢܐ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    ܐܢ (!en || U_FAILURE(*status)) {
        ܦܢܐ -1;
    }
    ܐܢ (en->count != nullptr) {
        ܦܢܐ en->count(en, status);
    } ܐܚܪܢܐ {
        *status = U_UNSUPPORTED_ERROR;
        ܦܢܐ -1;
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
    ܐܢ (en->next != nullptr) {
        ثابت حرف *cstr = en->next(en, &len, status);
        ܐܢ (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * حجم(char16_t));
            ܐܢ (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } ܐܚܪܢܐ {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ܐܚܪܢܐ {
        *status = U_UNSUPPORTED_ERROR;
    }
    ܐܢ (resultLength) {
        *resultLength = len;
    }
    ܦܢܐ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ثابت حرف* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    ܐܢ (en->uNext != nullptr) {
        حرف *tempCharVal;
        ثابت char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        ܐܢ (tempUCharVal == nullptr) {
            ܦܢܐ nullptr;
        }
        tempCharVal = (حرف*)
            _getBuffer(en, (*resultLength+1) * حجم(حرف));
        ܐܢ (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ܦܢܐ tempCharVal;
    } ܐܚܪܢܐ {
        *status = U_UNSUPPORTED_ERROR;
        ܦܢܐ nullptr;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    ܐܢ (!en || U_FAILURE(*status)) {
        ܦܢܐ nullptr;
    }
    ܐܢ (en->uNext != nullptr) {
        ܦܢܐ en->uNext(en, resultLength, status);
    } ܐܚܪܢܐ {
        *status = U_UNSUPPORTED_ERROR;
        ܦܢܐ nullptr;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    ܐܢ (!en || U_FAILURE(*status)) {
        ܦܢܐ nullptr;
    }
    ܐܢ (en->next != nullptr) {
        ܐܢ (resultLength != nullptr) {
            ܦܢܐ en->next(en, resultLength, status);
        }
        ܐܚܪܢܐ {
            int32_t dummyLength=0;
            ܦܢܐ en->next(en, &dummyLength, status);
        }
    } ܐܚܪܢܐ {
        *status = U_UNSUPPORTED_ERROR;
        ܦܢܐ nullptr;
    }
}

U_CAPI فراغ U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    ܐܢ (!en || U_FAILURE(*status)) {
        ܦܢܐ;
    }
    ܐܢ (en->reset != nullptr) {
        en->reset(en, status);
    } ܐܚܪܢܐ {
        *status = U_UNSUPPORTED_ERROR;
    }
}
