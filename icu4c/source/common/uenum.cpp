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

#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

/* Layout of the baseContext buffer. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    int32_t len;  /* number of bytes available starting at 'data' */
    ಅಕ್ಷರ    ದತ್ತಾಂಶ; /* actual data starts here */
} UEnum_ಬಫರ್;

/* Extra bytes to allocate in the baseContext buffer. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
ಸ್ಥಿರ ಶೂನ್ಯ* get_ಬಫರ್_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    ಆದರೆ (en->ತಳ_Context != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (static_cast<UEnum_ಬಫರ್*>(en->ತಳ_Context)->len < capacity) {
            capacity += PAD;
            en->ತಳ_Context = uprv_realloc(en->ತಳ_Context,
                                           ಗಾತ್ರ(int32_t) + capacity);
            ಆದರೆ (en->ತಳ_Context == ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            static_cast<UEnum_ಬಫರ್*>(en->ತಳ_Context)->len = capacity;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        capacity += PAD;
        en->ತಳ_Context = uprv_malloc(ಗಾತ್ರ(int32_t) + capacity);
        ಆದರೆ (en->ತಳ_Context == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        static_cast<UEnum_ಬಫರ್*>(en->ತಳ_Context)->len = capacity;
    }
    
    ಹಿಂತಿರುಗಿ static_cast<ಶೂನ್ಯ*>(&static_cast<UEnum_ಬಫರ್*>(en->ತಳ_Context)->ದತ್ತಾಂಶ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uenum_close(UEnumeration* en)
{
    ಆದರೆ (en) {
        ಆದರೆ (en->close != ಶೂನ್ಯಸೂಚಕ) {
            ಆದರೆ (en->ತಳ_Context) {
                uprv_free(en->ತಳ_Context);
            }
            en->close(en);
        } ಇಲ್ಲದಿದ್ದರೆ { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_ಎಣಿಕೆ(UEnumeration* en, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (!en || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ (en->count != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ en->count(en, ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                   UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    char16_t *ustr = ಶೂನ್ಯಸೂಚಕ;
    int32_t len = 0;
    ಆದರೆ (en->next != ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cstr = en->next(en, &len, ಸ್ಥಿತಿ_2);
        ಆದರೆ (cstr != ಶೂನ್ಯಸೂಚಕ) {
            ustr = (char16_t*) get_ಬಫರ್_2(en, (len+1) * ಗಾತ್ರ(char16_t));
            ಆದರೆ (ustr == ಶೂನ್ಯಸೂಚಕ) {
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            } ಇಲ್ಲದಿದ್ದರೆ {
                u_chars_ಗಮ್ಯ_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
    }
    ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = len;
    }
    ಹಿಂತಿರುಗಿ ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uenum_ಮುಂದಿನ_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (en->u_ಮುಂದಿನ != ಶೂನ್ಯಸೂಚಕ) {
        ಅಕ್ಷರ *ತಾತ್ಕಾಲಿಕ_Char_Val;
        ಸ್ಥಿರಾಂಕ char16_t *ತಾತ್ಕಾಲಿಕ_UChar_Val = en->u_ಮುಂದಿನ(en, ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
        ಆದರೆ (ತಾತ್ಕಾಲಿಕ_UChar_Val == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ತಾತ್ಕಾಲಿಕ_Char_Val = (ಅಕ್ಷರ*)
            get_ಬಫರ್_2(en, (*ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ+1) * ಗಾತ್ರ(ಅಕ್ಷರ));
        ಆದರೆ (!ತಾತ್ಕಾಲಿಕ_Char_Val) {
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        u_UChars_ಗಮ್ಯ_Chars(ತಾತ್ಕಾಲಿಕ_UChar_Val, ತಾತ್ಕಾಲಿಕ_Char_Val, *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ);
        ತಾತ್ಕಾಲಿಕ_Char_Val[*ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ] = 0;
        ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ_Char_Val;
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
            UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (!en || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (en->u_ಮುಂದಿನ != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ en->u_ಮುಂದಿನ(en, ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uenum_ಮುಂದಿನ(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
           UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (!en || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (en->next != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ en->next(en, ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            int32_t dummy_ಉದ್ದಳತೆ=0;
            ಹಿಂತಿರುಗಿ en->next(en, &dummy_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uenum_reset(UEnumeration* en, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (!en || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (en->reset != ಶೂನ್ಯಸೂಚಕ) {
        en->reset(en, ಸ್ಥಿತಿ_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
    }
}
