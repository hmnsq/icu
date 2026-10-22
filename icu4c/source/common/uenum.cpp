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
    حرف    انکڑے; /* actual data starts here */
} UEnum_بفر;

/* Extra bytes to allocate in the baseContext buffer. */
ساكن ثابت int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ساكن فراغ* get_بفر_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    إذا (en->ادھار_Context != خالی_سوچک) {
        إذا (static_cast<UEnum_بفر*>(en->ادھار_Context)->len < capacity) {
            capacity += PAD;
            en->ادھار_Context = uprv_realloc(en->ادھار_Context,
                                           حجم(int32_t) + capacity);
            إذا (en->ادھار_Context == خالی_سوچک) {
                أرجع خالی_سوچک;
            }
            static_cast<UEnum_بفر*>(en->ادھار_Context)->len = capacity;
        }
    } وإلا {
        capacity += PAD;
        en->ادھار_Context = uprv_malloc(حجم(int32_t) + capacity);
        إذا (en->ادھار_Context == خالی_سوچک) {
            أرجع خالی_سوچک;
        }
        static_cast<UEnum_بفر*>(en->ادھار_Context)->len = capacity;
    }
    
    أرجع static_cast<فراغ*>(&static_cast<UEnum_بفر*>(en->ادھار_Context)->انکڑے);
}

U_CAPI فراغ U_EXPORT2
uenum_close(UEnumeration* en)
{
    إذا (en) {
        إذا (en->close != خالی_سوچک) {
            إذا (en->ادھار_Context) {
                uprv_free(en->ادھار_Context);
            }
            en->close(en);
        } وإلا { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_گنتی(UEnumeration* en, UError_کوڈ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع -1;
    }
    إذا (en->count != خالی_سوچک) {
        أرجع en->count(en, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ثابت char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* نتیجہ_لمائی,
                   UError_کوڈ* حالت)
{
    char16_t *ustr = خالی_سوچک;
    int32_t len = 0;
    إذا (en->next != خالی_سوچک) {
        ثابت حرف *cstr = en->next(en, &len, حالت);
        إذا (cstr != خالی_سوچک) {
            ustr = (char16_t*) get_بفر_2(en, (len+1) * حجم(char16_t));
            إذا (ustr == خالی_سوچک) {
                *حالت = U_حافظہ_ونڈ_ERROR;
            } وإلا {
                u_chars_منزل_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
    }
    إذا (نتیجہ_لمائی) {
        *نتیجہ_لمائی = len;
    }
    أرجع ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ثابت حرف* U_EXPORT2
uenum_اگلا_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجہ_لمائی,
                  UError_کوڈ* حالت)
{
    إذا (en->u_اگلا != خالی_سوچک) {
        حرف *عارضی_Char_Val;
        ثابت char16_t *عارضی_UChar_Val = en->u_اگلا(en, نتیجہ_لمائی, حالت);
        إذا (عارضی_UChar_Val == خالی_سوچک) {
            أرجع خالی_سوچک;
        }
        عارضی_Char_Val = (حرف*)
            get_بفر_2(en, (*نتیجہ_لمائی+1) * حجم(حرف));
        إذا (!عارضی_Char_Val) {
            *حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }
        u_UChars_منزل_Chars(عارضی_UChar_Val, عارضی_Char_Val, *نتیجہ_لمائی);
        عارضی_Char_Val[*نتیجہ_لمائی] = 0;
        أرجع عارضی_Char_Val;
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع خالی_سوچک;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* نتیجہ_لمائی,
            UError_کوڈ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (en->u_اگلا != خالی_سوچک) {
        أرجع en->u_اگلا(en, نتیجہ_لمائی, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع خالی_سوچک;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
uenum_اگلا(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* نتیجہ_لمائی,
           UError_کوڈ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (en->next != خالی_سوچک) {
        إذا (نتیجہ_لمائی != خالی_سوچک) {
            أرجع en->next(en, نتیجہ_لمائی, حالت);
        }
        وإلا {
            int32_t dummy_لمائی=0;
            أرجع en->next(en, &dummy_لمائی, حالت);
        }
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع خالی_سوچک;
    }
}

U_CAPI فراغ U_EXPORT2
uenum_reset(UEnumeration* en, UError_کوڈ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع;
    }
    إذا (en->reset != خالی_سوچک) {
        en->reset(en, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
    }
}
