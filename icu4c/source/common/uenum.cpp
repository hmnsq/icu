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

#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "uenumimp.h"
#অন্তর্ভাৱ "cmemory.h"

/* Layout of the baseContext buffer. */
প্রকারনির্দেশ সংরচনা {
    int32_t len;  /* number of bytes available starting at 'data' */
    ৱর্ণ    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
স্থির ধ্রুৱ int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
স্থির শূন্য* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    যদি (en->baseContext != nullptr) {
        যদি (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           পরিমাণ(int32_t) + capacity);
            যদি (en->baseContext == nullptr) {
                ঘূৰাই nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } অন্যথা {
        capacity += PAD;
        en->baseContext = uprv_malloc(পরিমাণ(int32_t) + capacity);
        যদি (en->baseContext == nullptr) {
            ঘূৰাই nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    ঘূৰাই static_cast<শূন্য*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI শূন্য U_EXPORT2
uenum_close(UEnumeration* en)
{
    যদি (en) {
        যদি (en->close != nullptr) {
            যদি (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } অন্যথা { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_গুনতি(UEnumeration* en, UErrorCode* status)
{
    যদি (!en || U_FAILURE(*status)) {
        ঘূৰাই -1;
    }
    যদি (en->count != nullptr) {
        ঘূৰাই en->count(en, status);
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
        ঘূৰাই -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ধ্রুৱ char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    যদি (en->next != nullptr) {
        ধ্রুৱ ৱর্ণ *cstr = en->next(en, &len, status);
        যদি (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * পরিমাণ(char16_t));
            যদি (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } অন্যথা {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
    }
    যদি (resultLength) {
        *resultLength = len;
    }
    ঘূৰাই ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    যদি (en->uNext != nullptr) {
        ৱর্ণ *tempCharVal;
        ধ্রুৱ char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        যদি (tempUCharVal == nullptr) {
            ঘূৰাই nullptr;
        }
        tempCharVal = (ৱর্ণ*)
            _getBuffer(en, (*resultLength+1) * পরিমাণ(ৱর্ণ));
        যদি (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        ঘূৰাই tempCharVal;
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
        ঘূৰাই nullptr;
    }
}

U_CAPI ধ্রুৱ char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    যদি (!en || U_FAILURE(*status)) {
        ঘূৰাই nullptr;
    }
    যদি (en->uNext != nullptr) {
        ঘূৰাই en->uNext(en, resultLength, status);
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
        ঘূৰাই nullptr;
    }
}

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    যদি (!en || U_FAILURE(*status)) {
        ঘূৰাই nullptr;
    }
    যদি (en->next != nullptr) {
        যদি (resultLength != nullptr) {
            ঘূৰাই en->next(en, resultLength, status);
        }
        অন্যথা {
            int32_t dummyLength=0;
            ঘূৰাই en->next(en, &dummyLength, status);
        }
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
        ঘূৰাই nullptr;
    }
}

U_CAPI শূন্য U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    যদি (!en || U_FAILURE(*status)) {
        ঘূৰাই;
    }
    যদি (en->reset != nullptr) {
        en->reset(en, status);
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
    }
}
