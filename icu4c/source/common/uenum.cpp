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

#ضمن "unicode/putil.h"
#ضمن "uenumimp.h"
#ضمن "cmemory.h"

/* Layout of the baseContext buffer. */
تعريفنوع بنية {
    int32_t len;  /* number of bytes available starting at 'data' */
    حرف    معلومات; /* actual data starts here */
} UEnum_بفر;

/* Extra bytes to allocate in the baseContext buffer. */
ساكن ثابت int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ساكن فراغ* get_بفر_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    إذا (en->بنياد_Context != خالي_اشارو) {
        إذا (static_cast<UEnum_بفر*>(en->بنياد_Context)->len < capacity) {
            capacity += PAD;
            en->بنياد_Context = uprv_realloc(en->بنياد_Context,
                                           حجم(int32_t) + capacity);
            إذا (en->بنياد_Context == خالي_اشارو) {
                أرجع خالي_اشارو;
            }
            static_cast<UEnum_بفر*>(en->بنياد_Context)->len = capacity;
        }
    } وإلا {
        capacity += PAD;
        en->بنياد_Context = uprv_malloc(حجم(int32_t) + capacity);
        إذا (en->بنياد_Context == خالي_اشارو) {
            أرجع خالي_اشارو;
        }
        static_cast<UEnum_بفر*>(en->بنياد_Context)->len = capacity;
    }
    
    أرجع static_cast<فراغ*>(&static_cast<UEnum_بفر*>(en->بنياد_Context)->معلومات);
}

U_CAPI فراغ U_EXPORT2
uenum_close(UEnumeration* en)
{
    إذا (en) {
        إذا (en->close != خالي_اشارو) {
            إذا (en->بنياد_Context) {
                uprv_free(en->بنياد_Context);
            }
            en->close(en);
        } وإلا { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_ڳڻپ(UEnumeration* en, UError_ڪوڊ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع -1;
    }
    إذا (en->count != خالي_اشارو) {
        أرجع en->count(en, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ثابت char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* نتيجو_ڊيگهائي,
                   UError_ڪوڊ* حالت)
{
    char16_t *ustr = خالي_اشارو;
    int32_t len = 0;
    إذا (en->next != خالي_اشارو) {
        ثابت حرف *cstr = en->next(en, &len, حالت);
        إذا (cstr != خالي_اشارو) {
            ustr = (char16_t*) get_بفر_2(en, (len+1) * حجم(char16_t));
            إذا (ustr == خالي_اشارو) {
                *حالت = U_يادگيري_ورهاست_ERROR;
            } وإلا {
                u_chars_منزل_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
    }
    إذا (نتيجو_ڊيگهائي) {
        *نتيجو_ڊيگهائي = len;
    }
    أرجع ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ثابت حرف* U_EXPORT2
uenum_ايندڙ_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتيجو_ڊيگهائي,
                  UError_ڪوڊ* حالت)
{
    إذا (en->u_ايندڙ != خالي_اشارو) {
        حرف *عارضي_Char_Val;
        ثابت char16_t *عارضي_UChar_Val = en->u_ايندڙ(en, نتيجو_ڊيگهائي, حالت);
        إذا (عارضي_UChar_Val == خالي_اشارو) {
            أرجع خالي_اشارو;
        }
        عارضي_Char_Val = (حرف*)
            get_بفر_2(en, (*نتيجو_ڊيگهائي+1) * حجم(حرف));
        إذا (!عارضي_Char_Val) {
            *حالت = U_يادگيري_ورهاست_ERROR;
            أرجع خالي_اشارو;
        }
        u_UChars_منزل_Chars(عارضي_UChar_Val, عارضي_Char_Val, *نتيجو_ڊيگهائي);
        عارضي_Char_Val[*نتيجو_ڊيگهائي] = 0;
        أرجع عارضي_Char_Val;
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع خالي_اشارو;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* نتيجو_ڊيگهائي,
            UError_ڪوڊ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (en->u_ايندڙ != خالي_اشارو) {
        أرجع en->u_ايندڙ(en, نتيجو_ڊيگهائي, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع خالي_اشارو;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
uenum_ايندڙ(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* نتيجو_ڊيگهائي,
           UError_ڪوڊ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    إذا (en->next != خالي_اشارو) {
        إذا (نتيجو_ڊيگهائي != خالي_اشارو) {
            أرجع en->next(en, نتيجو_ڊيگهائي, حالت);
        }
        وإلا {
            int32_t dummy_ڊيگهائي=0;
            أرجع en->next(en, &dummy_ڊيگهائي, حالت);
        }
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع خالي_اشارو;
    }
}

U_CAPI فراغ U_EXPORT2
uenum_reset(UEnumeration* en, UError_ڪوڊ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع;
    }
    إذا (en->reset != خالي_اشارو) {
        en->reset(en, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
    }
}
