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

#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "cmemory.h"

/* Layout of the baseContext buffer. */
प्रकारनिर्देश संरचना {
    int32_t len;  /* number of bytes available starting at 'data' */
    वर्ण    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
स्थिर ध्रुव int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
स्थिर शून्य* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    जइ (en->baseContext != nullptr) {
        जइ (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           परिमाण(int32_t) + capacity);
            जइ (en->baseContext == nullptr) {
                प्रत्यागम nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } अण्णहा {
        capacity += PAD;
        en->baseContext = uprv_malloc(परिमाण(int32_t) + capacity);
        जइ (en->baseContext == nullptr) {
            प्रत्यागम nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    प्रत्यागम static_cast<शून्य*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI शून्य U_EXPORT2
uenum_close(UEnumeration* en)
{
    जइ (en) {
        जइ (en->close != nullptr) {
            जइ (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } अण्णहा { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_गणनम्(UEnumeration* en, UErrorCode* status)
{
    जइ (!en || U_FAILURE(*status)) {
        प्रत्यागम -1;
    }
    जइ (en->count != nullptr) {
        प्रत्यागम en->count(en, status);
    } अण्णहा {
        *status = U_UNSUPPORTED_ERROR;
        प्रत्यागम -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ध्रुव char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    जइ (en->next != nullptr) {
        ध्रुव वर्ण *cstr = en->next(en, &len, status);
        जइ (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * परिमाण(char16_t));
            जइ (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } अण्णहा {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } अण्णहा {
        *status = U_UNSUPPORTED_ERROR;
    }
    जइ (resultLength) {
        *resultLength = len;
    }
    प्रत्यागम ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ध्रुव वर्ण* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    जइ (en->uNext != nullptr) {
        वर्ण *tempCharVal;
        ध्रुव char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        जइ (tempUCharVal == nullptr) {
            प्रत्यागम nullptr;
        }
        tempCharVal = (वर्ण*)
            _getBuffer(en, (*resultLength+1) * परिमाण(वर्ण));
        जइ (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        प्रत्यागम tempCharVal;
    } अण्णहा {
        *status = U_UNSUPPORTED_ERROR;
        प्रत्यागम nullptr;
    }
}

U_CAPI ध्रुव char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    जइ (!en || U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ (en->uNext != nullptr) {
        प्रत्यागम en->uNext(en, resultLength, status);
    } अण्णहा {
        *status = U_UNSUPPORTED_ERROR;
        प्रत्यागम nullptr;
    }
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    जइ (!en || U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    जइ (en->next != nullptr) {
        जइ (resultLength != nullptr) {
            प्रत्यागम en->next(en, resultLength, status);
        }
        अण्णहा {
            int32_t dummyLength=0;
            प्रत्यागम en->next(en, &dummyLength, status);
        }
    } अण्णहा {
        *status = U_UNSUPPORTED_ERROR;
        प्रत्यागम nullptr;
    }
}

U_CAPI शून्य U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    जइ (!en || U_FAILURE(*status)) {
        प्रत्यागम;
    }
    जइ (en->reset != nullptr) {
        en->reset(en, status);
    } अण्णहा {
        *status = U_UNSUPPORTED_ERROR;
    }
}
