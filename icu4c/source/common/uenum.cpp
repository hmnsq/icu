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

#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "uenumimp.h"
#અન્તર્ભાવ "cmemory.h"

/* Layout of the baseContext buffer. */
પ્રકારવ્યાખ્યા સંરચના {
    int32_t len;  /* number of bytes available starting at 'data' */
    અક્ષર    data; /* actual data starts here */
} _UEnumBuffer;

/* Extra bytes to allocate in the baseContext buffer. */
સ્થિર અચલ int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
સ્થિર શૂન્ય* _getBuffer(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    જો (en->baseContext != nullptr) {
        જો (static_cast<_UEnumBuffer*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           કદ(int32_t) + capacity);
            જો (en->baseContext == nullptr) {
                પાછા nullptr;
            }
            static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
        }
    } નહીંતર {
        capacity += PAD;
        en->baseContext = uprv_malloc(કદ(int32_t) + capacity);
        જો (en->baseContext == nullptr) {
            પાછા nullptr;
        }
        static_cast<_UEnumBuffer*>(en->baseContext)->len = capacity;
    }
    
    પાછા static_cast<શૂન્ય*>(&static_cast<_UEnumBuffer*>(en->baseContext)->data);
}

U_CAPI શૂન્ય U_EXPORT2
uenum_close(UEnumeration* en)
{
    જો (en) {
        જો (en->close != nullptr) {
            જો (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } નહીંતર { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_ગણતરી(UEnumeration* en, UErrorCode* status)
{
    જો (!en || U_FAILURE(*status)) {
        પાછા -1;
    }
    જો (en->count != nullptr) {
        પાછા en->count(en, status);
    } નહીંતર {
        *status = U_UNSUPPORTED_ERROR;
        પાછા -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI અચલ char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* resultLength,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    જો (en->next != nullptr) {
        અચલ અક્ષર *cstr = en->next(en, &len, status);
        જો (cstr != nullptr) {
            ustr = (char16_t*) _getBuffer(en, (len+1) * કદ(char16_t));
            જો (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } નહીંતર {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } નહીંતર {
        *status = U_UNSUPPORTED_ERROR;
    }
    જો (resultLength) {
        *resultLength = len;
    }
    પાછા ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI અચલ અક્ષર* U_EXPORT2
uenum_nextDefault(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* status)
{
    જો (en->uNext != nullptr) {
        અક્ષર *tempCharVal;
        અચલ char16_t *tempUCharVal = en->uNext(en, resultLength, status);
        જો (tempUCharVal == nullptr) {
            પાછા nullptr;
        }
        tempCharVal = (અક્ષર*)
            _getBuffer(en, (*resultLength+1) * કદ(અક્ષર));
        જો (!tempCharVal) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            પાછા nullptr;
        }
        u_UCharsToChars(tempUCharVal, tempCharVal, *resultLength);
        tempCharVal[*resultLength] = 0;
        પાછા tempCharVal;
    } નહીંતર {
        *status = U_UNSUPPORTED_ERROR;
        પાછા nullptr;
    }
}

U_CAPI અચલ char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* resultLength,
            UErrorCode* status)
{
    જો (!en || U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો (en->uNext != nullptr) {
        પાછા en->uNext(en, resultLength, status);
    } નહીંતર {
        *status = U_UNSUPPORTED_ERROR;
        પાછા nullptr;
    }
}

U_CAPI અચલ અક્ષર* U_EXPORT2
uenum_next(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* resultLength,
           UErrorCode* status)
{
    જો (!en || U_FAILURE(*status)) {
        પાછા nullptr;
    }
    જો (en->next != nullptr) {
        જો (resultLength != nullptr) {
            પાછા en->next(en, resultLength, status);
        }
        નહીંતર {
            int32_t dummyLength=0;
            પાછા en->next(en, &dummyLength, status);
        }
    } નહીંતર {
        *status = U_UNSUPPORTED_ERROR;
        પાછા nullptr;
    }
}

U_CAPI શૂન્ય U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    જો (!en || U_FAILURE(*status)) {
        પાછા;
    }
    જો (en->reset != nullptr) {
        en->reset(en, status);
    } નહીંતર {
        *status = U_UNSUPPORTED_ERROR;
    }
}
