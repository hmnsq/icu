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

#شامل "unicode/putil.h"
#شامل "uenumimp.h"
#شامل "cmemory.h"

/* Layout of the baseContext buffer. */
قسمتعریف ساخت {
    int32_t len;  /* number of bytes available starting at 'data' */
    حرف    ڈیٹا; /* actual data starts here */
} UEnum_بفر;

/* Extra bytes to allocate in the baseContext buffer. */
ساکن ثابت int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ساکن خالی* get_بفر_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    اگر (en->بنیاد_Context != خالی_اشاریہ) {
        اگر (static_cast<UEnum_بفر*>(en->بنیاد_Context)->len < capacity) {
            capacity += PAD;
            en->بنیاد_Context = uprv_realloc(en->بنیاد_Context,
                                           حجم(int32_t) + capacity);
            اگر (en->بنیاد_Context == خالی_اشاریہ) {
                واپس خالی_اشاریہ;
            }
            static_cast<UEnum_بفر*>(en->بنیاد_Context)->len = capacity;
        }
    } ورنہ {
        capacity += PAD;
        en->بنیاد_Context = uprv_malloc(حجم(int32_t) + capacity);
        اگر (en->بنیاد_Context == خالی_اشاریہ) {
            واپس خالی_اشاریہ;
        }
        static_cast<UEnum_بفر*>(en->بنیاد_Context)->len = capacity;
    }
    
    واپس static_cast<خالی*>(&static_cast<UEnum_بفر*>(en->بنیاد_Context)->ڈیٹا);
}

U_CAPI خالی U_EXPORT2
uenum_close(UEnumeration* en)
{
    اگر (en) {
        اگر (en->close != خالی_اشاریہ) {
            اگر (en->بنیاد_Context) {
                uprv_free(en->بنیاد_Context);
            }
            en->close(en);
        } ورنہ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_گنتی(UEnumeration* en, UError_رمز* حالت_2)
{
    اگر (!en || U_FAILURE(*حالت_2)) {
        واپس -1;
    }
    اگر (en->count != خالی_اشاریہ) {
        واپس en->count(en, حالت_2);
    } ورنہ {
        *حالت_2 = U_UNSUPPORTED_ERROR;
        واپس -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ثابت char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* نتیجہ_لمبائی,
                   UError_رمز* حالت_2)
{
    char16_t *ustr = خالی_اشاریہ;
    int32_t len = 0;
    اگر (en->next != خالی_اشاریہ) {
        ثابت حرف *cstr = en->next(en, &len, حالت_2);
        اگر (cstr != خالی_اشاریہ) {
            ustr = (char16_t*) get_بفر_2(en, (len+1) * حجم(char16_t));
            اگر (ustr == خالی_اشاریہ) {
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
            } ورنہ {
                u_chars_مقصد_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ورنہ {
        *حالت_2 = U_UNSUPPORTED_ERROR;
    }
    اگر (نتیجہ_لمبائی) {
        *نتیجہ_لمبائی = len;
    }
    واپس ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ثابت حرف* U_EXPORT2
uenum_اگلا_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجہ_لمبائی,
                  UError_رمز* حالت_2)
{
    اگر (en->u_اگلا != خالی_اشاریہ) {
        حرف *عارضی_Char_Val;
        ثابت char16_t *عارضی_UChar_Val = en->u_اگلا(en, نتیجہ_لمبائی, حالت_2);
        اگر (عارضی_UChar_Val == خالی_اشاریہ) {
            واپس خالی_اشاریہ;
        }
        عارضی_Char_Val = (حرف*)
            get_بفر_2(en, (*نتیجہ_لمبائی+1) * حجم(حرف));
        اگر (!عارضی_Char_Val) {
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }
        u_UChars_مقصد_Chars(عارضی_UChar_Val, عارضی_Char_Val, *نتیجہ_لمبائی);
        عارضی_Char_Val[*نتیجہ_لمبائی] = 0;
        واپس عارضی_Char_Val;
    } ورنہ {
        *حالت_2 = U_UNSUPPORTED_ERROR;
        واپس خالی_اشاریہ;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* نتیجہ_لمبائی,
            UError_رمز* حالت_2)
{
    اگر (!en || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (en->u_اگلا != خالی_اشاریہ) {
        واپس en->u_اگلا(en, نتیجہ_لمبائی, حالت_2);
    } ورنہ {
        *حالت_2 = U_UNSUPPORTED_ERROR;
        واپس خالی_اشاریہ;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
uenum_اگلا(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* نتیجہ_لمبائی,
           UError_رمز* حالت_2)
{
    اگر (!en || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (en->next != خالی_اشاریہ) {
        اگر (نتیجہ_لمبائی != خالی_اشاریہ) {
            واپس en->next(en, نتیجہ_لمبائی, حالت_2);
        }
        ورنہ {
            int32_t dummy_لمبائی=0;
            واپس en->next(en, &dummy_لمبائی, حالت_2);
        }
    } ورنہ {
        *حالت_2 = U_UNSUPPORTED_ERROR;
        واپس خالی_اشاریہ;
    }
}

U_CAPI خالی U_EXPORT2
uenum_reset(UEnumeration* en, UError_رمز* حالت_2)
{
    اگر (!en || U_FAILURE(*حالت_2)) {
        واپس;
    }
    اگر (en->reset != خالی_اشاریہ) {
        en->reset(en, حالت_2);
    } ورنہ {
        *حالت_2 = U_UNSUPPORTED_ERROR;
    }
}
