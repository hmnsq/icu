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
    حرف    داده; /* actual data starts here */
} UEnum_میانگیر;

/* Extra bytes to allocate in the baseContext buffer. */
ساكن ثابت int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ساكن فراغ* get_میانگیر_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    إذا (en->پایه_Context != اشاره_تهی) {
        إذا (static_cast<UEnum_میانگیر*>(en->پایه_Context)->len < capacity) {
            capacity += PAD;
            en->پایه_Context = uprv_realloc(en->پایه_Context,
                                           حجم(int32_t) + capacity);
            إذا (en->پایه_Context == اشاره_تهی) {
                أرجع اشاره_تهی;
            }
            static_cast<UEnum_میانگیر*>(en->پایه_Context)->len = capacity;
        }
    } وإلا {
        capacity += PAD;
        en->پایه_Context = uprv_malloc(حجم(int32_t) + capacity);
        إذا (en->پایه_Context == اشاره_تهی) {
            أرجع اشاره_تهی;
        }
        static_cast<UEnum_میانگیر*>(en->پایه_Context)->len = capacity;
    }
    
    أرجع static_cast<فراغ*>(&static_cast<UEnum_میانگیر*>(en->پایه_Context)->داده);
}

U_CAPI فراغ U_EXPORT2
uenum_close(UEnumeration* en)
{
    إذا (en) {
        إذا (en->close != اشاره_تهی) {
            إذا (en->پایه_Context) {
                uprv_free(en->پایه_Context);
            }
            en->close(en);
        } وإلا { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_شمار(UEnumeration* en, UError_کد* وضعیت)
{
    إذا (!en || U_FAILURE(*وضعیت)) {
        أرجع -1;
    }
    إذا (en->count != اشاره_تهی) {
        أرجع en->count(en, وضعیت);
    } وإلا {
        *وضعیت = U_UNSUPPORTED_ERROR;
        أرجع -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ثابت char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* نتیجه_درازی,
                   UError_کد* وضعیت)
{
    char16_t *ustr = اشاره_تهی;
    int32_t len = 0;
    إذا (en->next != اشاره_تهی) {
        ثابت حرف *cstr = en->next(en, &len, وضعیت);
        إذا (cstr != اشاره_تهی) {
            ustr = (char16_t*) get_میانگیر_2(en, (len+1) * حجم(char16_t));
            إذا (ustr == اشاره_تهی) {
                *وضعیت = U_حافظه_تخصیص_ERROR;
            } وإلا {
                u_chars_مقصد_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } وإلا {
        *وضعیت = U_UNSUPPORTED_ERROR;
    }
    إذا (نتیجه_درازی) {
        *نتیجه_درازی = len;
    }
    أرجع ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ثابت حرف* U_EXPORT2
uenum_بعدی_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجه_درازی,
                  UError_کد* وضعیت)
{
    إذا (en->u_بعدی != اشاره_تهی) {
        حرف *موقت_Char_Val;
        ثابت char16_t *موقت_UChar_Val = en->u_بعدی(en, نتیجه_درازی, وضعیت);
        إذا (موقت_UChar_Val == اشاره_تهی) {
            أرجع اشاره_تهی;
        }
        موقت_Char_Val = (حرف*)
            get_میانگیر_2(en, (*نتیجه_درازی+1) * حجم(حرف));
        إذا (!موقت_Char_Val) {
            *وضعیت = U_حافظه_تخصیص_ERROR;
            أرجع اشاره_تهی;
        }
        u_UChars_مقصد_Chars(موقت_UChar_Val, موقت_Char_Val, *نتیجه_درازی);
        موقت_Char_Val[*نتیجه_درازی] = 0;
        أرجع موقت_Char_Val;
    } وإلا {
        *وضعیت = U_UNSUPPORTED_ERROR;
        أرجع اشاره_تهی;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* نتیجه_درازی,
            UError_کد* وضعیت)
{
    إذا (!en || U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (en->u_بعدی != اشاره_تهی) {
        أرجع en->u_بعدی(en, نتیجه_درازی, وضعیت);
    } وإلا {
        *وضعیت = U_UNSUPPORTED_ERROR;
        أرجع اشاره_تهی;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
uenum_بعدی(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* نتیجه_درازی,
           UError_کد* وضعیت)
{
    إذا (!en || U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا (en->next != اشاره_تهی) {
        إذا (نتیجه_درازی != اشاره_تهی) {
            أرجع en->next(en, نتیجه_درازی, وضعیت);
        }
        وإلا {
            int32_t dummy_درازی=0;
            أرجع en->next(en, &dummy_درازی, وضعیت);
        }
    } وإلا {
        *وضعیت = U_UNSUPPORTED_ERROR;
        أرجع اشاره_تهی;
    }
}

U_CAPI فراغ U_EXPORT2
uenum_reset(UEnumeration* en, UError_کد* وضعیت)
{
    إذا (!en || U_FAILURE(*وضعیت)) {
        أرجع;
    }
    إذا (en->reset != اشاره_تهی) {
        en->reset(en, وضعیت);
    } وإلا {
        *وضعیت = U_UNSUPPORTED_ERROR;
    }
}
