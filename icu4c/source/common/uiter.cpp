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

#define IS_EVEN(n) (((n)&1)==0)
#define IS_POINTER_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

ಸ್ಥಿರ int32_t U_CALLCONV
noopGetIndex(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ UBool U_CALLCONV
noopHasNext(UCharIterator * /*iter*/) {
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
}

ಸ್ಥಿರ uint32_t U_CALLCONV
noopGetState(ಧ್ರುವ UCharIterator * /*iter*/) {
    ಪ್ರತ್ಯಾಗಮ UITER_NO_STATE;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
noopSetState(UCharIterator * /*iter*/, uint32_t /*state*/, UErrorCode *pErrorCode) {
    *pErrorCode=U_UNSUPPORTED_ERROR;
}

ಸ್ಥಿರ ಧ್ರುವ UCharIterator noopIterator={
    nullptr, 0, 0, 0, 0, 0,
    noopGetIndex,
    noopMove,
    noopHasNext,
    noopHasNext,
    noopCurrent,
    noopCurrent,
    noopCurrent,
    nullptr,
    noopGetState,
    noopSetState
};

/* UCharIterator implementation for simple strings -------------------------- */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * for char16_t * strings.
 *
 * The UCharIterator.context field holds a pointer to the string.
 */

ಸ್ಥಿರ int32_t U_CALLCONV
stringIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ವಿಕಲ್ಪ(origin) {
    ಪಕ್ಷ UITER_ZERO:
        ಪ್ರತ್ಯಾಗಮ 0;
    ಪಕ್ಷ UITER_START:
        ಪ್ರತ್ಯಾಗಮ iter->start;
    ಪಕ್ಷ UITER_CURRENT:
        ಪ್ರತ್ಯಾಗಮ iter->index;
    ಪಕ್ಷ UITER_LIMIT:
        ಪ್ರತ್ಯಾಗಮ iter->limit;
    ಪಕ್ಷ UITER_LENGTH:
        ಪ್ರತ್ಯಾಗಮ iter->length;
    ಸ್ವಭಾವ:
        /* not a valid origin */
        /* Should never get here! */
        ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ಸ್ಥಿರ int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    ವಿಕಲ್ಪ(origin) {
    ಪಕ್ಷ UITER_ZERO:
        pos=delta;
        ಭಙ್ಗ;
    ಪಕ್ಷ UITER_START:
        pos=iter->start+delta;
        ಭಙ್ಗ;
    ಪಕ್ಷ UITER_CURRENT:
        pos=iter->index+delta;
        ಭಙ್ಗ;
    ಪಕ್ಷ UITER_LIMIT:
        pos=iter->limit+delta;
        ಭಙ್ಗ;
    ಪಕ್ಷ UITER_LENGTH:
        pos=iter->length+delta;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ -1;  /* Error */
    }

    ಯದಿ(pos<iter->start) {
        pos=iter->start;
    } ಅನ್ಯಥಾ ಯದಿ(pos>iter->limit) {
        pos=iter->limit;
    }

    ಪ್ರತ್ಯಾಗಮ iter->index=pos;
}

ಸ್ಥಿರ UBool U_CALLCONV
stringIteratorHasNext(UCharIterator *iter) {
    ಪ್ರತ್ಯಾಗಮ iter->index<iter->limit;
}

ಸ್ಥಿರ UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    ಪ್ರತ್ಯಾಗಮ iter->index>iter->start;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    ಯದಿ(iter->index<iter->limit) {
        ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ char16_t *)(iter->context))[iter->index];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
stringIteratorNext(UCharIterator *iter) {
    ಯದಿ(iter->index<iter->limit) {
        ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ char16_t *)(iter->context))[iter->index++];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    ಯದಿ(iter->index>iter->start) {
        ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ char16_t *)(iter->context))[--iter->index];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ uint32_t U_CALLCONV
