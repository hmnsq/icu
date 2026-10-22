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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uenumimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

/* Layout of the baseContext buffer. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    int32_t len;  /* number of bytes available starting at 'data' */
    ਅੱਖਰ    ਅੰਕੜੇ; /* actual data starts here */
} UEnum_ਬਫਰ;

/* Extra bytes to allocate in the baseContext buffer. */
ਸਥਿਰ ਅਚਲ int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ਸਥਿਰ ਖਾਲੀ* get_ਬਫਰ_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    ਜੇ (en->ਅਧਾਰ_Context != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (static_cast<UEnum_ਬਫਰ*>(en->ਅਧਾਰ_Context)->len < capacity) {
            capacity += PAD;
            en->ਅਧਾਰ_Context = uprv_realloc(en->ਅਧਾਰ_Context,
                                           ਆਕਾਰ(int32_t) + capacity);
            ਜੇ (en->ਅਧਾਰ_Context == ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            static_cast<UEnum_ਬਫਰ*>(en->ਅਧਾਰ_Context)->len = capacity;
        }
    } ਨਹੀਂਤਾਂ {
        capacity += PAD;
        en->ਅਧਾਰ_Context = uprv_malloc(ਆਕਾਰ(int32_t) + capacity);
        ਜੇ (en->ਅਧਾਰ_Context == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        static_cast<UEnum_ਬਫਰ*>(en->ਅਧਾਰ_Context)->len = capacity;
    }
    
    ਵਾਪਸ static_cast<ਖਾਲੀ*>(&static_cast<UEnum_ਬਫਰ*>(en->ਅਧਾਰ_Context)->ਅੰਕੜੇ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uenum_close(UEnumeration* en)
{
    ਜੇ (en) {
        ਜੇ (en->close != ਖਾਲੀ_ਸੂਚਕ) {
            ਜੇ (en->ਅਧਾਰ_Context) {
                uprv_free(en->ਅਧਾਰ_Context);
            }
            en->close(en);
        } ਨਹੀਂਤਾਂ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_ਗਿਣਤੀ(UEnumeration* en, UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (!en || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ -1;
    }
    ਜੇ (en->count != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ en->count(en, ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ਅਚਲ char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                   UError_ਕੋਡ* ਸਥਿਤੀ)
{
    char16_t *ustr = ਖਾਲੀ_ਸੂਚਕ;
    int32_t len = 0;
    ਜੇ (en->next != ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ ਅੱਖਰ *cstr = en->next(en, &len, ਸਥਿਤੀ);
        ਜੇ (cstr != ਖਾਲੀ_ਸੂਚਕ) {
            ustr = (char16_t*) get_ਬਫਰ_2(en, (len+1) * ਆਕਾਰ(char16_t));
            ਜੇ (ustr == ਖਾਲੀ_ਸੂਚਕ) {
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            } ਨਹੀਂਤਾਂ {
                u_chars_ਮੰਜ਼ਿਲ_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
    }
    ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = len;
    }
    ਵਾਪਸ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uenum_ਅਗਲਾ_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                  UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (en->u_ਅਗਲਾ != ਖਾਲੀ_ਸੂਚਕ) {
        ਅੱਖਰ *ਆਰਜ਼ੀ_Char_Val;
        ਅਚਲ char16_t *ਆਰਜ਼ੀ_UChar_Val = en->u_ਅਗਲਾ(en, ਨਤੀਜਾ_ਲੰਮਾਈ, ਸਥਿਤੀ);
        ਜੇ (ਆਰਜ਼ੀ_UChar_Val == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        ਆਰਜ਼ੀ_Char_Val = (ਅੱਖਰ*)
            get_ਬਫਰ_2(en, (*ਨਤੀਜਾ_ਲੰਮਾਈ+1) * ਆਕਾਰ(ਅੱਖਰ));
        ਜੇ (!ਆਰਜ਼ੀ_Char_Val) {
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        u_UChars_ਮੰਜ਼ਿਲ_Chars(ਆਰਜ਼ੀ_UChar_Val, ਆਰਜ਼ੀ_Char_Val, *ਨਤੀਜਾ_ਲੰਮਾਈ);
        ਆਰਜ਼ੀ_Char_Val[*ਨਤੀਜਾ_ਲੰਮਾਈ] = 0;
        ਵਾਪਸ ਆਰਜ਼ੀ_Char_Val;
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
            UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (!en || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (en->u_ਅਗਲਾ != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ en->u_ਅਗਲਾ(en, ਨਤੀਜਾ_ਲੰਮਾਈ, ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uenum_ਅਗਲਾ(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
           UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (!en || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (en->next != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ en->next(en, ਨਤੀਜਾ_ਲੰਮਾਈ, ਸਥਿਤੀ);
        }
        ਨਹੀਂਤਾਂ {
            int32_t dummy_ਲੰਮਾਈ=0;
            ਵਾਪਸ en->next(en, &dummy_ਲੰਮਾਈ, ਸਥਿਤੀ);
        }
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
uenum_reset(UEnumeration* en, UError_ਕੋਡ* ਸਥਿਤੀ)
{
    ਜੇ (!en || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (en->reset != ਖਾਲੀ_ਸੂਚਕ) {
        en->reset(en, ਸਥਿਤੀ);
    } ਨਹੀਂਤਾਂ {
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
    }
}
