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
स्थिर अचर int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
स्थिर शून्य* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    यदि (en->baseContext != nullptr) {
        यदि (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           परिमाण(int32_t) + capacity);
            यदि (en->baseContext == nullptr) {
                फर्काऊ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } अन्यथा {
        capacity += PAD;
        en->baseContext = uprv_malloc(परिमाण(int32_t) + capacity);
        यदि (en->baseContext == nullptr) {
            फर्काऊ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    फर्काऊ static_cast<शून्य*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI शून्य U_EXPORT2
uenum_close(UEnumeration* en)
{
    यदि (en) {
        यदि (en->बंद != nullptr) {
            यदि (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->बंद(en);
        } अन्यथा { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_गन्ती(UEnumeration* en, UErrorCode* status)
{
    यदि (!en || U_FAILURE(*status)) {
        फर्काऊ -1;
    }
    यदि (en->count != nullptr) {
        फर्काऊ en->count(en, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        फर्काऊ -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI अचर char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    यदि (en->next != nullptr) {
        अचर वर्ण *cstr = en->next(en, &len, status);
        यदि (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * परिमाण(char16_t));
            यदि (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } अन्यथा {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
    }
    यदि (resultLength) {
        *resultLength = len;
    }
    फर्काऊ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI अचर वर्ण* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    यदि (en->uNext != nullptr) {
        वर्ण *tempCharVal;
        अचर char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        यदि (tempUCharVal == nullptr) {
            फर्काऊ nullptr;
        }
        tempCharVal = (वर्ण*)
            _getBuffer(en, (*resultLength+1) * परिमाण(वर्ण));
        यदि (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        फर्काऊ tempCharVal;
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        फर्काऊ nullptr;
    }
}

U_CAPI अचर char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    यदि (!en || U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि (en->uNext != nullptr) {
        फर्काऊ en->uNext(en, resultLength, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        फर्काऊ nullptr;
    }
}

U_CAPI अचर वर्ण* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    यदि (!en || U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    यदि (en->next != nullptr) {
        यदि (resultLength != nullptr) {
            फर्काऊ en->next(en, resultLength, status);
        }
        अन्यथा {
            int32_t dummyLength=0;
            फर्काऊ en->next(en, &dummyLength, status);
        }
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        फर्काऊ nullptr;
    }
}

U_CAPI शून्य U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    यदि (!en || U_FAILURE(*status)) {
        फर्काऊ;
    }
    यदि (en->reset != nullptr) {
        en->reset(en, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
    }
}
