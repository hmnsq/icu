// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: November 11 2002
* Since: ICU 2.4
**********************************************************************
*/
#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work 

#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/strenum.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "ustrenum.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(chars_ಬಫರ್), chars_ಸಾಮರ್ಥ್ಯ(ಗಾತ್ರ(chars_ಬಫರ್)) {
}

StringEnumeration::~StringEnumeration() {
    ಆದರೆ (chars != ಶೂನ್ಯಸೂಚಕ && chars != chars_ಬಫರ್) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
StringEnumeration::next(int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    ಸ್ಥಿರಾಂಕ UnicodeString *s=snext(ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2) && s!=ಶೂನ್ಯಸೂಚಕ) {
        unistr=*s;
        ensure_Chars_ಸಾಮರ್ಥ್ಯ(unistr.length()+1, ಸ್ಥಿತಿ_2);
        ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            ಆದರೆ(ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ!=ಶೂನ್ಯಸೂಚಕ) {
                *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_ಸಾಮರ್ಥ್ಯ, US_INV);
            ಹಿಂತಿರುಗಿ chars;
        }
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ char16_t*
StringEnumeration::unext(int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    ಸ್ಥಿರಾಂಕ UnicodeString *s=snext(ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2) && s!=ಶೂನ್ಯಸೂಚಕ) {
        unistr=*s;
        ಆದರೆ(ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ!=ಶೂನ್ಯಸೂಚಕ) {
            *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ=unistr.length();
        }
        ಹಿಂತಿರುಗಿ unistr.get_Terminated_ಬಫರ್();
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ UnicodeString*
StringEnumeration::snext(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    int32_t length;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s=next(&length, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ ಗಣ_Chars(s, length, ಸ್ಥಿತಿ_2);
}

ಶೂನ್ಯ
StringEnumeration::ensure_Chars_ಸಾಮರ್ಥ್ಯ(int32_t capacity, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2) && capacity>chars_ಸಾಮರ್ಥ್ಯ) {
        ಆದರೆ(capacity<(chars_ಸಾಮರ್ಥ್ಯ+chars_ಸಾಮರ್ಥ್ಯ/2)) {
            // avoid allocation thrashing
            capacity=chars_ಸಾಮರ್ಥ್ಯ+chars_ಸಾಮರ್ಥ್ಯ/2;
        }
        ಆದರೆ(chars!=chars_ಬಫರ್) {
            uprv_free(chars);
        }
        chars = static_cast<ಅಕ್ಷರ*>(uprv_malloc(capacity));
        ಆದರೆ(chars==ಶೂನ್ಯಸೂಚಕ) {
            chars=chars_ಬಫರ್;
            chars_ಸಾಮರ್ಥ್ಯ=ಗಾತ್ರ(chars_ಬಫರ್);
            ಸ್ಥಿತಿ_2=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            chars_ಸಾಮರ್ಥ್ಯ=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::ಗಣ_Chars(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s, int32_t length, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2) && s!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *ಬಫರ್=unistr.get_ಬಫರ್(length+1);
        ಆದರೆ(ಬಫರ್!=ಶೂನ್ಯಸೂಚಕ) {
            u_chars_ಗಮ್ಯ_UChars(s, ಬಫರ್, length);
            ಬಫರ್[length]=0;
            unistr.release_ಬಫರ್(length);
            ಹಿಂತಿರುಗಿ &unistr;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಿತಿ_2=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}
ತಾರ್ಕಿಕ
StringEnumeration::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ StringEnumeration& that)ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ typeid(*ಇದು) == typeid(that); 
}

ತಾರ್ಕಿಕ
StringEnumeration::ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ StringEnumeration& that)ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenum_ಗಮ್ಯ_Adopt, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        uenum_close(uenum_ಗಮ್ಯ_Adopt);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UStringEnumeration *ಫಲಿತಾಂಶ = ಹೊಸ UStringEnumeration(uenum_ಗಮ್ಯ_Adopt);
    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        uenum_close(uenum_ಗಮ್ಯ_Adopt);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uenum_ಎಣಿಕೆ(uenum, &ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* UStringEnumeration::next(int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ uenum_ಮುಂದಿನ(uenum, ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, &ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರಾಂಕ UnicodeString* UStringEnumeration::snext(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) U_LIFETIME_BOUND {
    int32_t length;
    ಸ್ಥಿರಾಂಕ char16_t* str = uenum_unext(uenum, &length, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (str == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ &unistr.ಗಣ_ಗಮ್ಯ(str, length);
}

ಶೂನ್ಯ UStringEnumeration::reset(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    uenum_reset(uenum, &ಸ್ಥಿತಿ_2);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_ಕೊನೆ

// C wrapper --------------------------------------------------------------- ***

#ವ್ಯಾಖ್ಯಾನಿಸು THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    ಅಳಿಸು THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ int32_t U_CALLCONV
ustrenum_ಎಣಿಕೆ(UEnumeration* en,
               UError_ಸಂಕೇತ* ec)
{
    ಹಿಂತಿರುಗಿ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
               UError_ಸಂಕೇತ* ec)
{
    ಹಿಂತಿರುಗಿ THIS(en)->unext(ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_CALLCONV
ustrenum_ಮುಂದಿನ(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
              UError_ಸಂಕೇತ* ec)
{
    ಹಿಂತಿರುಗಿ THIS(en)->next(ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_ಸಂಕೇತ* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration USTRENUM_VT = {
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_ಎಣಿಕೆ,
    ustrenum_unext,
    ustrenum_ಮುಂದಿನ,
    ustrenum_reset
};

U_CDECL_ಕೊನೆ

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_ಸಂಕೇತ* ec) { 
    UEnumeration* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (U_SUCCESS(*ec) && adopted != ಶೂನ್ಯಸೂಚಕ) {
        ಫಲಿತಾಂಶ = (UEnumeration*) uprv_malloc(ಗಾತ್ರ(UEnumeration));
        ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
            *ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            uprv_memcpy(ಫಲಿತಾಂಶ, &USTRENUM_VT, ಗಾತ್ರ(USTRENUM_VT));
            ಫಲಿತಾಂಶ->context = adopted;
        }
    }
    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಅಳಿಸು adopted;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UCharStringEnumeration {
    UEnumeration uenum;
    int32_t ಸೂಚ್ಯಂಕ, count;
} UCharStringEnumeration;

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ಸ್ಥಿರ int32_t U_CALLCONV
ucharstrenum_ಎಣಿಕೆ(UEnumeration* en,
                   UError_ಸಂಕೇತ* /*ec*/) {
    ಹಿಂತಿರುಗಿ ((UCharStringEnumeration*)en)->count;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                  UError_ಸಂಕೇತ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ಆದರೆ (e->ಸೂಚ್ಯಂಕ >= e->count) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ char16_t* ಫಲಿತಾಂಶ = ((ಸ್ಥಿರಾಂಕ char16_t**)e->uenum.context)[e->ಸೂಚ್ಯಂಕ++];
    ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = u_strlen(ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_CALLCONV
ucharstrenum_ಮುಂದಿನ(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                  UError_ಸಂಕೇತ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ಆದರೆ (e->ಸೂಚ್ಯಂಕ >= e->count) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಫಲಿತಾಂಶ = ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**)e->uenum.context)[e->ಸೂಚ್ಯಂಕ++];
    ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = (int32_t)uprv_strlen(ಫಲಿತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_ಸಂಕೇತ* /*ec*/) {
    ((UCharStringEnumeration*)en)->ಸೂಚ್ಯಂಕ = 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration UCHARSTRENUM_VT = {
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ಎಣಿಕೆ,
    uenum_unextDefault,
    ucharstrenum_ಮುಂದಿನ,
    ucharstrenum_reset
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration UCHARSTRENUM_U_VT = {
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ಎಣಿಕೆ,
    ucharstrenum_unext,
    uenum_ಮುಂದಿನ_Default,
    ucharstrenum_reset
};

U_CDECL_ಕೊನೆ

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_ಅಕ್ಷರಸರಣಿಗಳು_Enumeration(U_LIFETIME_BOUND ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ ಅಕ್ಷರಸರಣಿಗಳು[],
                                 int32_t count,
                                 UError_ಸಂಕೇತ* ec) {
    UCharStringEnumeration* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || ಅಕ್ಷರಸರಣಿಗಳು != ಶೂನ್ಯಸೂಚಕ)) {
        ಫಲಿತಾಂಶ = (UCharStringEnumeration*) uprv_malloc(ಗಾತ್ರ(UCharStringEnumeration));
        ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
            *ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            U_ASSERT((ಅಕ್ಷರ*)ಫಲಿತಾಂಶ==(ಅಕ್ಷರ*)(&ಫಲಿತಾಂಶ->uenum));
            uprv_memcpy(ಫಲಿತಾಂಶ, &UCHARSTRENUM_VT, ಗಾತ್ರ(UCHARSTRENUM_VT));
            ಫಲಿತಾಂಶ->uenum.context = (ಶೂನ್ಯ*)ಅಕ್ಷರಸರಣಿಗಳು;
            ಫಲಿತಾಂಶ->ಸೂಚ್ಯಂಕ = 0;
            ಫಲಿತಾಂಶ->count = count;
        }
    }
    ಹಿಂತಿರುಗಿ (UEnumeration*) ಫಲಿತಾಂಶ;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_ಅಕ್ಷರಸರಣಿಗಳು_Enumeration(U_LIFETIME_BOUND ಸ್ಥಿರಾಂಕ char16_t* ಸ್ಥಿರಾಂಕ ಅಕ್ಷರಸರಣಿಗಳು[],
                                  int32_t count,
                                  UError_ಸಂಕೇತ* ec) {
    UCharStringEnumeration* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || ಅಕ್ಷರಸರಣಿಗಳು != ಶೂನ್ಯಸೂಚಕ)) {
        ಫಲಿತಾಂಶ = (UCharStringEnumeration*) uprv_malloc(ಗಾತ್ರ(UCharStringEnumeration));
        ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
            *ec = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            U_ASSERT((ಅಕ್ಷರ*)ಫಲಿತಾಂಶ==(ಅಕ್ಷರ*)(&ಫಲಿತಾಂಶ->uenum));
            uprv_memcpy(ಫಲಿತಾಂಶ, &UCHARSTRENUM_U_VT, ಗಾತ್ರ(UCHARSTRENUM_U_VT));
            ಫಲಿತಾಂಶ->uenum.context = (ಶೂನ್ಯ*)ಅಕ್ಷರಸರಣಿಗಳು;
            ಫಲಿತಾಂಶ->ಸೂಚ್ಯಂಕ = 0;
            ಫಲಿತಾಂಶ->count = count;
        }
    }
    ಹಿಂತಿರುಗಿ (UEnumeration*) ಫಲಿತಾಂಶ;
}


// end C Wrapper
