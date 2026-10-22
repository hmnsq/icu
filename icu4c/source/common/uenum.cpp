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

#समावेश "unicode/putil.h"
#समावेश "uenumimp.h"
#समावेश "cmemory.h"

/* Layout of the baseContext buffer. */
प्रकार काष्ठा {
    int32_t len;  /* number of bytes available starting at 'data' */
    अक्षर    तथ्य; /* actual data starts here */
} UEnum_बफर;

/* Extra bytes to allocate in the baseContext buffer. */
अटल स्थिर int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
अटल व्योम* get_बफर_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    अगर (en->baseContext != nullptr) {
        अगर (static_cast<UEnum_बफर*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           माप(int32_t) + capacity);
            अगर (en->baseContext == nullptr) {
                वापस nullptr;
            }
            static_cast<UEnum_बफर*>(en->baseContext)->len = capacity;
        }
    } अन्यथा {
        capacity += PAD;
        en->baseContext = uprv_malloc(माप(int32_t) + capacity);
        अगर (en->baseContext == nullptr) {
            वापस nullptr;
        }
        static_cast<UEnum_बफर*>(en->baseContext)->len = capacity;
    }
    
    वापस static_cast<व्योम*>(&static_cast<UEnum_बफर*>(en->baseContext)->तथ्य);
}

U_CAPI व्योम U_EXPORT2
uenum_close(UEnumeration* en)
{
    अगर (en) {
        अगर (en->बंद != nullptr) {
            अगर (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->बंद(en);
        } अन्यथा { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_गिनती(UEnumeration* en, UErrorCode* status)
{
    अगर (!en || U_FAILURE(*status)) {
        वापस -1;
    }
    अगर (en->count != nullptr) {
        वापस en->count(en, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        वापस -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI स्थिर char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* परिणाम_लंबाई,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    अगर (en->next != nullptr) {
        स्थिर अक्षर *cstr = en->next(en, &len, status);
        अगर (cstr != nullptr) {
            ustr = (char16_t*) get_बफर_2(en, (len+1) * माप(char16_t));
            अगर (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } अन्यथा {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
    }
    अगर (परिणाम_लंबाई) {
        *परिणाम_लंबाई = len;
    }
    वापस ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI स्थिर अक्षर* U_EXPORT2
uenum_अग्र_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* परिणाम_लंबाई,
                  UErrorCode* status)
{
    अगर (en->u_अग्र != nullptr) {
        अक्षर *अस्थायी_Char_Val;
        स्थिर char16_t *अस्थायी_UChar_Val = en->u_अग्र(en, परिणाम_लंबाई, status);
        अगर (अस्थायी_UChar_Val == nullptr) {
            वापस nullptr;
        }
        अस्थायी_Char_Val = (अक्षर*)
            get_बफर_2(en, (*परिणाम_लंबाई+1) * माप(अक्षर));
        अगर (!अस्थायी_Char_Val) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }
        u_UCharsToChars(अस्थायी_UChar_Val, अस्थायी_Char_Val, *परिणाम_लंबाई);
        अस्थायी_Char_Val[*परिणाम_लंबाई] = 0;
        वापस अस्थायी_Char_Val;
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        वापस nullptr;
    }
}

U_CAPI स्थिर char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* परिणाम_लंबाई,
            UErrorCode* status)
{
    अगर (!en || U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर (en->u_अग्र != nullptr) {
        वापस en->u_अग्र(en, परिणाम_लंबाई, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        वापस nullptr;
    }
}

U_CAPI स्थिर अक्षर* U_EXPORT2
uenum_अग्र(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* परिणाम_लंबाई,
           UErrorCode* status)
{
    अगर (!en || U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर (en->next != nullptr) {
        अगर (परिणाम_लंबाई != nullptr) {
            वापस en->next(en, परिणाम_लंबाई, status);
        }
        अन्यथा {
            int32_t dummy_लंबाई=0;
            वापस en->next(en, &dummy_लंबाई, status);
        }
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
        वापस nullptr;
    }
}

U_CAPI व्योम U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    अगर (!en || U_FAILURE(*status)) {
        वापस;
    }
    अगर (en->reset != nullptr) {
        en->reset(en, status);
    } अन्यथा {
        *status = U_UNSUPPORTED_ERROR;
    }
}
