// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uiter.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jan18
*   created by: Markus W. Scherer
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/chariter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"

U_NAMESPACE_USE

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_EVEN(n) (((n)&1)==0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_ਸੂਚਕ_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

ਸਥਿਰ int32_t U_CALLCONV
noop_Get_ਸੂਚਕਾਂਕ(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    ਵਾਪਸ 0;
}

ਸਥਿਰ int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    ਵਾਪਸ 0;
}

ਸਥਿਰ UBool U_CALLCONV
noop_Has_ਅਗਲਾ(UCharIterator * /*iter*/) {
    ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    ਵਾਪਸ U_SENTINEL;
}

ਸਥਿਰ uint32_t U_CALLCONV
noopGetState(ਅਚਲ UCharIterator * /*iter*/) {
    ਵਾਪਸ UITER_NO_STATE;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
noop_ਸੈੱਟ_State(UCharIterator * /*iter*/, uint32_t /*state*/, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
}

ਸਥਿਰ ਅਚਲ UCharIterator noopIterator={
    ਖਾਲੀ_ਸੂਚਕ, 0, 0, 0, 0, 0,
    noop_Get_ਸੂਚਕਾਂਕ,
    noopMove,
    noop_Has_ਅਗਲਾ,
    noop_Has_ਅਗਲਾ,
    noopCurrent,
    noopCurrent,
    noopCurrent,
    ਖਾਲੀ_ਸੂਚਕ,
    noopGetState,
    noop_ਸੈੱਟ_State
};

/* UCharIterator implementation for simple strings -------------------------- */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * for char16_t * strings.
 *
 * The UCharIterator.context field holds a pointer to the string.
 */

ਸਥਿਰ int32_t U_CALLCONV
string_Iterator_Get_ਸੂਚਕਾਂਕ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ਚੋਣ(origin) {
    ਹਾਲਤ UITER_ZERO:
        ਵਾਪਸ 0;
    ਹਾਲਤ UITER_ਸ਼ੁਰੂ:
        ਵਾਪਸ iter->ਸ਼ੁਰੂ;
    ਹਾਲਤ UITER_CURRENT:
        ਵਾਪਸ iter->ਸੂਚਕਾਂਕ;
    ਹਾਲਤ UITER_ਹੱਦਬੰਦੀ:
        ਵਾਪਸ iter->ਹੱਦਬੰਦੀ;
    ਹਾਲਤ UITER_ਲੰਮਾਈ:
        ਵਾਪਸ iter->length;
    ਮੂਲ:
        /* not a valid origin */
        /* Should never get here! */
        ਵਾਪਸ -1;
    }
}

ਸਥਿਰ int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    ਚੋਣ(origin) {
    ਹਾਲਤ UITER_ZERO:
        pos=delta;
        ਰੋਕੋ;
    ਹਾਲਤ UITER_ਸ਼ੁਰੂ:
        pos=iter->ਸ਼ੁਰੂ+delta;
        ਰੋਕੋ;
    ਹਾਲਤ UITER_CURRENT:
        pos=iter->ਸੂਚਕਾਂਕ+delta;
        ਰੋਕੋ;
    ਹਾਲਤ UITER_ਹੱਦਬੰਦੀ:
        pos=iter->ਹੱਦਬੰਦੀ+delta;
        ਰੋਕੋ;
    ਹਾਲਤ UITER_ਲੰਮਾਈ:
        pos=iter->length+delta;
        ਰੋਕੋ;
    ਮੂਲ:
        ਵਾਪਸ -1;  /* Error */
    }

    ਜੇ(pos<iter->ਸ਼ੁਰੂ) {
        pos=iter->ਸ਼ੁਰੂ;
    } ਨਹੀਂਤਾਂ ਜੇ(pos>iter->ਹੱਦਬੰਦੀ) {
        pos=iter->ਹੱਦਬੰਦੀ;
    }

    ਵਾਪਸ iter->ਸੂਚਕਾਂਕ=pos;
}

ਸਥਿਰ UBool U_CALLCONV
string_Iterator_Has_ਅਗਲਾ(UCharIterator *iter) {
    ਵਾਪਸ iter->ਸੂਚਕਾਂਕ<iter->ਹੱਦਬੰਦੀ;
}

ਸਥਿਰ UBool U_CALLCONV
string_Iterator_Has_ਪਿਛਲਾ(UCharIterator *iter) {
    ਵਾਪਸ iter->ਸੂਚਕਾਂਕ>iter->ਸ਼ੁਰੂ;
}

