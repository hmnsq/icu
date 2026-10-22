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
#ਅਨ੍ਤਰ੍ਭਾਵ "utypeinfo.h"  // for 'typeid' to work 

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/strenum.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uenumimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustrenum.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(chars_ਬਫਰ), chars_ਸਮਰੱਥਾ(ਆਕਾਰ(chars_ਬਫਰ)) {
}

StringEnumeration::~StringEnumeration() {
    ਜੇ (chars != ਖਾਲੀ_ਸੂਚਕ && chars != chars_ਬਫਰ) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ਅਚਲ {
  ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ ਅੱਖਰ*
StringEnumeration::next(int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    ਅਚਲ UnicodeString *s=snext(ਸਥਿਤੀ);
    ਜੇ(U_SUCCESS(ਸਥਿਤੀ) && s!=ਖਾਲੀ_ਸੂਚਕ) {
        unistr=*s;
        ensure_Chars_ਸਮਰੱਥਾ(unistr.length()+1, ਸਥਿਤੀ);
        ਜੇ(U_SUCCESS(ਸਥਿਤੀ)) {
            ਜੇ(ਨਤੀਜਾ_ਲੰਮਾਈ!=ਖਾਲੀ_ਸੂਚਕ) {
                *ਨਤੀਜਾ_ਲੰਮਾਈ=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_ਸਮਰੱਥਾ, US_INV);
            ਵਾਪਸ chars;
        }
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ char16_t*
StringEnumeration::unext(int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    ਅਚਲ UnicodeString *s=snext(ਸਥਿਤੀ);
    ਜੇ(U_SUCCESS(ਸਥਿਤੀ) && s!=ਖਾਲੀ_ਸੂਚਕ) {
        unistr=*s;
        ਜੇ(ਨਤੀਜਾ_ਲੰਮਾਈ!=ਖਾਲੀ_ਸੂਚਕ) {
            *ਨਤੀਜਾ_ਲੰਮਾਈ=unistr.length();
        }
        ਵਾਪਸ unistr.get_Terminated_ਬਫਰ();
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ UnicodeString*
StringEnumeration::snext(UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    int32_t length;
    ਅਚਲ ਅੱਖਰ *s=next(&length, ਸਥਿਤੀ);
    ਵਾਪਸ ਸੈੱਟ_Chars(s, length, ਸਥਿਤੀ);
}

ਖਾਲੀ
StringEnumeration::ensure_Chars_ਸਮਰੱਥਾ(int32_t capacity, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ(U_SUCCESS(ਸਥਿਤੀ) && capacity>chars_ਸਮਰੱਥਾ) {
        ਜੇ(capacity<(chars_ਸਮਰੱਥਾ+chars_ਸਮਰੱਥਾ/2)) {
            // avoid allocation thrashing
            capacity=chars_ਸਮਰੱਥਾ+chars_ਸਮਰੱਥਾ/2;
        }
        ਜੇ(chars!=chars_ਬਫਰ) {
            uprv_free(chars);
        }
        chars = static_cast<ਅੱਖਰ*>(uprv_malloc(capacity));
        ਜੇ(chars==ਖਾਲੀ_ਸੂਚਕ) {
            chars=chars_ਬਫਰ;
            chars_ਸਮਰੱਥਾ=ਆਕਾਰ(chars_ਬਫਰ);
            ਸਥਿਤੀ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            chars_ਸਮਰੱਥਾ=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::ਸੈੱਟ_Chars(ਅਚਲ ਅੱਖਰ* s, int32_t length, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    ਜੇ(U_SUCCESS(ਸਥਿਤੀ) && s!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *ਬਫਰ=unistr.get_ਬਫਰ(length+1);
        ਜੇ(ਬਫਰ!=ਖਾਲੀ_ਸੂਚਕ) {
            u_chars_ਮੰਜ਼ਿਲ_UChars(s, ਬਫਰ, length);
            ਬਫਰ[length]=0;
            unistr.release_ਬਫਰ(length);
            ਵਾਪਸ &unistr;
        } ਨਹੀਂਤਾਂ {
            ਸਥਿਤੀ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}
ਤਾਰਕਿਕ
StringEnumeration::ਸੰਚਾਲਕ==(ਅਚਲ StringEnumeration& that)ਅਚਲ {
    ਵਾਪਸ typeid(*ਇਹ) == typeid(that); 
}

ਤਾਰਕਿਕ
StringEnumeration::ਸੰਚਾਲਕ!=(ਅਚਲ StringEnumeration& that)ਅਚਲ {
    ਵਾਪਸ !ਸੰਚਾਲਕ==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenum_ਮੰਜ਼ਿਲ_Adopt, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        uenum_close(uenum_ਮੰਜ਼ਿਲ_Adopt);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UStringEnumeration *ਨਤੀਜਾ = ਨਵਾਂ UStringEnumeration(uenum_ਮੰਜ਼ਿਲ_Adopt);
    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        uenum_close(uenum_ਮੰਜ਼ਿਲ_Adopt);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ {
    ਵਾਪਸ uenum_ਗਿਣਤੀ(uenum, &ਸਥਿਤੀ);
}

ਅਚਲ ਅੱਖਰ* UStringEnumeration::next(int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    ਵਾਪਸ uenum_ਅਗਲਾ(uenum, ਨਤੀਜਾ_ਲੰਮਾਈ, &ਸਥਿਤੀ);
}

ਅਚਲ UnicodeString* UStringEnumeration::snext(UError_ਕੋਡ& ਸਥਿਤੀ) U_LIFETIME_BOUND {
    int32_t length;
    ਅਚਲ char16_t* str = uenum_unext(uenum, &length, &ਸਥਿਤੀ);
    ਜੇ (str == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ &unistr.ਸੈੱਟ_ਮੰਜ਼ਿਲ(str, length);
}

ਖਾਲੀ UStringEnumeration::reset(UError_ਕੋਡ& ਸਥਿਤੀ) {
    uenum_reset(uenum, &ਸਥਿਤੀ);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_ਸਿਰਾ

// C wrapper --------------------------------------------------------------- ***

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    ਮਿਟਾਓ THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ਸਥਿਰ int32_t U_CALLCONV
ustrenum_ਗਿਣਤੀ(UEnumeration* en,
               UError_ਕੋਡ* ec)
{
    ਵਾਪਸ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ਸਥਿਰ ਅਚਲ char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
               UError_ਕੋਡ* ec)
{
    ਵਾਪਸ THIS(en)->unext(ਨਤੀਜਾ_ਲੰਮਾਈ, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* U_CALLCONV
ustrenum_ਅਗਲਾ(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
              UError_ਕੋਡ* ec)
{
    ਵਾਪਸ THIS(en)->next(ਨਤੀਜਾ_ਲੰਮਾਈ, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_ਕੋਡ* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ਸਥਿਰ ਅਚਲ UEnumeration USTRENUM_VT = {
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_ਗਿਣਤੀ,
    ustrenum_unext,
    ustrenum_ਅਗਲਾ,
    ustrenum_reset
};

U_CDECL_ਸਿਰਾ

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_ਕੋਡ* ec) { 
    UEnumeration* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (U_SUCCESS(*ec) && adopted != ਖਾਲੀ_ਸੂਚਕ) {
        ਨਤੀਜਾ = (UEnumeration*) uprv_malloc(ਆਕਾਰ(UEnumeration));
        ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
            *ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            uprv_memcpy(ਨਤੀਜਾ, &USTRENUM_VT, ਆਕਾਰ(USTRENUM_VT));
            ਨਤੀਜਾ->context = adopted;
        }
    }
    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        ਮਿਟਾਓ adopted;
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UCharStringEnumeration {
    UEnumeration uenum;
    int32_t ਸੂਚਕਾਂਕ, count;
} UCharStringEnumeration;

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ਸਥਿਰ int32_t U_CALLCONV
ucharstrenum_ਗਿਣਤੀ(UEnumeration* en,
                   UError_ਕੋਡ* /*ec*/) {
    ਵਾਪਸ ((UCharStringEnumeration*)en)->count;
}

ਸਥਿਰ ਅਚਲ char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                  UError_ਕੋਡ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ਜੇ (e->ਸੂਚਕਾਂਕ >= e->count) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ char16_t* ਨਤੀਜਾ = ((ਅਚਲ char16_t**)e->uenum.context)[e->ਸੂਚਕਾਂਕ++];
    ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = u_strlen(ਨਤੀਜਾ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}


ਸਥਿਰ ਅਚਲ ਅੱਖਰ* U_CALLCONV
ucharstrenum_ਅਗਲਾ(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                  UError_ਕੋਡ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ਜੇ (e->ਸੂਚਕਾਂਕ >= e->count) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ ਅੱਖਰ* ਨਤੀਜਾ = ((ਅਚਲ ਅੱਖਰ**)e->uenum.context)[e->ਸੂਚਕਾਂਕ++];
    ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = (int32_t)uprv_strlen(ਨਤੀਜਾ);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_ਕੋਡ* /*ec*/) {
    ((UCharStringEnumeration*)en)->ਸੂਚਕਾਂਕ = 0;
}

ਸਥਿਰ ਅਚਲ UEnumeration UCHARSTRENUM_VT = {
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ਗਿਣਤੀ,
    uenum_unextDefault,
    ucharstrenum_ਅਗਲਾ,
    ucharstrenum_reset
};

ਸਥਿਰ ਅਚਲ UEnumeration UCHARSTRENUM_U_VT = {
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ਗਿਣਤੀ,
    ucharstrenum_unext,
    uenum_ਅਗਲਾ_Default,
    ucharstrenum_reset
};

U_CDECL_ਸਿਰਾ

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_ਲੜੀਆਂ_Enumeration(U_LIFETIME_BOUND ਅਚਲ ਅੱਖਰ* ਅਚਲ ਲੜੀਆਂ[],
                                 int32_t count,
                                 UError_ਕੋਡ* ec) {
    UCharStringEnumeration* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || ਲੜੀਆਂ != ਖਾਲੀ_ਸੂਚਕ)) {
        ਨਤੀਜਾ = (UCharStringEnumeration*) uprv_malloc(ਆਕਾਰ(UCharStringEnumeration));
        ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
            *ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            U_ASSERT((ਅੱਖਰ*)ਨਤੀਜਾ==(ਅੱਖਰ*)(&ਨਤੀਜਾ->uenum));
            uprv_memcpy(ਨਤੀਜਾ, &UCHARSTRENUM_VT, ਆਕਾਰ(UCHARSTRENUM_VT));
            ਨਤੀਜਾ->uenum.context = (ਖਾਲੀ*)ਲੜੀਆਂ;
            ਨਤੀਜਾ->ਸੂਚਕਾਂਕ = 0;
            ਨਤੀਜਾ->count = count;
        }
    }
    ਵਾਪਸ (UEnumeration*) ਨਤੀਜਾ;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_ਲੜੀਆਂ_Enumeration(U_LIFETIME_BOUND ਅਚਲ char16_t* ਅਚਲ ਲੜੀਆਂ[],
                                  int32_t count,
                                  UError_ਕੋਡ* ec) {
    UCharStringEnumeration* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || ਲੜੀਆਂ != ਖਾਲੀ_ਸੂਚਕ)) {
        ਨਤੀਜਾ = (UCharStringEnumeration*) uprv_malloc(ਆਕਾਰ(UCharStringEnumeration));
        ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
            *ec = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
            U_ASSERT((ਅੱਖਰ*)ਨਤੀਜਾ==(ਅੱਖਰ*)(&ਨਤੀਜਾ->uenum));
            uprv_memcpy(ਨਤੀਜਾ, &UCHARSTRENUM_U_VT, ਆਕਾਰ(UCHARSTRENUM_U_VT));
            ਨਤੀਜਾ->uenum.context = (ਖਾਲੀ*)ਲੜੀਆਂ;
            ਨਤੀਜਾ->ਸੂਚਕਾਂਕ = 0;
            ਨਤੀਜਾ->count = count;
        }
    }
    ਵਾਪਸ (UEnumeration*) ਨਤੀਜਾ;
}


// end C Wrapper
