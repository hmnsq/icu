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

#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

/* Layout of the baseContext buffer. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    int32_t len;  /* number of bytes available starting at 'data' */
    ವರ್ಣ    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
ಸ್ಥಿರ ಧ್ರುವ int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ಸ್ಥಿರ ಶೂನ್ಯ* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    ಯದಿ (en->baseContext != nullptr) {
        ಯದಿ (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           ಪರಿಮಾಣ(int32_t) + capacity);
            ಯದಿ (en->baseContext == nullptr) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } ಅನ್ಯಥಾ {
        capacity += PAD;
        en->baseContext = uprv_malloc(ಪರಿಮಾಣ(int32_t) + capacity);
        ಯದಿ (en->baseContext == nullptr) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ಪ್ರತ್ಯಾಗಮ static_cast<ಶೂನ್ಯ*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uenum_close(UEnumeration* en)
{
    ಯದಿ (en) {
        ಯದಿ (en->close != nullptr) {
            ಯದಿ (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } ಅನ್ಯಥಾ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_ಎಣಿಕೆ(UEnumeration* en, UErrorCode* status)
{
    ಯದಿ (!en || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (en->count != nullptr) {
        ಪ್ರತ್ಯಾಗಮ en->count(en, status);
    } ಅನ್ಯಥಾ {
        *status = U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    ಯದಿ (en->next != nullptr) {
        ಧ್ರುವ ವರ್ಣ *cstr = en->next(en, &len, status);
        ಯದಿ (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * ಪರಿಮಾಣ(char16_t));
            ಯದಿ (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } ಅನ್ಯಥಾ {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ಅನ್ಯಥಾ {
        *status = U_UNSUPPORTED_ERROR;
    }
    ಯದಿ (resultLength) {
        *resultLength = len;
    }
    ಪ್ರತ್ಯಾಗಮ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    ಯದಿ (en->uNext != nullptr) {
        ವರ್ಣ *tempCharVal;
        ಧ್ರುವ char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        ಯದಿ (tempUCharVal == nullptr) {
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        tempCharVal = (ವರ್ಣ*)
            _getBuffer(en, (*resultLength+1) * ಪರಿಮಾಣ(ವರ್ಣ));
        ಯದಿ (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ಪ್ರತ್ಯಾಗಮ tempCharVal;
    } ಅನ್ಯಥಾ {
        *status = U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    ಯದಿ (!en || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (en->uNext != nullptr) {
        ಪ್ರತ್ಯಾಗಮ en->uNext(en, resultLength, status);
    } ಅನ್ಯಥಾ {
        *status = U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    ಯದಿ (!en || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ (en->next != nullptr) {
        ಯದಿ (resultLength != nullptr) {
            ಪ್ರತ್ಯಾಗಮ en->next(en, resultLength, status);
        }
        ಅನ್ಯಥಾ {
            int32_t dummyLength=0;
            ಪ್ರತ್ಯಾಗಮ en->next(en, &dummyLength, status);
        }
    } ಅನ್ಯಥಾ {
        *status = U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    ಯದಿ (!en || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (en->reset != nullptr) {
        en->reset(en, status);
    } ಅನ್ಯಥಾ {
        *status = U_UNSUPPORTED_ERROR;
    }
}
