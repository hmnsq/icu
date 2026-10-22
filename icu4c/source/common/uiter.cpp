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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/chariter.h"
#ಅನ್ತರ್ಭಾವ "unicode/rep.h"
#ಅನ್ತರ್ಭಾವ "unicode/uiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"

U_NAMESPACE_USE

#ವ್ಯಾಖ್ಯಾನಿಸು IS_EVEN(n) (((n)&1)==0)
#ವ್ಯಾಖ್ಯಾನಿಸು IS_ಸೂಚಕ_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

ಸ್ಥಿರ int32_t U_CALLCONV
noop_Get_ಸೂಚ್ಯಂಕ(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ UBool U_CALLCONV
noop_Has_ಮುಂದಿನ(UCharIterator * /*iter*/) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    ಹಿಂತಿರುಗಿ U_SENTINEL;
}

ಸ್ಥಿರ uint32_t U_CALLCONV
noopGetState(ಸ್ಥಿರಾಂಕ UCharIterator * /*iter*/) {
    ಹಿಂತಿರುಗಿ UITER_NO_STATE;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
noop_ಗಣ_State(UCharIterator * /*iter*/, uint32_t /*state*/, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UCharIterator noopIterator={
    ಶೂನ್ಯಸೂಚಕ, 0, 0, 0, 0, 0,
    noop_Get_ಸೂಚ್ಯಂಕ,
    noopMove,
    noop_Has_ಮುಂದಿನ,
    noop_Has_ಮುಂದಿನ,
    noopCurrent,
    noopCurrent,
    noopCurrent,
    ಶೂನ್ಯಸೂಚಕ,
    noopGetState,
    noop_ಗಣ_State
};

/* UCharIterator implementation for simple strings -------------------------- */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * for char16_t * strings.
 *
 * The UCharIterator.context field holds a pointer to the string.
 */

ಸ್ಥಿರ int32_t U_CALLCONV
string_Iterator_Get_ಸೂಚ್ಯಂಕ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ಆಯ್ಕೆ(origin) {
    ಸ್ಥಿತಿ UITER_ZERO:
        ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ UITER_ಆರಂಭ:
        ಹಿಂತಿರುಗಿ iter->ಆರಂಭ;
    ಸ್ಥಿತಿ UITER_CURRENT:
        ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿತಿ UITER_ಎಲ್ಲೆ:
        ಹಿಂತಿರುಗಿ iter->ಎಲ್ಲೆ;
    ಸ್ಥಿತಿ UITER_ಉದ್ದಳತೆ:
        ಹಿಂತಿರುಗಿ iter->length;
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* not a valid origin */
        /* Should never get here! */
        ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    ಆಯ್ಕೆ(origin) {
    ಸ್ಥಿತಿ UITER_ZERO:
        pos=delta;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UITER_ಆರಂಭ:
        pos=iter->ಆರಂಭ+delta;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UITER_CURRENT:
        pos=iter->ಸೂಚ್ಯಂಕ+delta;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UITER_ಎಲ್ಲೆ:
        pos=iter->ಎಲ್ಲೆ+delta;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UITER_ಉದ್ದಳತೆ:
        pos=iter->length+delta;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ -1;  /* Error */
    }

    ಆದರೆ(pos<iter->ಆರಂಭ) {
        pos=iter->ಆರಂಭ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pos>iter->ಎಲ್ಲೆ) {
        pos=iter->ಎಲ್ಲೆ;
    }

    ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ=pos;
}

ಸ್ಥಿರ UBool U_CALLCONV
string_Iterator_Has_ಮುಂದಿನ(UCharIterator *iter) {
    ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ<iter->ಎಲ್ಲೆ;
}

ಸ್ಥಿರ UBool U_CALLCONV
string_Iterator_Has_ಹಿಂದಿನ(UCharIterator *iter) {
    ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ>iter->ಆರಂಭ;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    ಆದರೆ(iter->ಸೂಚ್ಯಂಕ<iter->ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ char16_t *)(iter->context))[iter->ಸೂಚ್ಯಂಕ];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
string_Iterator_ಮುಂದಿನ(UCharIterator *iter) {
    ಆದರೆ(iter->ಸೂಚ್ಯಂಕ<iter->ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ char16_t *)(iter->context))[iter->ಸೂಚ್ಯಂಕ++];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
string_Iterator_ಹಿಂದಿನ(UCharIterator *iter) {
    ಆದರೆ(iter->ಸೂಚ್ಯಂಕ>iter->ಆರಂಭ) {
        ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ char16_t *)(iter->context))[--iter->ಸೂಚ್ಯಂಕ];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ uint32_t U_CALLCONV
stringIteratorGetState(ಸ್ಥಿರಾಂಕ UCharIterator *iter) {
    ಹಿಂತಿರುಗಿ (uint32_t)iter->ಸೂಚ್ಯಂಕ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
string_Iterator_ಗಣ_State(UCharIterator *iter, uint32_t state, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        /* do nothing */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((int32_t)state<iter->ಆರಂಭ || iter->ಎಲ್ಲೆ<(int32_t)state) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        iter->ಸೂಚ್ಯಂಕ=(int32_t)state;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UCharIterator stringIterator={
    ಶೂನ್ಯಸೂಚಕ, 0, 0, 0, 0, 0,
    string_Iterator_Get_ಸೂಚ್ಯಂಕ,
    stringIteratorMove,
    string_Iterator_Has_ಮುಂದಿನ,
    string_Iterator_Has_ಹಿಂದಿನ,
    stringIteratorCurrent,
    string_Iterator_ಮುಂದಿನ,
    string_Iterator_ಹಿಂದಿನ,
    ಶೂನ್ಯಸೂಚಕ,
    stringIteratorGetState,
    string_Iterator_ಗಣ_State
};

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_ಗಣ_String(UCharIterator *iter, ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
    ಆದರೆ (iter != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (s != ಶೂನ್ಯಸೂಚಕ && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            ಆದರೆ(length>=0) {
                iter->length=length;
            } ಇಲ್ಲದಿದ್ದರೆ {
                iter->length=u_strlen(s);
            }
            iter->ಎಲ್ಲೆ=iter->length;
        } ಇಲ್ಲದಿದ್ದರೆ {
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t ಸೂಚ್ಯಂಕ) {
    ಸ್ಥಿರಾಂಕ uint8_t *p=(ಸ್ಥಿರಾಂಕ uint8_t *)iter->context;
    ಹಿಂತಿರುಗಿ ((char16_t)p[2*ಸೂಚ್ಯಂಕ]<<8)|(char16_t)p[2*ಸೂಚ್ಯಂಕ+1];
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t ಸೂಚ್ಯಂಕ;

    ಆದರೆ((ಸೂಚ್ಯಂಕ=iter->ಸೂಚ್ಯಂಕ)<iter->ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ utf16BEIteratorGet(iter, ಸೂಚ್ಯಂಕ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf16_BEIterator_ಮುಂದಿನ(UCharIterator *iter) {
    int32_t ಸೂಚ್ಯಂಕ;

    ಆದರೆ((ಸೂಚ್ಯಂಕ=iter->ಸೂಚ್ಯಂಕ)<iter->ಎಲ್ಲೆ) {
        iter->ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ+1;
        ಹಿಂತಿರುಗಿ utf16BEIteratorGet(iter, ಸೂಚ್ಯಂಕ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf16_BEIterator_ಹಿಂದಿನ(UCharIterator *iter) {
    int32_t ಸೂಚ್ಯಂಕ;

    ಆದರೆ((ಸೂಚ್ಯಂಕ=iter->ಸೂಚ್ಯಂಕ)>iter->ಆರಂಭ) {
        iter->ಸೂಚ್ಯಂಕ=--ಸೂಚ್ಯಂಕ;
        ಹಿಂತಿರುಗಿ utf16BEIteratorGet(iter, ಸೂಚ್ಯಂಕ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UCharIterator utf16BEIterator={
    ಶೂನ್ಯಸೂಚಕ, 0, 0, 0, 0, 0,
    string_Iterator_Get_ಸೂಚ್ಯಂಕ,
    stringIteratorMove,
    string_Iterator_Has_ಮುಂದಿನ,
    string_Iterator_Has_ಹಿಂದಿನ,
    utf16BEIteratorCurrent,
    utf16_BEIterator_ಮುಂದಿನ,
    utf16_BEIterator_ಹಿಂದಿನ,
    ಶೂನ್ಯಸೂಚಕ,
    stringIteratorGetState,
    string_Iterator_ಗಣ_State
};

/*
 * Count the number of UChars in a UTF-16BE string before a terminating char16_t NUL,
 * i.e., before a pair of 0 bytes where the first 0 byte is at an even
 * offset from s.
 */
ಸ್ಥಿರ int32_t
utf16BE_strlen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
    ಆದರೆ(IS_ಸೂಚಕ_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        ಹಿಂತಿರುಗಿ u_strlen((ಸ್ಥಿರಾಂಕ char16_t *)s);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* odd-aligned, search for pair of 0 bytes */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p=s;

        ಆಗ(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        ಹಿಂತಿರುಗಿ (int32_t)((p-s)/2);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_ಗಣ_UTF16_BE(UCharIterator *iter, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t length) {
    ಆದರೆ(iter!=ಶೂನ್ಯಸೂಚಕ) {
        /* allow only even-length strings (the input length counts bytes) */
        ಆದರೆ(s!=ಶೂನ್ಯಸೂಚಕ && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            ಆದರೆ(U_IS_BIG_ENDIAN && IS_ಸೂಚಕ_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_ಗಣ_String(iter, (ಸ್ಥಿರಾಂಕ char16_t *)s, length);
                ಹಿಂತಿರುಗಿ;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            ಆದರೆ(length>=0) {
                iter->length=length;
            } ಇಲ್ಲದಿದ್ದರೆ {
                iter->length=utf16BE_strlen(s);
            }
            iter->ಎಲ್ಲೆ=iter->length;
        } ಇಲ್ಲದಿದ್ದರೆ {
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

ಸ್ಥಿರ int32_t U_CALLCONV
character_Iterator_Get_ಸೂಚ್ಯಂಕ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ಆಯ್ಕೆ(origin) {
    ಸ್ಥಿತಿ UITER_ZERO:
        ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ UITER_ಆರಂಭ:
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->ಆರಂಭ_ಸೂಚ್ಯಂಕ();
    ಸ್ಥಿತಿ UITER_CURRENT:
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->get_ಸೂಚ್ಯಂಕ();
    ಸ್ಥಿತಿ UITER_ಎಲ್ಲೆ:
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->ಕೊನೆ_ಸೂಚ್ಯಂಕ();
    ಸ್ಥಿತಿ UITER_ಉದ್ದಳತೆ:
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->get_ಉದ್ದಳತೆ();
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* not a valid origin */
        /* Should never get here! */
        ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ಆಯ್ಕೆ(origin) {
    ಸ್ಥಿತಿ UITER_ZERO:
        ((CharacterIterator *)(iter->context))->ಗಣ_ಸೂಚ್ಯಂಕ(delta);
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->get_ಸೂಚ್ಯಂಕ();
    ಸ್ಥಿತಿ UITER_ಆರಂಭ:
    ಸ್ಥಿತಿ UITER_CURRENT:
    ಸ್ಥಿತಿ UITER_ಎಲ್ಲೆ:
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    ಸ್ಥಿತಿ UITER_ಉದ್ದಳತೆ:
        ((CharacterIterator *)(iter->context))->ಗಣ_ಸೂಚ್ಯಂಕ(((CharacterIterator *)(iter->context))->get_ಉದ್ದಳತೆ()+delta);
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->get_ಸೂಚ್ಯಂಕ();
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* not a valid origin */
        /* Should never get here! */
        ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ UBool U_CALLCONV
character_Iterator_Has_ಮುಂದಿನ(UCharIterator *iter) {
    ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->has_ಮುಂದಿನ();
}

ಸ್ಥಿರ UBool U_CALLCONV
character_Iterator_Has_ಹಿಂದಿನ(UCharIterator *iter) {
    ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->has_ಹಿಂದಿನ();
}

ಸ್ಥಿರ UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    ಆದರೆ(c!=0xffff || ((CharacterIterator *)(iter->context))->has_ಮುಂದಿನ()) {
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
character_Iterator_ಮುಂದಿನ(UCharIterator *iter) {
    ಆದರೆ(((CharacterIterator *)(iter->context))->has_ಮುಂದಿನ()) {
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->ಮುಂದಿನ_Post_Inc();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
character_Iterator_ಹಿಂದಿನ(UCharIterator *iter) {
    ಆದರೆ(((CharacterIterator *)(iter->context))->has_ಹಿಂದಿನ()) {
        ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->ಹಿಂದಿನ();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ uint32_t U_CALLCONV
characterIteratorGetState(ಸ್ಥಿರಾಂಕ UCharIterator *iter) {
    ಹಿಂತಿರುಗಿ ((CharacterIterator *)(iter->context))->get_ಸೂಚ್ಯಂಕ();
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
character_Iterator_ಗಣ_State(UCharIterator *iter, uint32_t state, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        /* do nothing */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter==ಶೂನ್ಯಸೂಚಕ || iter->context==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((int32_t)state<((CharacterIterator *)(iter->context))->ಆರಂಭ_ಸೂಚ್ಯಂಕ() || ((CharacterIterator *)(iter->context))->ಕೊನೆ_ಸೂಚ್ಯಂಕ()<(int32_t)state) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ((CharacterIterator *)(iter->context))->ಗಣ_ಸೂಚ್ಯಂಕ((int32_t)state);
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UCharIterator characterIteratorWrapper={
    ಶೂನ್ಯಸೂಚಕ, 0, 0, 0, 0, 0,
    character_Iterator_Get_ಸೂಚ್ಯಂಕ,
    characterIteratorMove,
    character_Iterator_Has_ಮುಂದಿನ,
    character_Iterator_Has_ಹಿಂದಿನ,
    characterIteratorCurrent,
    character_Iterator_ಮುಂದಿನ,
    character_Iterator_ಹಿಂದಿನ,
    ಶೂನ್ಯಸೂಚಕ,
    characterIteratorGetState,
    character_Iterator_ಗಣ_State
};

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_ಗಣ_Character_Iterator(UCharIterator *iter, CharacterIterator *charIter) {
    ಆದರೆ (iter != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (charIter != ಶೂನ್ಯಸೂಚಕ) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } ಇಲ್ಲದಿದ್ದರೆ {
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

ಸ್ಥಿರ UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    ಆದರೆ(iter->ಸೂಚ್ಯಂಕ<iter->ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ ((Replaceable *)(iter->context))->charAt(iter->ಸೂಚ್ಯಂಕ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
replaceable_Iterator_ಮುಂದಿನ(UCharIterator *iter) {
    ಆದರೆ(iter->ಸೂಚ್ಯಂಕ<iter->ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ ((Replaceable *)(iter->context))->charAt(iter->ಸೂಚ್ಯಂಕ++);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
replaceable_Iterator_ಹಿಂದಿನ(UCharIterator *iter) {
    ಆದರೆ(iter->ಸೂಚ್ಯಂಕ>iter->ಆರಂಭ) {
        ಹಿಂತಿರುಗಿ ((Replaceable *)(iter->context))->charAt(--iter->ಸೂಚ್ಯಂಕ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UCharIterator replaceableIterator={
    ಶೂನ್ಯಸೂಚಕ, 0, 0, 0, 0, 0,
    string_Iterator_Get_ಸೂಚ್ಯಂಕ,
    stringIteratorMove,
    string_Iterator_Has_ಮುಂದಿನ,
    string_Iterator_Has_ಹಿಂದಿನ,
    replaceableIteratorCurrent,
    replaceable_Iterator_ಮುಂದಿನ,
    replaceable_Iterator_ಹಿಂದಿನ,
    ಶೂನ್ಯಸೂಚಕ,
    stringIteratorGetState,
    string_Iterator_ಗಣ_State
};

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_ಗಣ_Replaceable(UCharIterator *iter, ಸ್ಥಿರಾಂಕ Replaceable *rep) {
    ಆದರೆ (iter != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (rep != ಶೂನ್ಯಸೂಚಕ) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->ಎಲ್ಲೆ=iter->length=rep->length();
        } ಇಲ್ಲದಿದ್ದರೆ {
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

#ವ್ಯಾಖ್ಯಾನಿಸು UITER_CNV_ಸಾಮರ್ಥ್ಯ 16

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

ಸ್ಥಿರ int32_t U_CALLCONV
utf8_Iterator_Get_ಸೂಚ್ಯಂಕ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ಆಯ್ಕೆ(origin) {
    ಸ್ಥಿತಿ UITER_ZERO:
    ಸ್ಥಿತಿ UITER_ಆರಂಭ:
        ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ UITER_CURRENT:
        ಆದರೆ(iter->ಸೂಚ್ಯಂಕ<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ಸ್ಥಿರಾಂಕ uint8_t *s;
            UChar32 c;
            int32_t i, ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ;

            s=(ಸ್ಥಿರಾಂಕ uint8_t *)iter->context;
            i=ಸೂಚ್ಯಂಕ=0;
            ಎಲ್ಲೆ=iter->ಆರಂಭ; /* count up to the UTF-8 index */
            ಆಗ(i<ಎಲ್ಲೆ) {
                U8_ಮುಂದಿನ_OR_FFFD(s, i, ಎಲ್ಲೆ, c);
                ಸೂಚ್ಯಂಕ+=U16_ಉದ್ದಳತೆ(c);
            }

            iter->ಆರಂಭ=i; /* just in case setState() did not get us to a code point boundary */
            ಆದರೆ(i==iter->ಎಲ್ಲೆ) {
                iter->length=ಸೂಚ್ಯಂಕ; /* in case it was <0 or wrong */
            }
            ಆದರೆ(iter->reserved_ಕ್ಷೇತ್ರ!=0) {
                --ಸೂಚ್ಯಂಕ; /* we are in the middle of a supplementary code point */
            }
            iter->ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ;
        }
        ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ;
    ಸ್ಥಿತಿ UITER_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ UITER_ಉದ್ದಳತೆ:
        ಆದರೆ(iter->length<0) {
            ಸ್ಥಿರಾಂಕ uint8_t *s;
            UChar32 c;
            int32_t i, ಎಲ್ಲೆ, length;

            s=(ಸ್ಥಿರಾಂಕ uint8_t *)iter->context;
            ಆದರೆ(iter->ಸೂಚ್ಯಂಕ<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                ಎಲ್ಲೆ=iter->ಆರಂಭ;

                /* count from the beginning to the current index */
                ಆಗ(i<ಎಲ್ಲೆ) {
                    U8_ಮುಂದಿನ_OR_FFFD(s, i, ಎಲ್ಲೆ, c);
                    length+=U16_ಉದ್ದಳತೆ(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->ಆರಂಭ=i; /* just in case setState() did not get us to a code point boundary */
                iter->ಸೂಚ್ಯಂಕ= iter->reserved_ಕ್ಷೇತ್ರ!=0 ? length-1 : length;
            } ಇಲ್ಲದಿದ್ದರೆ {
                i=iter->ಆರಂಭ;
                length=iter->ಸೂಚ್ಯಂಕ;
                ಆದರೆ(iter->reserved_ಕ್ಷೇತ್ರ!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            ಎಲ್ಲೆ=iter->ಎಲ್ಲೆ;
            ಆಗ(i<ಎಲ್ಲೆ) {
                U8_ಮುಂದಿನ_OR_FFFD(s, i, ಎಲ್ಲೆ, c);
                length+=U16_ಉದ್ದಳತೆ(c);
            }
            iter->length=length;
        }
        ಹಿಂತಿರುಗಿ iter->length;
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* not a valid origin */
        /* Should never get here! */
        ಹಿಂತಿರುಗಿ -1;
    }
}

ಸ್ಥಿರ int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ಸ್ಥಿರಾಂಕ uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    ಆಯ್ಕೆ(origin) {
    ಸ್ಥಿತಿ UITER_ZERO:
    ಸ್ಥಿತಿ UITER_ಆರಂಭ:
        pos=delta;
        havePos=ನಿಜ;
        /* iter->index<0 (unknown) is possible */
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UITER_CURRENT:
        ಆದರೆ(iter->ಸೂಚ್ಯಂಕ>=0) {
            pos=iter->ಸೂಚ್ಯಂಕ+delta;
            havePos=ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=ಸುಳ್ಳು;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UITER_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ UITER_ಉದ್ದಳತೆ:
        ಆದರೆ(iter->length>=0) {
            pos=iter->length+delta;
            havePos=ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* pin to the end, avoid counting the length */
            iter->ಸೂಚ್ಯಂಕ=-1;
            iter->ಆರಂಭ=iter->ಎಲ್ಲೆ;
            iter->reserved_ಕ್ಷೇತ್ರ=0;
            ಆದರೆ(delta>=0) {
                ಹಿಂತಿರುಗಿ UITER_UNKNOWN_ಸೂಚ್ಯಂಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=ಸುಳ್ಳು;
            }
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ -1;  /* Error */
    }

    ಆದರೆ(havePos) {
        /* shortcuts: pinning to the edges of the string */
        ಆದರೆ(pos<=0) {
            iter->ಸೂಚ್ಯಂಕ=iter->ಆರಂಭ=iter->reserved_ಕ್ಷೇತ್ರ=0;
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->length>=0 && pos>=iter->length) {
            iter->ಸೂಚ್ಯಂಕ=iter->length;
            iter->ಆರಂಭ=iter->ಎಲ್ಲೆ;
            iter->reserved_ಕ್ಷೇತ್ರ=0;
            ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        ಆದರೆ(iter->ಸೂಚ್ಯಂಕ<0 || pos<iter->ಸೂಚ್ಯಂಕ/2) {
            /* go forward from the start instead of backward from the current index */
            iter->ಸೂಚ್ಯಂಕ=iter->ಆರಂಭ=iter->reserved_ಕ್ಷೇತ್ರ=0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->length>=0 && (iter->length-pos)<(pos-iter->ಸೂಚ್ಯಂಕ)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->ಸೂಚ್ಯಂಕ=iter->length;
            iter->ಆರಂಭ=iter->ಎಲ್ಲೆ;
            iter->reserved_ಕ್ಷೇತ್ರ=0;
        }

        delta=pos-iter->ಸೂಚ್ಯಂಕ;
        ಆದರೆ(delta==0) {
            ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ; /* nothing to do */
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* move relative to unknown UTF-16 index */
        ಆದರೆ(delta==0) {
            ಹಿಂತಿರುಗಿ UITER_UNKNOWN_ಸೂಚ್ಯಂಕ; /* nothing to do */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(-delta>=iter->ಆರಂಭ) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->ಸೂಚ್ಯಂಕ=iter->ಆರಂಭ=iter->reserved_ಕ್ಷೇತ್ರ=0;
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(delta>=(iter->ಎಲ್ಲೆ-iter->ಆರಂಭ)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->ಸೂಚ್ಯಂಕ=iter->length; /* may or may not be <0 (unknown) */
            iter->ಆರಂಭ=iter->ಎಲ್ಲೆ;
            iter->reserved_ಕ್ಷೇತ್ರ=0;
            ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ>=0 ? iter->ಸೂಚ್ಯಂಕ : (int32_t)UITER_UNKNOWN_ಸೂಚ್ಯಂಕ;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ಸ್ಥಿರಾಂಕ uint8_t *)iter->context;
    pos=iter->ಸೂಚ್ಯಂಕ; /* could be <0 (unknown) */
    i=iter->ಆರಂಭ;
    ಆದರೆ(delta>0) {
        /* go forward */
        int32_t ಎಲ್ಲೆ=iter->ಎಲ್ಲೆ;
        ಆದರೆ(iter->reserved_ಕ್ಷೇತ್ರ!=0) {
            iter->reserved_ಕ್ಷೇತ್ರ=0;
            ++pos;
            --delta;
        }
        ಆಗ(delta>0 && i<ಎಲ್ಲೆ) {
            U8_ಮುಂದಿನ_OR_FFFD(s, i, ಎಲ್ಲೆ, c);
            ಆದರೆ(c<=0xffff) {
                ++pos;
                --delta;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(delta>=2) {
                pos+=2;
                delta-=2;
            } ಇಲ್ಲದಿದ್ದರೆ /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reserved_ಕ್ಷೇತ್ರ=c;
                ++pos;
                ನಿಲ್ಲಿಸು; /* delta=0; */
            }
        }
        ಆದರೆ(i==ಎಲ್ಲೆ) {
            ಆದರೆ(iter->length<0 && iter->ಸೂಚ್ಯಂಕ>=0) {
                iter->length= iter->reserved_ಕ್ಷೇತ್ರ==0 ? pos : pos+1;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->ಸೂಚ್ಯಂಕ<0 && iter->length>=0) {
                iter->ಸೂಚ್ಯಂಕ= iter->reserved_ಕ್ಷೇತ್ರ==0 ? iter->length : iter->length-1;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* delta<0 */ {
        /* go backward */
        ಆದರೆ(iter->reserved_ಕ್ಷೇತ್ರ!=0) {
            iter->reserved_ಕ್ಷೇತ್ರ=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        ಆಗ(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            ಆದರೆ(c<=0xffff) {
                --pos;
                ++delta;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(delta<=-2) {
                pos-=2;
                delta+=2;
            } ಇಲ್ಲದಿದ್ದರೆ /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reserved_ಕ್ಷೇತ್ರ=c;
                --pos;
                ನಿಲ್ಲಿಸು; /* delta=0; */
            }
        }
    }

    iter->ಆರಂಭ=i;
    ಆದರೆ(iter->ಸೂಚ್ಯಂಕ>=0) {
        ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ=pos;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* we started with index<0 (unknown) so pos is bogus */
        ಆದರೆ(i<=1) {
            ಹಿಂತಿರುಗಿ iter->ಸೂಚ್ಯಂಕ=i; /* reached the beginning */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* we still don't know the UTF-16 index */
            ಹಿಂತಿರುಗಿ UITER_UNKNOWN_ಸೂಚ್ಯಂಕ;
        }
    }
}

ಸ್ಥಿರ UBool U_CALLCONV
utf8_Iterator_Has_ಮುಂದಿನ(UCharIterator *iter) {
    ಹಿಂತಿರುಗಿ iter->ಆರಂಭ<iter->ಎಲ್ಲೆ || iter->reserved_ಕ್ಷೇತ್ರ!=0;
}

ಸ್ಥಿರ UBool U_CALLCONV
utf8_Iterator_Has_ಹಿಂದಿನ(UCharIterator *iter) {
    ಹಿಂತಿರುಗಿ iter->ಆರಂಭ>0;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    ಆದರೆ(iter->reserved_ಕ್ಷೇತ್ರ!=0) {
        ಹಿಂತಿರುಗಿ U16_TRAIL(iter->reserved_ಕ್ಷೇತ್ರ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->ಆರಂಭ<iter->ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ uint8_t *s=(ಸ್ಥಿರಾಂಕ uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->ಆರಂಭ;

        U8_ಮುಂದಿನ_OR_FFFD(s, i, iter->ಎಲ್ಲೆ, c);
        ಆದರೆ(c<=0xffff) {
            ಹಿಂತಿರುಗಿ c;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ U16_LEAD(c);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf8_Iterator_ಮುಂದಿನ(UCharIterator *iter) {
    int32_t ಸೂಚ್ಯಂಕ;

    ಆದರೆ(iter->reserved_ಕ್ಷೇತ್ರ!=0) {
        char16_t trail=U16_TRAIL(iter->reserved_ಕ್ಷೇತ್ರ);
        iter->reserved_ಕ್ಷೇತ್ರ=0;
        ಆದರೆ((ಸೂಚ್ಯಂಕ=iter->ಸೂಚ್ಯಂಕ)>=0) {
            iter->ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ+1;
        }
        ಹಿಂತಿರುಗಿ trail;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->ಆರಂಭ<iter->ಎಲ್ಲೆ) {
        ಸ್ಥಿರಾಂಕ uint8_t *s=(ಸ್ಥಿರಾಂಕ uint8_t *)iter->context;
        UChar32 c;

        U8_ಮುಂದಿನ_OR_FFFD(s, iter->ಆರಂಭ, iter->ಎಲ್ಲೆ, c);
        ಆದರೆ((ಸೂಚ್ಯಂಕ=iter->ಸೂಚ್ಯಂಕ)>=0) {
            iter->ಸೂಚ್ಯಂಕ=++ಸೂಚ್ಯಂಕ;
            ಆದರೆ(iter->length<0 && iter->ಆರಂಭ==iter->ಎಲ್ಲೆ) {
                iter->length= c<=0xffff ? ಸೂಚ್ಯಂಕ : ಸೂಚ್ಯಂಕ+1;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->ಆರಂಭ==iter->ಎಲ್ಲೆ && iter->length>=0) {
            iter->ಸೂಚ್ಯಂಕ= c<=0xffff ? iter->length : iter->length-1;
        }
        ಆದರೆ(c<=0xffff) {
            ಹಿಂತಿರುಗಿ c;
        } ಇಲ್ಲದಿದ್ದರೆ {
            iter->reserved_ಕ್ಷೇತ್ರ=c;
            ಹಿಂತಿರುಗಿ U16_LEAD(c);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf8_Iterator_ಹಿಂದಿನ(UCharIterator *iter) {
    int32_t ಸೂಚ್ಯಂಕ;

    ಆದರೆ(iter->reserved_ಕ್ಷೇತ್ರ!=0) {
        char16_t lead=U16_LEAD(iter->reserved_ಕ್ಷೇತ್ರ);
        iter->reserved_ಕ್ಷೇತ್ರ=0;
        iter->ಆರಂಭ-=4; /* we stayed behind the supplementary code point; go before it now */
        ಆದರೆ((ಸೂಚ್ಯಂಕ=iter->ಸೂಚ್ಯಂಕ)>0) {
            iter->ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ-1;
        }
        ಹಿಂತಿರುಗಿ lead;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->ಆರಂಭ>0) {
        ಸ್ಥಿರಾಂಕ uint8_t *s=(ಸ್ಥಿರಾಂಕ uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->ಆರಂಭ, c);
        ಆದರೆ((ಸೂಚ್ಯಂಕ=iter->ಸೂಚ್ಯಂಕ)>0) {
            iter->ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ-1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->ಆರಂಭ<=1) {
            iter->ಸೂಚ್ಯಂಕ= c<=0xffff ? iter->ಆರಂಭ : iter->ಆರಂಭ+1;
        }
        ಆದರೆ(c<=0xffff) {
            ಹಿಂತಿರುಗಿ c;
        } ಇಲ್ಲದಿದ್ದರೆ {
            iter->ಆರಂಭ+=4; /* back to behind this supplementary code point for consistent state */
            iter->reserved_ಕ್ಷೇತ್ರ=c;
            ಹಿಂತಿರುಗಿ U16_TRAIL(c);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ U_SENTINEL;
    }
}

ಸ್ಥಿರ uint32_t U_CALLCONV
utf8IteratorGetState(ಸ್ಥಿರಾಂಕ UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->ಆರಂಭ<<1);
    ಆದರೆ(iter->reserved_ಕ್ಷೇತ್ರ!=0) {
        state|=1;
    }
    ಹಿಂತಿರುಗಿ state;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
utf8_Iterator_ಗಣ_State(UCharIterator *iter,
                     uint32_t state,
                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        /* do nothing */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t ಸೂಚ್ಯಂಕ=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        ಆದರೆ((state==0 ? ಸೂಚ್ಯಂಕ<0 : ಸೂಚ್ಯಂಕ<4) || iter->ಎಲ್ಲೆ<ಸೂಚ್ಯಂಕ) {
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            iter->ಆರಂಭ=ಸೂಚ್ಯಂಕ; /* restore UTF-8 byte index */
            ಆದರೆ(ಸೂಚ್ಯಂಕ<=1) {
                iter->ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                iter->ಸೂಚ್ಯಂಕ=-1; /* unknown UTF-16 index */
            }
            ಆದರೆ(state==0) {
                iter->reserved_ಕ್ಷೇತ್ರ=0;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((ಸ್ಥಿರಾಂಕ uint8_t *)iter->context, 0, ಸೂಚ್ಯಂಕ, c);
                ಆದರೆ(c<=0xffff) {
                    *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    iter->reserved_ಕ್ಷೇತ್ರ=c;
                }
            }
        }
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UCharIterator utf8Iterator={
    ಶೂನ್ಯಸೂಚಕ, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_ಸೂಚ್ಯಂಕ,
    utf8IteratorMove,
    utf8_Iterator_Has_ಮುಂದಿನ,
    utf8_Iterator_Has_ಹಿಂದಿನ,
    utf8IteratorCurrent,
    utf8_Iterator_ಮುಂದಿನ,
    utf8_Iterator_ಹಿಂದಿನ,
    ಶೂನ್ಯಸೂಚಕ,
    utf8IteratorGetState,
    utf8_Iterator_ಗಣ_State
};

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_ಗಣ_UTF8(UCharIterator *iter, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t length) {
    ಆದರೆ (iter != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (s != ಶೂನ್ಯಸೂಚಕ && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            ಆದರೆ(length>=0) {
                iter->ಎಲ್ಲೆ=length;
            } ಇಲ್ಲದಿದ್ದರೆ {
                iter->ಎಲ್ಲೆ=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->ಎಲ್ಲೆ<=1 ? iter->ಎಲ್ಲೆ : -1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    ಆದರೆ(U16_IS_SURROGATE(c)) {
        ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            ಆದರೆ(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(U16_IS_LEAD(c2=iter->ಹಿಂದಿನ(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            ಆದರೆ(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    ಹಿಂತಿರುಗಿ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    ಆದರೆ(U16_IS_LEAD(c)) {
        ಆದರೆ(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    ಹಿಂತಿರುಗಿ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->ಹಿಂದಿನ(iter);
    ಆದರೆ(U16_IS_TRAIL(c)) {
        ಆದರೆ(U16_IS_LEAD(c2=iter->ಹಿಂದಿನ(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    ಹಿಂತಿರುಗಿ c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ಸ್ಥಿರಾಂಕ UCharIterator *iter) {
    ಆದರೆ(iter==ಶೂನ್ಯಸೂಚಕ || iter->getState==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ UITER_NO_STATE;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ iter->getState(iter);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_ಗಣ_State(UCharIterator *iter, uint32_t state, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        /* do nothing */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iter->ಗಣ_State==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        iter->ಗಣ_State(iter, state, p_Error_ಸಂಕೇತ);
    }
}

U_CDECL_ಕೊನೆ
