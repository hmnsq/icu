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

#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "uenumimp.h"
#അന്തര്ഭാവ "cmemory.h"

/* Layout of the baseContext buffer. */
തരനിർവചനം ഘടന {
    int32_t len;  /* number of bytes available starting at 'data' */
    അക്ഷരം    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
സ്ഥിതം സ്ഥിരം int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
സ്ഥിതം ശൂന്യം* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    എങ്കിൽ (en->baseContext != nullptr) {
        എങ്കിൽ (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           വലുപ്പം(int32_t) + capacity);
            എങ്കിൽ (en->baseContext == nullptr) {
                തിരികെ nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } അല്ലെങ്കിൽ {
        capacity += PAD;
        en->baseContext = uprv_malloc(വലുപ്പം(int32_t) + capacity);
        എങ്കിൽ (en->baseContext == nullptr) {
            തിരികെ nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    തിരികെ static_cast<ശൂന്യം*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI ശൂന്യം U_EXPORT2
uenum_close(UEnumeration* en)
{
    എങ്കിൽ (en) {
        എങ്കിൽ (en->close != nullptr) {
            എങ്കിൽ (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } അല്ലെങ്കിൽ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_എണ്ണം(UEnumeration* en, UErrorCode* status)
{
    എങ്കിൽ (!en || U_FAILURE(*status)) {
        തിരികെ -1;
    }
    എങ്കിൽ (en->count != nullptr) {
        തിരികെ en->count(en, status);
    } അല്ലെങ്കിൽ {
        *status = U_UNSUPPORTED_ERROR;
        തിരികെ -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI സ്ഥിരം char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    എങ്കിൽ (en->next != nullptr) {
        സ്ഥിരം അക്ഷരം *cstr = en->next(en, &len, status);
        എങ്കിൽ (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * വലുപ്പം(char16_t));
            എങ്കിൽ (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } അല്ലെങ്കിൽ {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } അല്ലെങ്കിൽ {
        *status = U_UNSUPPORTED_ERROR;
    }
    എങ്കിൽ (resultLength) {
        *resultLength = len;
    }
    തിരികെ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    എങ്കിൽ (en->uNext != nullptr) {
        അക്ഷരം *tempCharVal;
        സ്ഥിരം char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        എങ്കിൽ (tempUCharVal == nullptr) {
            തിരികെ nullptr;
        }
        tempCharVal = (അക്ഷരം*)
            _getBuffer(en, (*resultLength+1) * വലുപ്പം(അക്ഷരം));
        എങ്കിൽ (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        തിരികെ tempCharVal;
    } അല്ലെങ്കിൽ {
        *status = U_UNSUPPORTED_ERROR;
        തിരികെ nullptr;
    }
}

U_CAPI സ്ഥിരം char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    എങ്കിൽ (!en || U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (en->uNext != nullptr) {
        തിരികെ en->uNext(en, resultLength, status);
    } അല്ലെങ്കിൽ {
        *status = U_UNSUPPORTED_ERROR;
        തിരികെ nullptr;
    }
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    എങ്കിൽ (!en || U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (en->next != nullptr) {
        എങ്കിൽ (resultLength != nullptr) {
            തിരികെ en->next(en, resultLength, status);
        }
        അല്ലെങ്കിൽ {
            int32_t dummyLength=0;
            തിരികെ en->next(en, &dummyLength, status);
        }
    } അല്ലെങ്കിൽ {
        *status = U_UNSUPPORTED_ERROR;
        തിരികെ nullptr;
    }
}

U_CAPI ശൂന്യം U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    എങ്കിൽ (!en || U_FAILURE(*status)) {
        തിരികെ;
    }
    എങ്കിൽ (en->reset != nullptr) {
        en->reset(en, status);
    } അല്ലെങ്കിൽ {
        *status = U_UNSUPPORTED_ERROR;
    }
}
