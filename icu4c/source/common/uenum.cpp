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
प्रकारव्याख्या संरचना {
    int32_t len;  /* number of bytes available starting at 'data' */
    अक्षर    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
स्थिर स्थिरांक int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
स्थिर शून्य* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    जर (en->baseContext != nullptr) {
        जर (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           आकार(int32_t) + capacity);
            जर (en->baseContext == nullptr) {
                परत nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } अन्यथा {
        capacity += PAD;
        en->baseContext = uprv_malloc(आकार(int32_t) + capacity);
        जर (en->baseContext == nullptr) {
            परत nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    परत static_cast<शून्य*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI शून्य U_EXPORT2
uenum_close(UEnumeration* en)
{
    जर (en) {
        जर (en->बंद != nullptr) {
            जर (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->बंद(en);
        } अन्यथा { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_मोजणी(UEnumeration* en, UErrorCode* status)
{
    जर (!en || U_FAILURE(*status)) {
        परत -1;
    }
    जर (en->count != nullptr) {
        परत en->count(en, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        परत -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI स्थिरांक char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    जर (en->next != nullptr) {
        स्थिरांक अक्षर *cstr = en->next(en, &len, status);
        जर (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * आकार(char16_t));
            जर (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } अन्यथा {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
    }
    जर (resultLength) {
        *resultLength = len;
    }
    परत ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI स्थिरांक अक्षर* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    जर (en->uNext != nullptr) {
        अक्षर *tempCharVal;
        स्थिरांक char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        जर (tempUCharVal == nullptr) {
            परत nullptr;
        }
        tempCharVal = (अक्षर*)
            _getBuffer(en, (*resultLength+1) * आकार(अक्षर));
        जर (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            परत nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        परत tempCharVal;
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        परत nullptr;
    }
}

U_CAPI स्थिरांक char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    जर (!en || U_FAILURE(*status)) {
        परत nullptr;
    }
    जर (en->uNext != nullptr) {
        परत en->uNext(en, resultLength, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        परत nullptr;
    }
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    जर (!en || U_FAILURE(*status)) {
        परत nullptr;
    }
    जर (en->next != nullptr) {
        जर (resultLength != nullptr) {
            परत en->next(en, resultLength, status);
        }
        अन्यथा {
            int32_t dummyLength=0;
            परत en->next(en, &dummyLength, status);
        }
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        परत nullptr;
    }
}

U_CAPI शून्य U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    जर (!en || U_FAILURE(*status)) {
        परत;
    }
    जर (en->reset != nullptr) {
        en->reset(en, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
    }
}
