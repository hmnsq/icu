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

#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "uenumimp.h"
#அந்தர்பாவ "cmemory.h"

/* Layout of the baseContext buffer. */
வகைவரை கட்டமைப்பு {
    int32_t len;  /* number of bytes available starting at 'data' */
    எழுத்து    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
நிலையான மாறிலி int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
நிலையான வெறுமை* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    எனில் (en->baseContext != nullptr) {
        எனில் (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           அளவு(int32_t) + capacity);
            எனில் (en->baseContext == nullptr) {
                திரும்பு nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } இல்லையெனில் {
        capacity += PAD;
        en->baseContext = uprv_malloc(அளவு(int32_t) + capacity);
        எனில் (en->baseContext == nullptr) {
            திரும்பு nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    திரும்பு static_cast<வெறுமை*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI வெறுமை U_EXPORT2
uenum_close(UEnumeration* en)
{
    எனில் (en) {
        எனில் (en->close != nullptr) {
            எனில் (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } இல்லையெனில் { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_மொத்தம்(UEnumeration* en, UErrorCode* status)
{
    எனில் (!en || U_FAILURE(*status)) {
        திரும்பு -1;
    }
    எனில் (en->count != nullptr) {
        திரும்பு en->count(en, status);
    } இல்லையெனில் {
        *status = U_UNSUPPORTED_ERROR;
        திரும்பு -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI மாறிலி char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    எனில் (en->next != nullptr) {
        மாறிலி எழுத்து *cstr = en->next(en, &len, status);
        எனில் (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * அளவு(char16_t));
            எனில் (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } இல்லையெனில் {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } இல்லையெனில் {
        *status = U_UNSUPPORTED_ERROR;
    }
    எனில் (resultLength) {
        *resultLength = len;
    }
    திரும்பு ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI மாறிலி எழுத்து* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    எனில் (en->uNext != nullptr) {
        எழுத்து *tempCharVal;
        மாறிலி char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        எனில் (tempUCharVal == nullptr) {
            திரும்பு nullptr;
        }
        tempCharVal = (எழுத்து*)
            _getBuffer(en, (*resultLength+1) * அளவு(எழுத்து));
        எனில் (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        திரும்பு tempCharVal;
    } இல்லையெனில் {
        *status = U_UNSUPPORTED_ERROR;
        திரும்பு nullptr;
    }
}

U_CAPI மாறிலி char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    எனில் (!en || U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில் (en->uNext != nullptr) {
        திரும்பு en->uNext(en, resultLength, status);
    } இல்லையெனில் {
        *status = U_UNSUPPORTED_ERROR;
        திரும்பு nullptr;
    }
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    எனில் (!en || U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில் (en->next != nullptr) {
        எனில் (resultLength != nullptr) {
            திரும்பு en->next(en, resultLength, status);
        }
        இல்லையெனில் {
            int32_t dummyLength=0;
            திரும்பு en->next(en, &dummyLength, status);
        }
    } இல்லையெனில் {
        *status = U_UNSUPPORTED_ERROR;
        திரும்பு nullptr;
    }
}

U_CAPI வெறுமை U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    எனில் (!en || U_FAILURE(*status)) {
        திரும்பு;
    }
    எனில் (en->reset != nullptr) {
        en->reset(en, status);
    } இல்லையெனில் {
        *status = U_UNSUPPORTED_ERROR;
    }
}