ਸਥਿਰ UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    ਜੇ(iter->ਸੂਚਕਾਂਕ<iter->ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ ((ਅਚਲ char16_t *)(iter->context))[iter->ਸੂਚਕਾਂਕ];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
string_Iterator_ਅਗਲਾ(UCharIterator *iter) {
    ਜੇ(iter->ਸੂਚਕਾਂਕ<iter->ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ ((ਅਚਲ char16_t *)(iter->context))[iter->ਸੂਚਕਾਂਕ++];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
string_Iterator_ਪਿਛਲਾ(UCharIterator *iter) {
    ਜੇ(iter->ਸੂਚਕਾਂਕ>iter->ਸ਼ੁਰੂ) {
        ਵਾਪਸ ((ਅਚਲ char16_t *)(iter->context))[--iter->ਸੂਚਕਾਂਕ];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ uint32_t U_CALLCONV
stringIteratorGetState(ਅਚਲ UCharIterator *iter) {
    ਵਾਪਸ (uint32_t)iter->ਸੂਚਕਾਂਕ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
string_Iterator_ਸੈੱਟ_State(UCharIterator *iter, uint32_t state, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        /* do nothing */
    } ਨਹੀਂਤਾਂ ਜੇ(iter==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    } ਨਹੀਂਤਾਂ ਜੇ((int32_t)state<iter->ਸ਼ੁਰੂ || iter->ਹੱਦਬੰਦੀ<(int32_t)state) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    } ਨਹੀਂਤਾਂ {
        iter->ਸੂਚਕਾਂਕ=(int32_t)state;
    }
}

ਸਥਿਰ ਅਚਲ UCharIterator stringIterator={
    ਖਾਲੀ_ਸੂਚਕ, 0, 0, 0, 0, 0,
    string_Iterator_Get_ਸੂਚਕਾਂਕ,
    stringIteratorMove,
    string_Iterator_Has_ਅਗਲਾ,
    string_Iterator_Has_ਪਿਛਲਾ,
    stringIteratorCurrent,
    string_Iterator_ਅਗਲਾ,
    string_Iterator_ਪਿਛਲਾ,
    ਖਾਲੀ_ਸੂਚਕ,
    stringIteratorGetState,
    string_Iterator_ਸੈੱਟ_State
};

U_CAPI ਖਾਲੀ U_EXPORT2
uiter_ਸੈੱਟ_String(UCharIterator *iter, ਅਚਲ char16_t *s, int32_t length) {
    ਜੇ (iter != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (s != ਖਾਲੀ_ਸੂਚਕ && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            ਜੇ(length>=0) {
                iter->length=length;
            } ਨਹੀਂਤਾਂ {
                iter->length=u_strlen(s);
            }
            iter->ਹੱਦਬੰਦੀ=iter->length;
        } ਨਹੀਂਤਾਂ {
            *iter=noopIterator;
        }
    }
}

/* UCharIterator implementation for UTF-16BE strings ------------------------ */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * for UTF-16BE strings, i.e., strings in byte-vectors where
 * each char16_t is stored as a big-endian pair of bytes.
 *
 * The UCharIterator.context field holds a pointer to the string.
 * Everything works just like with a normal char16_t iterator (uiter_setString),
 * except that UChars are assembled from byte pairs.
 */

/* internal helper function */
ਸਥਿਰ ਸਤਰਅੰਦਰ UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t ਸੂਚਕਾਂਕ) {
    ਅਚਲ uint8_t *p=(ਅਚਲ uint8_t *)iter->context;
    ਵਾਪਸ ((char16_t)p[2*ਸੂਚਕਾਂਕ]<<8)|(char16_t)p[2*ਸੂਚਕਾਂਕ+1];
}

ਸਥਿਰ UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t ਸੂਚਕਾਂਕ;

    ਜੇ((ਸੂਚਕਾਂਕ=iter->ਸੂਚਕਾਂਕ)<iter->ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ utf16BEIteratorGet(iter, ਸੂਚਕਾਂਕ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
utf16_BEIterator_ਅਗਲਾ(UCharIterator *iter) {
    int32_t ਸੂਚਕਾਂਕ;

    ਜੇ((ਸੂਚਕਾਂਕ=iter->ਸੂਚਕਾਂਕ)<iter->ਹੱਦਬੰਦੀ) {
        iter->ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ+1;
        ਵਾਪਸ utf16BEIteratorGet(iter, ਸੂਚਕਾਂਕ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
utf16_BEIterator_ਪਿਛਲਾ(UCharIterator *iter) {
    int32_t ਸੂਚਕਾਂਕ;

    ਜੇ((ਸੂਚਕਾਂਕ=iter->ਸੂਚਕਾਂਕ)>iter->ਸ਼ੁਰੂ) {
        iter->ਸੂਚਕਾਂਕ=--ਸੂਚਕਾਂਕ;
        ਵਾਪਸ utf16BEIteratorGet(iter, ਸੂਚਕਾਂਕ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ ਅਚਲ UCharIterator utf16BEIterator={
    ਖਾਲੀ_ਸੂਚਕ, 0, 0, 0, 0, 0,
    string_Iterator_Get_ਸੂਚਕਾਂਕ,
    stringIteratorMove,
    string_Iterator_Has_ਅਗਲਾ,
    string_Iterator_Has_ਪਿਛਲਾ,
    utf16BEIteratorCurrent,
    utf16_BEIterator_ਅਗਲਾ,
    utf16_BEIterator_ਪਿਛਲਾ,
    ਖਾਲੀ_ਸੂਚਕ,
    stringIteratorGetState,
    string_Iterator_ਸੈੱਟ_State
};

/*
 * Count the number of UChars in a UTF-16BE string before a terminating char16_t NUL,
 * i.e., before a pair of 0 bytes where the first 0 byte is at an even
 * offset from s.
 */
ਸਥਿਰ int32_t
utf16BE_strlen(ਅਚਲ ਅੱਖਰ *s) {
    ਜੇ(IS_ਸੂਚਕ_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        ਵਾਪਸ u_strlen((ਅਚਲ char16_t *)s);
    } ਨਹੀਂਤਾਂ {
        /* odd-aligned, search for pair of 0 bytes */
        ਅਚਲ ਅੱਖਰ *p=s;

        ਜਦੋਂ(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        ਵਾਪਸ (int32_t)((p-s)/2);
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
uiter_ਸੈੱਟ_UTF16_BE(UCharIterator *iter, ਅਚਲ ਅੱਖਰ *s, int32_t length) {
    ਜੇ(iter!=ਖਾਲੀ_ਸੂਚਕ) {
        /* allow only even-length strings (the input length counts bytes) */
        ਜੇ(s!=ਖਾਲੀ_ਸੂਚਕ && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            ਜੇ(U_IS_BIG_ENDIAN && IS_ਸੂਚਕ_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_ਸੈੱਟ_String(iter, (ਅਚਲ char16_t *)s, length);
                ਵਾਪਸ;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            ਜੇ(length>=0) {
                iter->length=length;
            } ਨਹੀਂਤਾਂ {
                iter->length=utf16BE_strlen(s);
            }
            iter->ਹੱਦਬੰਦੀ=iter->length;
        } ਨਹੀਂਤਾਂ {
            *iter=noopIterator;
        }
    }
}

/* UCharIterator wrapper around CharacterIterator --------------------------- */

/*
 * This is wrapper code around a C++ CharacterIterator to
 * look like a C UCharIterator.
 *
 * The UCharIterator.context field holds a pointer to the CharacterIterator.
 */

ਸਥਿਰ int32_t U_CALLCONV
character_Iterator_Get_ਸੂਚਕਾਂਕ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ਚੋਣ(origin) {
    ਹਾਲਤ UITER_ZERO:
        ਵਾਪਸ 0;
    ਹਾਲਤ UITER_ਸ਼ੁਰੂ:
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ();
    ਹਾਲਤ UITER_CURRENT:
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->get_ਸੂਚਕਾਂਕ();
    ਹਾਲਤ UITER_ਹੱਦਬੰਦੀ:
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->ਸਿਰਾ_ਸੂਚਕਾਂਕ();
    ਹਾਲਤ UITER_ਲੰਮਾਈ:
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->get_ਲੰਮਾਈ();
    ਮੂਲ:
        /* not a valid origin */
        /* Should never get here! */
        ਵਾਪਸ -1;
    }
}

ਸਥਿਰ int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ਚੋਣ(origin) {
    ਹਾਲਤ UITER_ZERO:
        ((CharacterIterator *)(iter->context))->ਸੈੱਟ_ਸੂਚਕਾਂਕ(delta);
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->get_ਸੂਚਕਾਂਕ();
    ਹਾਲਤ UITER_ਸ਼ੁਰੂ:
    ਹਾਲਤ UITER_CURRENT:
    ਹਾਲਤ UITER_ਹੱਦਬੰਦੀ:
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    ਹਾਲਤ UITER_ਲੰਮਾਈ:
        ((CharacterIterator *)(iter->context))->ਸੈੱਟ_ਸੂਚਕਾਂਕ(((CharacterIterator *)(iter->context))->get_ਲੰਮਾਈ()+delta);
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->get_ਸੂਚਕਾਂਕ();
    ਮੂਲ:
        /* not a valid origin */
        /* Should never get here! */
        ਵਾਪਸ -1;
    }
}

ਸਥਿਰ UBool U_CALLCONV
character_Iterator_Has_ਅਗਲਾ(UCharIterator *iter) {
    ਵਾਪਸ ((CharacterIterator *)(iter->context))->has_ਅਗਲਾ();
}

ਸਥਿਰ UBool U_CALLCONV
character_Iterator_Has_ਪਿਛਲਾ(UCharIterator *iter) {
    ਵਾਪਸ ((CharacterIterator *)(iter->context))->has_ਪਿਛਲਾ();
}

ਸਥਿਰ UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    ਜੇ(c!=0xffff || ((CharacterIterator *)(iter->context))->has_ਅਗਲਾ()) {
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
character_Iterator_ਅਗਲਾ(UCharIterator *iter) {
    ਜੇ(((CharacterIterator *)(iter->context))->has_ਅਗਲਾ()) {
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->ਅਗਲਾ_Post_Inc();
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
character_Iterator_ਪਿਛਲਾ(UCharIterator *iter) {
    ਜੇ(((CharacterIterator *)(iter->context))->has_ਪਿਛਲਾ()) {
        ਵਾਪਸ ((CharacterIterator *)(iter->context))->ਪਿਛਲਾ();
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ uint32_t U_CALLCONV
characterIteratorGetState(ਅਚਲ UCharIterator *iter) {
    ਵਾਪਸ ((CharacterIterator *)(iter->context))->get_ਸੂਚਕਾਂਕ();
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
character_Iterator_ਸੈੱਟ_State(UCharIterator *iter, uint32_t state, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        /* do nothing */
    } ਨਹੀਂਤਾਂ ਜੇ(iter==ਖਾਲੀ_ਸੂਚਕ || iter->context==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    } ਨਹੀਂਤਾਂ ਜੇ((int32_t)state<((CharacterIterator *)(iter->context))->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ() || ((CharacterIterator *)(iter->context))->ਸਿਰਾ_ਸੂਚਕਾਂਕ()<(int32_t)state) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    } ਨਹੀਂਤਾਂ {
        ((CharacterIterator *)(iter->context))->ਸੈੱਟ_ਸੂਚਕਾਂਕ((int32_t)state);
    }
}

ਸਥਿਰ ਅਚਲ UCharIterator characterIteratorWrapper={
    ਖਾਲੀ_ਸੂਚਕ, 0, 0, 0, 0, 0,
    character_Iterator_Get_ਸੂਚਕਾਂਕ,
    characterIteratorMove,
    character_Iterator_Has_ਅਗਲਾ,
    character_Iterator_Has_ਪਿਛਲਾ,
    characterIteratorCurrent,
    character_Iterator_ਅਗਲਾ,
    character_Iterator_ਪਿਛਲਾ,
    ਖਾਲੀ_ਸੂਚਕ,
    characterIteratorGetState,
    character_Iterator_ਸੈੱਟ_State
};

U_CAPI ਖਾਲੀ U_EXPORT2
uiter_ਸੈੱਟ_Character_Iterator(UCharIterator *iter, CharacterIterator *charIter) {
    ਜੇ (iter != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (charIter != ਖਾਲੀ_ਸੂਚਕ) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } ਨਹੀਂਤਾਂ {
            *iter=noopIterator;
        }
    }
}

/* UCharIterator wrapper around Replaceable --------------------------------- */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * based on a Replaceable object.
 *
 * The UCharIterator.context field holds a pointer to the Replaceable.
 * UCharIterator.length and UCharIterator.index hold Replaceable.length()
 * and the iteration index.
 */

ਸਥਿਰ UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    ਜੇ(iter->ਸੂਚਕਾਂਕ<iter->ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ ((Replaceable *)(iter->context))->charAt(iter->ਸੂਚਕਾਂਕ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
replaceable_Iterator_ਅਗਲਾ(UCharIterator *iter) {
    ਜੇ(iter->ਸੂਚਕਾਂਕ<iter->ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ ((Replaceable *)(iter->context))->charAt(iter->ਸੂਚਕਾਂਕ++);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
replaceable_Iterator_ਪਿਛਲਾ(UCharIterator *iter) {
    ਜੇ(iter->ਸੂਚਕਾਂਕ>iter->ਸ਼ੁਰੂ) {
        ਵਾਪਸ ((Replaceable *)(iter->context))->charAt(--iter->ਸੂਚਕਾਂਕ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ ਅਚਲ UCharIterator replaceableIterator={
    ਖਾਲੀ_ਸੂਚਕ, 0, 0, 0, 0, 0,
    string_Iterator_Get_ਸੂਚਕਾਂਕ,
    stringIteratorMove,
    string_Iterator_Has_ਅਗਲਾ,
    string_Iterator_Has_ਪਿਛਲਾ,
    replaceableIteratorCurrent,
    replaceable_Iterator_ਅਗਲਾ,
    replaceable_Iterator_ਪਿਛਲਾ,
    ਖਾਲੀ_ਸੂਚਕ,
    stringIteratorGetState,
    string_Iterator_ਸੈੱਟ_State
};

U_CAPI ਖਾਲੀ U_EXPORT2
uiter_ਸੈੱਟ_Replaceable(UCharIterator *iter, ਅਚਲ Replaceable *rep) {
    ਜੇ (iter != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (rep != ਖਾਲੀ_ਸੂਚਕ) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->ਹੱਦਬੰਦੀ=iter->length=rep->length();
        } ਨਹੀਂਤਾਂ {
            *iter=noopIterator;
        }
    }
}

/* UCharIterator implementation for UTF-8 strings --------------------------- */

/*
 * Possible, probably necessary only for an implementation for arbitrary
 * converters:
 * Maintain a buffer (ring buffer?) for a piece of converted 16-bit text.
 * This would require to turn reservedFn into a close function and
 * to introduce a uiter_close(iter).
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UITER_CNV_ਸਮਰੱਥਾ 16

/*
 * Minimal implementation:
 * Maintain a single-char16_t buffer for an additional surrogate.
 * The caller must not modify start and limit because they are used internally.
 *
 * Use UCharIterator fields as follows:
 *   context        pointer to UTF-8 string
 *   length         UTF-16 length of the string; -1 until lazy evaluation
 *   start          current UTF-8 index
 *   index          current UTF-16 index; may be -1="unknown" after setState()
 *   limit          UTF-8 length of the string
 *   reservedField  supplementary code point
 *
 * Since UCharIterator delivers 16-bit code units, the iteration can be
 * currently in the middle of the byte sequence for a supplementary code point.
 * In this case, reservedField will contain that code point and start will
 * point to after the corresponding byte sequence. The UTF-16 index will be
 * one less than what it would otherwise be corresponding to the UTF-8 index.
 * Otherwise, reservedField will be 0.
 */

/*
 * Possible optimization for NUL-terminated UTF-8 and UTF-16 strings:
 * Add implementations that do not call strlen() for iteration but check for NUL.
 */

ਸਥਿਰ int32_t U_CALLCONV
utf8_Iterator_Get_ਸੂਚਕਾਂਕ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ਚੋਣ(origin) {
    ਹਾਲਤ UITER_ZERO:
    ਹਾਲਤ UITER_ਸ਼ੁਰੂ:
        ਵਾਪਸ 0;
    ਹਾਲਤ UITER_CURRENT:
        ਜੇ(iter->ਸੂਚਕਾਂਕ<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ਅਚਲ uint8_t *s;
            UChar32 c;
            int32_t i, ਹੱਦਬੰਦੀ, ਸੂਚਕਾਂਕ;

            s=(ਅਚਲ uint8_t *)iter->context;
            i=ਸੂਚਕਾਂਕ=0;
            ਹੱਦਬੰਦੀ=iter->ਸ਼ੁਰੂ; /* count up to the UTF-8 index */
            ਜਦੋਂ(i<ਹੱਦਬੰਦੀ) {
                U8_ਅਗਲਾ_OR_FFFD(s, i, ਹੱਦਬੰਦੀ, c);
                ਸੂਚਕਾਂਕ+=U16_ਲੰਮਾਈ(c);
            }

            iter->ਸ਼ੁਰੂ=i; /* just in case setState() did not get us to a code point boundary */
            ਜੇ(i==iter->ਹੱਦਬੰਦੀ) {
                iter->length=ਸੂਚਕਾਂਕ; /* in case it was <0 or wrong */
            }
            ਜੇ(iter->reserved_ਖੇਤਰ!=0) {
                --ਸੂਚਕਾਂਕ; /* we are in the middle of a supplementary code point */
            }
            iter->ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ;
        }
        ਵਾਪਸ iter->ਸੂਚਕਾਂਕ;
    ਹਾਲਤ UITER_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ UITER_ਲੰਮਾਈ:
        ਜੇ(iter->length<0) {
            ਅਚਲ uint8_t *s;
            UChar32 c;
            int32_t i, ਹੱਦਬੰਦੀ, length;

            s=(ਅਚਲ uint8_t *)iter->context;
            ਜੇ(iter->ਸੂਚਕਾਂਕ<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                ਹੱਦਬੰਦੀ=iter->ਸ਼ੁਰੂ;

                /* count from the beginning to the current index */
                ਜਦੋਂ(i<ਹੱਦਬੰਦੀ) {
                    U8_ਅਗਲਾ_OR_FFFD(s, i, ਹੱਦਬੰਦੀ, c);
                    length+=U16_ਲੰਮਾਈ(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->ਸ਼ੁਰੂ=i; /* just in case setState() did not get us to a code point boundary */
                iter->ਸੂਚਕਾਂਕ= iter->reserved_ਖੇਤਰ!=0 ? length-1 : length;
            } ਨਹੀਂਤਾਂ {
                i=iter->ਸ਼ੁਰੂ;
                length=iter->ਸੂਚਕਾਂਕ;
                ਜੇ(iter->reserved_ਖੇਤਰ!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            ਹੱਦਬੰਦੀ=iter->ਹੱਦਬੰਦੀ;
            ਜਦੋਂ(i<ਹੱਦਬੰਦੀ) {
                U8_ਅਗਲਾ_OR_FFFD(s, i, ਹੱਦਬੰਦੀ, c);
                length+=U16_ਲੰਮਾਈ(c);
            }
            iter->length=length;
        }
        ਵਾਪਸ iter->length;
    ਮੂਲ:
        /* not a valid origin */
        /* Should never get here! */
        ਵਾਪਸ -1;
    }
}

ਸਥਿਰ int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ਅਚਲ uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    ਚੋਣ(origin) {
    ਹਾਲਤ UITER_ZERO:
    ਹਾਲਤ UITER_ਸ਼ੁਰੂ:
        pos=delta;
        havePos=ਸੱਚ;
        /* iter->index<0 (unknown) is possible */
        ਰੋਕੋ;
    ਹਾਲਤ UITER_CURRENT:
        ਜੇ(iter->ਸੂਚਕਾਂਕ>=0) {
            pos=iter->ਸੂਚਕਾਂਕ+delta;
            havePos=ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=ਝੂਠ;
        }
        ਰੋਕੋ;
    ਹਾਲਤ UITER_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ UITER_ਲੰਮਾਈ:
        ਜੇ(iter->length>=0) {
            pos=iter->length+delta;
            havePos=ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
            /* pin to the end, avoid counting the length */
            iter->ਸੂਚਕਾਂਕ=-1;
            iter->ਸ਼ੁਰੂ=iter->ਹੱਦਬੰਦੀ;
            iter->reserved_ਖੇਤਰ=0;
            ਜੇ(delta>=0) {
                ਵਾਪਸ UITER_UNKNOWN_ਸੂਚਕਾਂਕ;
            } ਨਹੀਂਤਾਂ {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=ਝੂਠ;
            }
        }
        ਰੋਕੋ;
    ਮੂਲ:
        ਵਾਪਸ -1;  /* Error */
    }

    ਜੇ(havePos) {
        /* shortcuts: pinning to the edges of the string */
        ਜੇ(pos<=0) {
            iter->ਸੂਚਕਾਂਕ=iter->ਸ਼ੁਰੂ=iter->reserved_ਖੇਤਰ=0;
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ ਜੇ(iter->length>=0 && pos>=iter->length) {
            iter->ਸੂਚਕਾਂਕ=iter->length;
            iter->ਸ਼ੁਰੂ=iter->ਹੱਦਬੰਦੀ;
            iter->reserved_ਖੇਤਰ=0;
            ਵਾਪਸ iter->ਸੂਚਕਾਂਕ;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        ਜੇ(iter->ਸੂਚਕਾਂਕ<0 || pos<iter->ਸੂਚਕਾਂਕ/2) {
            /* go forward from the start instead of backward from the current index */
            iter->ਸੂਚਕਾਂਕ=iter->ਸ਼ੁਰੂ=iter->reserved_ਖੇਤਰ=0;
        } ਨਹੀਂਤਾਂ ਜੇ(iter->length>=0 && (iter->length-pos)<(pos-iter->ਸੂਚਕਾਂਕ)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->ਸੂਚਕਾਂਕ=iter->length;
            iter->ਸ਼ੁਰੂ=iter->ਹੱਦਬੰਦੀ;
            iter->reserved_ਖੇਤਰ=0;
        }

        delta=pos-iter->ਸੂਚਕਾਂਕ;
        ਜੇ(delta==0) {
            ਵਾਪਸ iter->ਸੂਚਕਾਂਕ; /* nothing to do */
        }
    } ਨਹੀਂਤਾਂ {
        /* move relative to unknown UTF-16 index */
        ਜੇ(delta==0) {
            ਵਾਪਸ UITER_UNKNOWN_ਸੂਚਕਾਂਕ; /* nothing to do */
        } ਨਹੀਂਤਾਂ ਜੇ(-delta>=iter->ਸ਼ੁਰੂ) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->ਸੂਚਕਾਂਕ=iter->ਸ਼ੁਰੂ=iter->reserved_ਖੇਤਰ=0;
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ ਜੇ(delta>=(iter->ਹੱਦਬੰਦੀ-iter->ਸ਼ੁਰੂ)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->ਸੂਚਕਾਂਕ=iter->length; /* may or may not be <0 (unknown) */
            iter->ਸ਼ੁਰੂ=iter->ਹੱਦਬੰਦੀ;
            iter->reserved_ਖੇਤਰ=0;
            ਵਾਪਸ iter->ਸੂਚਕਾਂਕ>=0 ? iter->ਸੂਚਕਾਂਕ : (int32_t)UITER_UNKNOWN_ਸੂਚਕਾਂਕ;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ਅਚਲ uint8_t *)iter->context;
    pos=iter->ਸੂਚਕਾਂਕ; /* could be <0 (unknown) */
    i=iter->ਸ਼ੁਰੂ;
    ਜੇ(delta>0) {
        /* go forward */
        int32_t ਹੱਦਬੰਦੀ=iter->ਹੱਦਬੰਦੀ;
        ਜੇ(iter->reserved_ਖੇਤਰ!=0) {
            iter->reserved_ਖੇਤਰ=0;
            ++pos;
            --delta;
        }
        ਜਦੋਂ(delta>0 && i<ਹੱਦਬੰਦੀ) {
            U8_ਅਗਲਾ_OR_FFFD(s, i, ਹੱਦਬੰਦੀ, c);
            ਜੇ(c<=0xffff) {
                ++pos;
                --delta;
            } ਨਹੀਂਤਾਂ ਜੇ(delta>=2) {
                pos+=2;
                delta-=2;
            } ਨਹੀਂਤਾਂ /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reserved_ਖੇਤਰ=c;
                ++pos;
                ਰੋਕੋ; /* delta=0; */
            }
        }
        ਜੇ(i==ਹੱਦਬੰਦੀ) {
            ਜੇ(iter->length<0 && iter->ਸੂਚਕਾਂਕ>=0) {
                iter->length= iter->reserved_ਖੇਤਰ==0 ? pos : pos+1;
            } ਨਹੀਂਤਾਂ ਜੇ(iter->ਸੂਚਕਾਂਕ<0 && iter->length>=0) {
                iter->ਸੂਚਕਾਂਕ= iter->reserved_ਖੇਤਰ==0 ? iter->length : iter->length-1;
            }
        }
    } ਨਹੀਂਤਾਂ /* delta<0 */ {
        /* go backward */
        ਜੇ(iter->reserved_ਖੇਤਰ!=0) {
            iter->reserved_ਖੇਤਰ=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        ਜਦੋਂ(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            ਜੇ(c<=0xffff) {
                --pos;
                ++delta;
            } ਨਹੀਂਤਾਂ ਜੇ(delta<=-2) {
                pos-=2;
                delta+=2;
            } ਨਹੀਂਤਾਂ /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reserved_ਖੇਤਰ=c;
                --pos;
                ਰੋਕੋ; /* delta=0; */
            }
        }
    }

    iter->ਸ਼ੁਰੂ=i;
    ਜੇ(iter->ਸੂਚਕਾਂਕ>=0) {
        ਵਾਪਸ iter->ਸੂਚਕਾਂਕ=pos;
    } ਨਹੀਂਤਾਂ {
        /* we started with index<0 (unknown) so pos is bogus */
        ਜੇ(i<=1) {
            ਵਾਪਸ iter->ਸੂਚਕਾਂਕ=i; /* reached the beginning */
        } ਨਹੀਂਤਾਂ {
            /* we still don't know the UTF-16 index */
            ਵਾਪਸ UITER_UNKNOWN_ਸੂਚਕਾਂਕ;
        }
    }
}

ਸਥਿਰ UBool U_CALLCONV
utf8_Iterator_Has_ਅਗਲਾ(UCharIterator *iter) {
    ਵਾਪਸ iter->ਸ਼ੁਰੂ<iter->ਹੱਦਬੰਦੀ || iter->reserved_ਖੇਤਰ!=0;
}

ਸਥਿਰ UBool U_CALLCONV
utf8_Iterator_Has_ਪਿਛਲਾ(UCharIterator *iter) {
    ਵਾਪਸ iter->ਸ਼ੁਰੂ>0;
}

ਸਥਿਰ UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    ਜੇ(iter->reserved_ਖੇਤਰ!=0) {
        ਵਾਪਸ U16_TRAIL(iter->reserved_ਖੇਤਰ);
    } ਨਹੀਂਤਾਂ ਜੇ(iter->ਸ਼ੁਰੂ<iter->ਹੱਦਬੰਦੀ) {
        ਅਚਲ uint8_t *s=(ਅਚਲ uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->ਸ਼ੁਰੂ;

        U8_ਅਗਲਾ_OR_FFFD(s, i, iter->ਹੱਦਬੰਦੀ, c);
        ਜੇ(c<=0xffff) {
            ਵਾਪਸ c;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ U16_LEAD(c);
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
utf8_Iterator_ਅਗਲਾ(UCharIterator *iter) {
    int32_t ਸੂਚਕਾਂਕ;

    ਜੇ(iter->reserved_ਖੇਤਰ!=0) {
        char16_t trail=U16_TRAIL(iter->reserved_ਖੇਤਰ);
        iter->reserved_ਖੇਤਰ=0;
        ਜੇ((ਸੂਚਕਾਂਕ=iter->ਸੂਚਕਾਂਕ)>=0) {
            iter->ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ+1;
        }
        ਵਾਪਸ trail;
    } ਨਹੀਂਤਾਂ ਜੇ(iter->ਸ਼ੁਰੂ<iter->ਹੱਦਬੰਦੀ) {
        ਅਚਲ uint8_t *s=(ਅਚਲ uint8_t *)iter->context;
        UChar32 c;

        U8_ਅਗਲਾ_OR_FFFD(s, iter->ਸ਼ੁਰੂ, iter->ਹੱਦਬੰਦੀ, c);
        ਜੇ((ਸੂਚਕਾਂਕ=iter->ਸੂਚਕਾਂਕ)>=0) {
            iter->ਸੂਚਕਾਂਕ=++ਸੂਚਕਾਂਕ;
            ਜੇ(iter->length<0 && iter->ਸ਼ੁਰੂ==iter->ਹੱਦਬੰਦੀ) {
                iter->length= c<=0xffff ? ਸੂਚਕਾਂਕ : ਸੂਚਕਾਂਕ+1;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(iter->ਸ਼ੁਰੂ==iter->ਹੱਦਬੰਦੀ && iter->length>=0) {
            iter->ਸੂਚਕਾਂਕ= c<=0xffff ? iter->length : iter->length-1;
        }
        ਜੇ(c<=0xffff) {
            ਵਾਪਸ c;
        } ਨਹੀਂਤਾਂ {
            iter->reserved_ਖੇਤਰ=c;
            ਵਾਪਸ U16_LEAD(c);
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ UChar32 U_CALLCONV
utf8_Iterator_ਪਿਛਲਾ(UCharIterator *iter) {
    int32_t ਸੂਚਕਾਂਕ;

    ਜੇ(iter->reserved_ਖੇਤਰ!=0) {
        char16_t lead=U16_LEAD(iter->reserved_ਖੇਤਰ);
        iter->reserved_ਖੇਤਰ=0;
        iter->ਸ਼ੁਰੂ-=4; /* we stayed behind the supplementary code point; go before it now */
        ਜੇ((ਸੂਚਕਾਂਕ=iter->ਸੂਚਕਾਂਕ)>0) {
            iter->ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ-1;
        }
        ਵਾਪਸ lead;
    } ਨਹੀਂਤਾਂ ਜੇ(iter->ਸ਼ੁਰੂ>0) {
        ਅਚਲ uint8_t *s=(ਅਚਲ uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->ਸ਼ੁਰੂ, c);
        ਜੇ((ਸੂਚਕਾਂਕ=iter->ਸੂਚਕਾਂਕ)>0) {
            iter->ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ-1;
        } ਨਹੀਂਤਾਂ ਜੇ(iter->ਸ਼ੁਰੂ<=1) {
            iter->ਸੂਚਕਾਂਕ= c<=0xffff ? iter->ਸ਼ੁਰੂ : iter->ਸ਼ੁਰੂ+1;
        }
        ਜੇ(c<=0xffff) {
            ਵਾਪਸ c;
        } ਨਹੀਂਤਾਂ {
            iter->ਸ਼ੁਰੂ+=4; /* back to behind this supplementary code point for consistent state */
            iter->reserved_ਖੇਤਰ=c;
            ਵਾਪਸ U16_TRAIL(c);
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ U_SENTINEL;
    }
}

ਸਥਿਰ uint32_t U_CALLCONV
utf8IteratorGetState(ਅਚਲ UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->ਸ਼ੁਰੂ<<1);
    ਜੇ(iter->reserved_ਖੇਤਰ!=0) {
        state|=1;
    }
    ਵਾਪਸ state;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
utf8_Iterator_ਸੈੱਟ_State(UCharIterator *iter,
                     uint32_t state,
                     UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        /* do nothing */
    } ਨਹੀਂਤਾਂ ਜੇ(iter==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    } ਨਹੀਂਤਾਂ ਜੇ(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } ਨਹੀਂਤਾਂ {
        int32_t ਸੂਚਕਾਂਕ=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        ਜੇ((state==0 ? ਸੂਚਕਾਂਕ<0 : ਸੂਚਕਾਂਕ<4) || iter->ਹੱਦਬੰਦੀ<ਸੂਚਕਾਂਕ) {
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        } ਨਹੀਂਤਾਂ {
            iter->ਸ਼ੁਰੂ=ਸੂਚਕਾਂਕ; /* restore UTF-8 byte index */
            ਜੇ(ਸੂਚਕਾਂਕ<=1) {
                iter->ਸੂਚਕਾਂਕ=ਸੂਚਕਾਂਕ;
            } ਨਹੀਂਤਾਂ {
                iter->ਸੂਚਕਾਂਕ=-1; /* unknown UTF-16 index */
            }
            ਜੇ(state==0) {
                iter->reserved_ਖੇਤਰ=0;
            } ਨਹੀਂਤਾਂ {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((ਅਚਲ uint8_t *)iter->context, 0, ਸੂਚਕਾਂਕ, c);
                ਜੇ(c<=0xffff) {
                    *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                } ਨਹੀਂਤਾਂ {
                    iter->reserved_ਖੇਤਰ=c;
                }
            }
        }
    }
}

ਸਥਿਰ ਅਚਲ UCharIterator utf8Iterator={
    ਖਾਲੀ_ਸੂਚਕ, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_ਸੂਚਕਾਂਕ,
    utf8IteratorMove,
    utf8_Iterator_Has_ਅਗਲਾ,
    utf8_Iterator_Has_ਪਿਛਲਾ,
    utf8IteratorCurrent,
    utf8_Iterator_ਅਗਲਾ,
    utf8_Iterator_ਪਿਛਲਾ,
    ਖਾਲੀ_ਸੂਚਕ,
    utf8IteratorGetState,
    utf8_Iterator_ਸੈੱਟ_State
};

U_CAPI ਖਾਲੀ U_EXPORT2
uiter_ਸੈੱਟ_UTF8(UCharIterator *iter, ਅਚਲ ਅੱਖਰ *s, int32_t length) {
    ਜੇ (iter != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (s != ਖਾਲੀ_ਸੂਚਕ && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            ਜੇ(length>=0) {
                iter->ਹੱਦਬੰਦੀ=length;
            } ਨਹੀਂਤਾਂ {
                iter->ਹੱਦਬੰਦੀ=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->ਹੱਦਬੰਦੀ<=1 ? iter->ਹੱਦਬੰਦੀ : -1;
        } ਨਹੀਂਤਾਂ {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    ਜੇ(U16_IS_SURROGATE(c)) {
        ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            ਜੇ(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } ਨਹੀਂਤਾਂ {
            ਜੇ(U16_IS_LEAD(c2=iter->ਪਿਛਲਾ(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            ਜੇ(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    ਵਾਪਸ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    ਜੇ(U16_IS_LEAD(c)) {
        ਜੇ(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } ਨਹੀਂਤਾਂ ਜੇ(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    ਵਾਪਸ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->ਪਿਛਲਾ(iter);
    ਜੇ(U16_IS_TRAIL(c)) {
        ਜੇ(U16_IS_LEAD(c2=iter->ਪਿਛਲਾ(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } ਨਹੀਂਤਾਂ ਜੇ(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    ਵਾਪਸ c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ਅਚਲ UCharIterator *iter) {
    ਜੇ(iter==ਖਾਲੀ_ਸੂਚਕ || iter->getState==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ UITER_NO_STATE;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ iter->getState(iter);
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
uiter_ਸੈੱਟ_State(UCharIterator *iter, uint32_t state, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        /* do nothing */
    } ਨਹੀਂਤਾਂ ਜੇ(iter==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    } ਨਹੀਂਤਾਂ ਜੇ(iter->ਸੈੱਟ_State==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
    } ਨਹੀਂਤਾਂ {
        iter->ਸੈੱਟ_State(iter, state, p_Error_ਕੋਡ);
    }
}

U_CDECL_ਸਿਰਾ
