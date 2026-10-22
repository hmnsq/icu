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

#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "uenumimp.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"

/* Layout of the baseContext buffer. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    int32_t len;  /* number of bytes available starting at 'data' */
    ଅକ୍ଷର    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
ସ୍ଥିର ଅଚଳ int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ସ୍ଥିର ଶୂନ୍ୟ* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    ଯଦି (en->baseContext != nullptr) {
        ଯଦି (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           ଆକାର(int32_t) + capacity);
            ଯଦି (en->baseContext == nullptr) {
                ଫେରନ୍ତୁ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } ଅନ୍ୟଥା {
        capacity += PAD;
        en->baseContext = uprv_malloc(ଆକାର(int32_t) + capacity);
        ଯଦି (en->baseContext == nullptr) {
            ଫେରନ୍ତୁ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ଫେରନ୍ତୁ static_cast<ଶୂନ୍ୟ*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uenum_close(UEnumeration* en)
{
    ଯଦି (en) {
        ଯଦି (en->close != nullptr) {
            ଯଦି (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } ଅନ୍ୟଥା { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_ଗଣନା(UEnumeration* en, UErrorCode* status)
{
    ଯଦି (!en || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ -1;
    }
    ଯଦି (en->count != nullptr) {
        ଫେରନ୍ତୁ en->count(en, status);
    } ଅନ୍ୟଥା {
        *status = U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ଅଚଳ char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    ଯଦି (en->next != nullptr) {
        ଅଚଳ ଅକ୍ଷର *cstr = en->next(en, &len, status);
        ଯଦି (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * ଆକାର(char16_t));
            ଯଦି (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } ଅନ୍ୟଥା {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ଅନ୍ୟଥା {
        *status = U_UNSUPPORTED_ERROR;
    }
    ଯଦି (resultLength) {
        *resultLength = len;
    }
    ଫେରନ୍ତୁ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    ଯଦି (en->uNext != nullptr) {
        ଅକ୍ଷର *tempCharVal;
        ଅଚଳ char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        ଯଦି (tempUCharVal == nullptr) {
            ଫେରନ୍ତୁ nullptr;
        }
        tempCharVal = (ଅକ୍ଷର*)
            _getBuffer(en, (*resultLength+1) * ଆକାର(ଅକ୍ଷର));
        ଯଦି (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ଫେରନ୍ତୁ tempCharVal;
    } ଅନ୍ୟଥା {
        *status = U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
}

U_CAPI ଅଚଳ char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    ଯଦି (!en || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (en->uNext != nullptr) {
        ଫେରନ୍ତୁ en->uNext(en, resultLength, status);
    } ଅନ୍ୟଥା {
        *status = U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    ଯଦି (!en || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି (en->next != nullptr) {
        ଯଦି (resultLength != nullptr) {
            ଫେରନ୍ତୁ en->next(en, resultLength, status);
        }
        ଅନ୍ୟଥା {
            int32_t dummyLength=0;
            ଫେରନ୍ତୁ en->next(en, &dummyLength, status);
        }
    } ଅନ୍ୟଥା {
        *status = U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    ଯଦି (!en || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (en->reset != nullptr) {
        en->reset(en, status);
    } ଅନ୍ୟଥା {
        *status = U_UNSUPPORTED_ERROR;
    }
}
