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

#include "unicode/putil.h"
#include "uenumimp.h"
#include "cmemory.h"

/* Layout of the baseContext buffer. */
typedef struct {
    int32_t len;  /* number of bytes available starting at 'data' */
    char    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
static const int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
static void* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    якщо (en->baseContext != nullptr) {
        якщо (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           sizeof(int32_t) + capacity);
            якщо (en->baseContext == nullptr) {
                повернути nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } інакше {
        capacity += PAD;
        en->baseContext = uprv_malloc(sizeof(int32_t) + capacity);
        якщо (en->baseContext == nullptr) {
            повернути nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    повернути static_cast<void*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI void U_EXPORT2
uenum_close(UEnumeration* en)
{
    якщо (en) {
        якщо (en->close != nullptr) {
            якщо (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } інакше { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_count(UEnumeration* en, UErrorCode* status)
{
    якщо (!en || U_FAILURE(*status)) {
        повернути -1;
    }
    якщо (en->count != nullptr) {
        повернути en->count(en, status);
    } інакше {
        *status = U_UNSUPPORTED_ERROR;
        повернути -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI const char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    якщо (en->next != nullptr) {
        const char *cstr = en->next(en, &len, status);
        якщо (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * sizeof(char16_t));
            якщо (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } інакше {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } інакше {
        *status = U_UNSUPPORTED_ERROR;
    }
    якщо (resultLength) {
        *resultLength = len;
    }
    повернути ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI const char* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    якщо (en->uNext != nullptr) {
        char *tempCharVal;
        const char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        якщо (tempUCharVal == nullptr) {
            повернути nullptr;
        }
        tempCharVal = (char*)
            _getBuffer(en, (*resultLength+1) * sizeof(char));
        якщо (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            повернути nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        повернути tempCharVal;
    } інакше {
        *status = U_UNSUPPORTED_ERROR;
        повернути nullptr;
    }
}

U_CAPI const char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    якщо (!en || U_FAILURE(*status)) {
        повернути nullptr;
    }
    якщо (en->uNext != nullptr) {
        повернути en->uNext(en, resultLength, status);
    } інакше {
        *status = U_UNSUPPORTED_ERROR;
        повернути nullptr;
    }
}

U_CAPI const char* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    якщо (!en || U_FAILURE(*status)) {
        повернути nullptr;
    }
    якщо (en->next != nullptr) {
        якщо (resultLength != nullptr) {
            повернути en->next(en, resultLength, status);
        }
        інакше {
            int32_t dummyLength=0;
            повернути en->next(en, &dummyLength, status);
        }
    } інакше {
        *status = U_UNSUPPORTED_ERROR;
        повернути nullptr;
    }
}

U_CAPI void U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    якщо (!en || U_FAILURE(*status)) {
        повернути;
    }
    якщо (en->reset != nullptr) {
        en->reset(en, status);
    } інакше {
        *status = U_UNSUPPORTED_ERROR;
    }
}