stringIteratorGetState(ಧ್ರುವ UCharIterator *iter) {
    ಪ್ರತ್ಯಾಗಮ (uint32_t)iter->index;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ಅನ್ಯಥಾ ಯದಿ(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ಅನ್ಯಥಾ ಯದಿ((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } ಅನ್ಯಥಾ {
        iter->index=(int32_t)state;
    }
}

ಸ್ಥಿರ ಧ್ರುವ UCharIterator stringIterator={
    nullptr, 0, 0, 0, 0, 0,
    stringIteratorGetIndex,
    stringIteratorMove,
    stringIteratorHasNext,
    stringIteratorHasPrevious,
    stringIteratorCurrent,
    stringIteratorNext,
    stringIteratorPrevious,
    nullptr,
    stringIteratorGetState,
    stringIteratorSetState
};

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_setString(UCharIterator *iter, ಧ್ರುವ char16_t *s, int32_t length) {
    ಯದಿ (iter != nullptr) {
        ಯದಿ (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            ಯದಿ(length>=0) {
                iter->length=length;
            } ಅನ್ಯಥಾ {
                iter->length=u_strlen(s);
            }
            iter->limit=iter->length;
        } ಅನ್ಯಥಾ {
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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t index) {
    ಧ್ರುವ uint8_t *p=(ಧ್ರುವ uint8_t *)iter->context;
    ಪ್ರತ್ಯಾಗಮ ((char16_t)p[2*index]<<8)|(char16_t)p[2*index+1];
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t index;

    ಯದಿ((index=iter->index)<iter->limit) {
        ಪ್ರತ್ಯಾಗಮ utf16BEIteratorGet(iter, index);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf16BEIteratorNext(UCharIterator *iter) {
    int32_t index;

    ಯದಿ((index=iter->index)<iter->limit) {
        iter->index=index+1;
        ಪ್ರತ್ಯಾಗಮ utf16BEIteratorGet(iter, index);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t index;

    ಯದಿ((index=iter->index)>iter->start) {
        iter->index=--index;
        ಪ್ರತ್ಯಾಗಮ utf16BEIteratorGet(iter, index);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ ಧ್ರುವ UCharIterator utf16BEIterator={
    nullptr, 0, 0, 0, 0, 0,
    stringIteratorGetIndex,
    stringIteratorMove,
    stringIteratorHasNext,
    stringIteratorHasPrevious,
    utf16BEIteratorCurrent,
    utf16BEIteratorNext,
    utf16BEIteratorPrevious,
    nullptr,
    stringIteratorGetState,
    stringIteratorSetState
};

/*
 * Count the number of UChars in a UTF-16BE string before a terminating char16_t NUL,
 * i.e., before a pair of 0 bytes where the first 0 byte is at an even
 * offset from s.
 */
ಸ್ಥಿರ int32_t
utf16BE_strlen(ಧ್ರುವ ವರ್ಣ *s) {
    ಯದಿ(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        ಪ್ರತ್ಯಾಗಮ u_strlen((ಧ್ರುವ char16_t *)s);
    } ಅನ್ಯಥಾ {
        /* odd-aligned, search for pair of 0 bytes */
        ಧ್ರುವ ವರ್ಣ *p=s;

        ಯಾವತ್(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        ಪ್ರತ್ಯಾಗಮ (int32_t)((p-s)/2);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, ಧ್ರುವ ವರ್ಣ *s, int32_t length) {
    ಯದಿ(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        ಯದಿ(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            ಯದಿ(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (ಧ್ರುವ char16_t *)s, length);
                ಪ್ರತ್ಯಾಗಮ;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            ಯದಿ(length>=0) {
                iter->length=length;
            } ಅನ್ಯಥಾ {
                iter->length=utf16BE_strlen(s);
            }
            iter->limit=iter->length;
        } ಅನ್ಯಥಾ {
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
characterIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ವಿಕಲ್ಪ(origin) {
    ಪಕ್ಷ UITER_ZERO:
        ಪ್ರತ್ಯಾಗಮ 0;
    ಪಕ್ಷ UITER_START:
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->startIndex();
    ಪಕ್ಷ UITER_CURRENT:
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->getIndex();
    ಪಕ್ಷ UITER_LIMIT:
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->endIndex();
    ಪಕ್ಷ UITER_LENGTH:
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->getLength();
    ಸ್ವಭಾವ:
        /* not a valid origin */
        /* Should never get here! */
        ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ಸ್ಥಿರ int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ವಿಕಲ್ಪ(origin) {
    ಪಕ್ಷ UITER_ZERO:
        ((CharacterIterator *)(iter->context))->setIndex(delta);
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->getIndex();
    ಪಕ್ಷ UITER_START:
    ಪಕ್ಷ UITER_CURRENT:
    ಪಕ್ಷ UITER_LIMIT:
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    ಪಕ್ಷ UITER_LENGTH:
        ((CharacterIterator *)(iter->context))->setIndex(((CharacterIterator *)(iter->context))->getLength()+delta);
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->getIndex();
    ಸ್ವಭಾವ:
        /* not a valid origin */
        /* Should never get here! */
        ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ಸ್ಥಿರ UBool U_CALLCONV
characterIteratorHasNext(UCharIterator *iter) {
    ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->hasNext();
}

ಸ್ಥಿರ UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->hasPrevious();
}

ಸ್ಥಿರ UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    ಯದಿ(c!=0xffff || ((CharacterIterator *)(iter->context))->hasNext()) {
        ಪ್ರತ್ಯಾಗಮ c;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
characterIteratorNext(UCharIterator *iter) {
    ಯದಿ(((CharacterIterator *)(iter->context))->hasNext()) {
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->nextPostInc();
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    ಯದಿ(((CharacterIterator *)(iter->context))->hasPrevious()) {
        ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->previous();
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ uint32_t U_CALLCONV
characterIteratorGetState(ಧ್ರುವ UCharIterator *iter) {
    ಪ್ರತ್ಯಾಗಮ ((CharacterIterator *)(iter->context))->getIndex();
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ಅನ್ಯಥಾ ಯದಿ(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ಅನ್ಯಥಾ ಯದಿ((int32_t)state<((CharacterIterator *)(iter->context))->startIndex() || ((CharacterIterator *)(iter->context))->endIndex()<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } ಅನ್ಯಥಾ {
        ((CharacterIterator *)(iter->context))->setIndex((int32_t)state);
    }
}

ಸ್ಥಿರ ಧ್ರುವ UCharIterator characterIteratorWrapper={
    nullptr, 0, 0, 0, 0, 0,
    characterIteratorGetIndex,
    characterIteratorMove,
    characterIteratorHasNext,
    characterIteratorHasPrevious,
    characterIteratorCurrent,
    characterIteratorNext,
    characterIteratorPrevious,
    nullptr,
    characterIteratorGetState,
    characterIteratorSetState
};

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_setCharacterIterator(UCharIterator *iter, CharacterIterator *charIter) {
    ಯದಿ (iter != nullptr) {
        ಯದಿ (charIter != nullptr) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } ಅನ್ಯಥಾ {
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
    ಯದಿ(iter->index<iter->limit) {
        ಪ್ರತ್ಯಾಗಮ ((Replaceable *)(iter->context))->charAt(iter->index);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
replaceableIteratorNext(UCharIterator *iter) {
    ಯದಿ(iter->index<iter->limit) {
        ಪ್ರತ್ಯಾಗಮ ((Replaceable *)(iter->context))->charAt(iter->index++);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    ಯದಿ(iter->index>iter->start) {
        ಪ್ರತ್ಯಾಗಮ ((Replaceable *)(iter->context))->charAt(--iter->index);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ ಧ್ರುವ UCharIterator replaceableIterator={
    nullptr, 0, 0, 0, 0, 0,
    stringIteratorGetIndex,
    stringIteratorMove,
    stringIteratorHasNext,
    stringIteratorHasPrevious,
    replaceableIteratorCurrent,
    replaceableIteratorNext,
    replaceableIteratorPrevious,
    nullptr,
    stringIteratorGetState,
    stringIteratorSetState
};

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_setReplaceable(UCharIterator *iter, ಧ್ರುವ Replaceable *rep) {
    ಯದಿ (iter != nullptr) {
        ಯದಿ (rep != nullptr) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->limit=iter->length=rep->length();
        } ಅನ್ಯಥಾ {
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

#define UITER_CNV_CAPACITY 16

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
utf8IteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ವಿಕಲ್ಪ(origin) {
    ಪಕ್ಷ UITER_ZERO:
    ಪಕ್ಷ UITER_START:
        ಪ್ರತ್ಯಾಗಮ 0;
    ಪಕ್ಷ UITER_CURRENT:
        ಯದಿ(iter->index<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ಧ್ರುವ uint8_t *s;
            UChar32 c;
            int32_t i, limit, index;

            s=(ಧ್ರುವ uint8_t *)iter->context;
            i=index=0;
            limit=iter->start; /* count up to the UTF-8 index */
            ಯಾವತ್(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                index+=U16_LENGTH(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            ಯದಿ(i==iter->limit) {
                iter->length=index; /* in case it was <0 or wrong */
            }
            ಯದಿ(iter->reservedField!=0) {
                --index; /* we are in the middle of a supplementary code point */
            }
            iter->index=index;
        }
        ಪ್ರತ್ಯಾಗಮ iter->index;
    ಪಕ್ಷ UITER_LIMIT:
    ಪಕ್ಷ UITER_LENGTH:
        ಯದಿ(iter->length<0) {
            ಧ್ರುವ uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(ಧ್ರುವ uint8_t *)iter->context;
            ಯದಿ(iter->index<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                ಯಾವತ್(i<limit) {
                    U8_NEXT_OR_FFFD(s, i, limit, c);
                    length+=U16_LENGTH(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->index= iter->reservedField!=0 ? length-1 : length;
            } ಅನ್ಯಥಾ {
                i=iter->start;
                length=iter->index;
                ಯದಿ(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            ಯಾವತ್(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                length+=U16_LENGTH(c);
            }
            iter->length=length;
        }
        ಪ್ರತ್ಯಾಗಮ iter->length;
    ಸ್ವಭಾವ:
        /* not a valid origin */
        /* Should never get here! */
        ಪ್ರತ್ಯಾಗಮ -1;
    }
}

ಸ್ಥಿರ int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ಧ್ರುವ uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    ವಿಕಲ್ಪ(origin) {
    ಪಕ್ಷ UITER_ZERO:
    ಪಕ್ಷ UITER_START:
        pos=delta;
        havePos=ಸತ್ಯ;
        /* iter->index<0 (unknown) is possible */
        ಭಙ್ಗ;
    ಪಕ್ಷ UITER_CURRENT:
        ಯದಿ(iter->index>=0) {
            pos=iter->index+delta;
            havePos=ಸತ್ಯ;
        } ಅನ್ಯಥಾ {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=ಅಸತ್ಯ;
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UITER_LIMIT:
    ಪಕ್ಷ UITER_LENGTH:
        ಯದಿ(iter->length>=0) {
            pos=iter->length+delta;
            havePos=ಸತ್ಯ;
        } ಅನ್ಯಥಾ {
            /* pin to the end, avoid counting the length */
            iter->index=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            ಯದಿ(delta>=0) {
                ಪ್ರತ್ಯಾಗಮ UITER_UNKNOWN_INDEX;
            } ಅನ್ಯಥಾ {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=ಅಸತ್ಯ;
            }
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ -1;  /* Error */
    }

    ಯದಿ(havePos) {
        /* shortcuts: pinning to the edges of the string */
        ಯದಿ(pos<=0) {
            iter->index=iter->start=iter->reservedField=0;
            ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ ಯದಿ(iter->length>=0 && pos>=iter->length) {
            iter->index=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            ಪ್ರತ್ಯಾಗಮ iter->index;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        ಯದಿ(iter->index<0 || pos<iter->index/2) {
            /* go forward from the start instead of backward from the current index */
            iter->index=iter->start=iter->reservedField=0;
        } ಅನ್ಯಥಾ ಯದಿ(iter->length>=0 && (iter->length-pos)<(pos-iter->index)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->index=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
        }

        delta=pos-iter->index;
        ಯದಿ(delta==0) {
            ಪ್ರತ್ಯಾಗಮ iter->index; /* nothing to do */
        }
    } ಅನ್ಯಥಾ {
        /* move relative to unknown UTF-16 index */
        ಯದಿ(delta==0) {
            ಪ್ರತ್ಯಾಗಮ UITER_UNKNOWN_INDEX; /* nothing to do */
        } ಅನ್ಯಥಾ ಯದಿ(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->index=iter->start=iter->reservedField=0;
            ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ ಯದಿ(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->index=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            ಪ್ರತ್ಯಾಗಮ iter->index>=0 ? iter->index : (int32_t)UITER_UNKNOWN_INDEX;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ಧ್ರುವ uint8_t *)iter->context;
    pos=iter->index; /* could be <0 (unknown) */
    i=iter->start;
    ಯದಿ(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        ಯದಿ(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        ಯಾವತ್(delta>0 && i<limit) {
            U8_NEXT_OR_FFFD(s, i, limit, c);
            ಯದಿ(c<=0xffff) {
                ++pos;
                --delta;
            } ಅನ್ಯಥಾ ಯದಿ(delta>=2) {
                pos+=2;
                delta-=2;
            } ಅನ್ಯಥಾ /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                ಭಙ್ಗ; /* delta=0; */
            }
        }
        ಯದಿ(i==limit) {
            ಯದಿ(iter->length<0 && iter->index>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } ಅನ್ಯಥಾ ಯದಿ(iter->index<0 && iter->length>=0) {
                iter->index= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } ಅನ್ಯಥಾ /* delta<0 */ {
        /* go backward */
        ಯದಿ(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        ಯಾವತ್(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            ಯದಿ(c<=0xffff) {
                --pos;
                ++delta;
            } ಅನ್ಯಥಾ ಯದಿ(delta<=-2) {
                pos-=2;
                delta+=2;
            } ಅನ್ಯಥಾ /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                ಭಙ್ಗ; /* delta=0; */
            }
        }
    }

    iter->start=i;
    ಯದಿ(iter->index>=0) {
        ಪ್ರತ್ಯಾಗಮ iter->index=pos;
    } ಅನ್ಯಥಾ {
        /* we started with index<0 (unknown) so pos is bogus */
        ಯದಿ(i<=1) {
            ಪ್ರತ್ಯಾಗಮ iter->index=i; /* reached the beginning */
        } ಅನ್ಯಥಾ {
            /* we still don't know the UTF-16 index */
            ಪ್ರತ್ಯಾಗಮ UITER_UNKNOWN_INDEX;
        }
    }
}

ಸ್ಥಿರ UBool U_CALLCONV
utf8IteratorHasNext(UCharIterator *iter) {
    ಪ್ರತ್ಯಾಗಮ iter->start<iter->limit || iter->reservedField!=0;
}

ಸ್ಥಿರ UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    ಪ್ರತ್ಯಾಗಮ iter->start>0;
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    ಯದಿ(iter->reservedField!=0) {
        ಪ್ರತ್ಯಾಗಮ U16_TRAIL(iter->reservedField);
    } ಅನ್ಯಥಾ ಯದಿ(iter->start<iter->limit) {
        ಧ್ರುವ uint8_t *s=(ಧ್ರುವ uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_NEXT_OR_FFFD(s, i, iter->limit, c);
        ಯದಿ(c<=0xffff) {
            ಪ್ರತ್ಯಾಗಮ c;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ U16_LEAD(c);
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf8IteratorNext(UCharIterator *iter) {
    int32_t index;

    ಯದಿ(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        ಯದಿ((index=iter->index)>=0) {
            iter->index=index+1;
        }
        ಪ್ರತ್ಯಾಗಮ trail;
    } ಅನ್ಯಥಾ ಯದಿ(iter->start<iter->limit) {
        ಧ್ರುವ uint8_t *s=(ಧ್ರುವ uint8_t *)iter->context;
        UChar32 c;

        U8_NEXT_OR_FFFD(s, iter->start, iter->limit, c);
        ಯದಿ((index=iter->index)>=0) {
            iter->index=++index;
            ಯದಿ(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? index : index+1;
            }
        } ಅನ್ಯಥಾ ಯದಿ(iter->start==iter->limit && iter->length>=0) {
            iter->index= c<=0xffff ? iter->length : iter->length-1;
        }
        ಯದಿ(c<=0xffff) {
            ಪ್ರತ್ಯಾಗಮ c;
        } ಅನ್ಯಥಾ {
            iter->reservedField=c;
            ಪ್ರತ್ಯಾಗಮ U16_LEAD(c);
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t index;

    ಯದಿ(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        ಯದಿ((index=iter->index)>0) {
            iter->index=index-1;
        }
        ಪ್ರತ್ಯಾಗಮ lead;
    } ಅನ್ಯಥಾ ಯದಿ(iter->start>0) {
        ಧ್ರುವ uint8_t *s=(ಧ್ರುವ uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->start, c);
        ಯದಿ((index=iter->index)>0) {
            iter->index=index-1;
        } ಅನ್ಯಥಾ ಯದಿ(iter->start<=1) {
            iter->index= c<=0xffff ? iter->start : iter->start+1;
        }
        ಯದಿ(c<=0xffff) {
            ಪ್ರತ್ಯಾಗಮ c;
        } ಅನ್ಯಥಾ {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            ಪ್ರತ್ಯಾಗಮ U16_TRAIL(c);
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ U_SENTINEL;
    }
}

ಸ್ಥಿರ uint32_t U_CALLCONV
utf8IteratorGetState(ಧ್ರುವ UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    ಯದಿ(iter->reservedField!=0) {
        state|=1;
    }
    ಪ್ರತ್ಯಾಗಮ state;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ಅನ್ಯಥಾ ಯದಿ(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ಅನ್ಯಥಾ ಯದಿ(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } ಅನ್ಯಥಾ {
        int32_t index=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        ಯದಿ((state==0 ? index<0 : index<4) || iter->limit<index) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        } ಅನ್ಯಥಾ {
            iter->start=index; /* restore UTF-8 byte index */
            ಯದಿ(index<=1) {
                iter->index=index;
            } ಅನ್ಯಥಾ {
                iter->index=-1; /* unknown UTF-16 index */
            }
            ಯದಿ(state==0) {
                iter->reservedField=0;
            } ಅನ್ಯಥಾ {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((ಧ್ರುವ uint8_t *)iter->context, 0, index, c);
                ಯದಿ(c<=0xffff) {
                    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                } ಅನ್ಯಥಾ {
                    iter->reservedField=c;
                }
            }
        }
    }
}

ಸ್ಥಿರ ಧ್ರುವ UCharIterator utf8Iterator={
    nullptr, 0, 0, 0, 0, 0,
    utf8IteratorGetIndex,
    utf8IteratorMove,
    utf8IteratorHasNext,
    utf8IteratorHasPrevious,
    utf8IteratorCurrent,
    utf8IteratorNext,
    utf8IteratorPrevious,
    nullptr,
    utf8IteratorGetState,
    utf8IteratorSetState
};

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_setUTF8(UCharIterator *iter, ಧ್ರುವ ವರ್ಣ *s, int32_t length) {
    ಯದಿ (iter != nullptr) {
        ಯದಿ (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            ಯದಿ(length>=0) {
                iter->limit=length;
            } ಅನ್ಯಥಾ {
                iter->limit=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->limit<=1 ? iter->limit : -1;
        } ಅನ್ಯಥಾ {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    ಯದಿ(U16_IS_SURROGATE(c)) {
        ಯದಿ(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            ಯದಿ(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } ಅನ್ಯಥಾ {
            ಯದಿ(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            ಯದಿ(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    ಯದಿ(U16_IS_LEAD(c)) {
        ಯದಿ(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } ಅನ್ಯಥಾ ಯದಿ(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    ಪ್ರತ್ಯಾಗಮ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    ಯದಿ(U16_IS_TRAIL(c)) {
        ಯದಿ(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } ಅನ್ಯಥಾ ಯದಿ(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    ಪ್ರತ್ಯಾಗಮ c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ಧ್ರುವ UCharIterator *iter) {
    ಯದಿ(iter==nullptr || iter->getState==nullptr) {
        ಪ್ರತ್ಯಾಗಮ UITER_NO_STATE;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ iter->getState(iter);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ಅನ್ಯಥಾ ಯದಿ(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ಅನ್ಯಥಾ ಯದಿ(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } ಅನ್ಯಥಾ {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
