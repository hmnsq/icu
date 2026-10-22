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

    إذا (en->بنسټ_Context != تش_اشاره) {
        إذا (static_cast<UEnum_بفر*>(en->بنسټ_Context)->len < capacity) {
            capacity += PAD;
            en->بنسټ_Context = uprv_realloc(en->بنسټ_Context,
                                           حجم(int32_t) + capacity);
            إذا (en->بنسټ_Context == تش_اشاره) {
                أرجع تش_اشاره;
            }
            static_cast<UEnum_بفر*>(en->بنسټ_Context)->len = capacity;
        }
    } وإلا {
        capacity += PAD;
        en->بنسټ_Context = uprv_malloc(حجم(int32_t) + capacity);
        إذا (en->بنسټ_Context == تش_اشاره) {
            أرجع تش_اشاره;
        }
        static_cast<UEnum_بفر*>(en->بنسټ_Context)->len = capacity;
    }
    
    أرجع static_cast<فراغ*>(&static_cast<UEnum_بفر*>(en->بنسټ_Context)->معلومات);
}

U_CAPI فراغ U_EXPORT2
uenum_close(UEnumeration* en)
{
    إذا (en) {
        إذا (en->close != تش_اشاره) {
            إذا (en->بنسټ_Context) {
                uprv_free(en->بنسټ_Context);
            }
            en->close(en);
        } وإلا { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_شمېر(UEnumeration* en, UError_کوډ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع -1;
    }
    إذا (en->count != تش_اشاره) {
        أرجع en->count(en, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ثابت char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* پایله_اوږدتیا,
                   UError_کوډ* حالت)
{
    char16_t *ustr = تش_اشاره;
    int32_t len = 0;
    إذا (en->next != تش_اشاره) {
        ثابت حرف *cstr = en->next(en, &len, حالت);
        إذا (cstr != تش_اشاره) {
            ustr = (char16_t*) get_بفر_2(en, (len+1) * حجم(char16_t));
            إذا (ustr == تش_اشاره) {
                *حالت = U_حافظه_ویش_ERROR;
            } وإلا {
                u_chars_منزل_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
    }
    إذا (پایله_اوږدتیا) {
        *پایله_اوږدتیا = len;
    }
    أرجع ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ثابت حرف* U_EXPORT2
uenum_بل_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* پایله_اوږدتیا,
                  UError_کوډ* حالت)
{
    إذا (en->u_بل != تش_اشاره) {
        حرف *لنډمهاله_Char_Val;
        ثابت char16_t *لنډمهاله_UChar_Val = en->u_بل(en, پایله_اوږدتیا, حالت);
        إذا (لنډمهاله_UChar_Val == تش_اشاره) {
            أرجع تش_اشاره;
        }
        لنډمهاله_Char_Val = (حرف*)
            get_بفر_2(en, (*پایله_اوږدتیا+1) * حجم(حرف));
        إذا (!لنډمهاله_Char_Val) {
            *حالت = U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }
        u_UChars_منزل_Chars(لنډمهاله_UChar_Val, لنډمهاله_Char_Val, *پایله_اوږدتیا);
        لنډمهاله_Char_Val[*پایله_اوږدتیا] = 0;
        أرجع لنډمهاله_Char_Val;
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع تش_اشاره;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* پایله_اوږدتیا,
            UError_کوډ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا (en->u_بل != تش_اشاره) {
        أرجع en->u_بل(en, پایله_اوږدتیا, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع تش_اشاره;
    }
}

U_CAPI ثابت حرف* U_EXPORT2
uenum_بل(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* پایله_اوږدتیا,
           UError_کوډ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    إذا (en->next != تش_اشاره) {
        إذا (پایله_اوږدتیا != تش_اشاره) {
            أرجع en->next(en, پایله_اوږدتیا, حالت);
        }
        وإلا {
            int32_t dummy_اوږدتیا=0;
            أرجع en->next(en, &dummy_اوږدتیا, حالت);
        }
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
        أرجع تش_اشاره;
    }
}

U_CAPI فراغ U_EXPORT2
uenum_reset(UEnumeration* en, UError_کوډ* حالت)
{
    إذا (!en || U_FAILURE(*حالت)) {
        أرجع;
    }
    إذا (en->reset != تش_اشاره) {
        en->reset(en, حالت);
    } وإلا {
        *حالت = U_UNSUPPORTED_ERROR;
    }
}
