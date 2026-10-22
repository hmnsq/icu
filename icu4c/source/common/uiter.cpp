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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/chariter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/rep.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"

U_NAMESPACE_USE

#define IS_EVEN(n) (((n)&1)==0)
#define IS_POINTER_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

ସ୍ଥିର int32_t U_CALLCONV
noopGetIndex(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର UBool U_CALLCONV
noopHasNext(UCharIterator * /*iter*/) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ସ୍ଥିର UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    ଫେରନ୍ତୁ U_SENTINEL;
}

ସ୍ଥିର uint32_t U_CALLCONV
noopGetState(ଅଚଳ UCharIterator * /*iter*/) {
    ଫେରନ୍ତୁ UITER_NO_STATE;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
noopSetState(UCharIterator * /*iter*/, uint32_t /*state*/, UErrorCode *pErrorCode) {
    *pErrorCode=U_UNSUPPORTED_ERROR;
}

ସ୍ଥିର ଅଚଳ UCharIterator noopIterator={
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

ସ୍ଥିର int32_t U_CALLCONV
stringIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ବାଛ(origin) {
    ସ୍ଥିତି UITER_ZERO:
        ଫେରନ୍ତୁ 0;
    ସ୍ଥିତି UITER_START:
        ଫେରନ୍ତୁ iter->start;
    ସ୍ଥିତି UITER_CURRENT:
        ଫେରନ୍ତୁ iter->index;
    ସ୍ଥିତି UITER_LIMIT:
        ଫେରନ୍ତୁ iter->limit;
    ସ୍ଥିତି UITER_LENGTH:
        ଫେରନ୍ତୁ iter->length;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* not a valid origin */
        /* Should never get here! */
        ଫେରନ୍ତୁ -1;
    }
}

ସ୍ଥିର int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    ବାଛ(origin) {
    ସ୍ଥିତି UITER_ZERO:
        pos=delta;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UITER_START:
        pos=iter->start+delta;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UITER_CURRENT:
        pos=iter->index+delta;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UITER_LIMIT:
        pos=iter->limit+delta;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UITER_LENGTH:
        pos=iter->length+delta;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ -1;  /* Error */
    }

    ଯଦି(pos<iter->start) {
        pos=iter->start;
    } ଅନ୍ୟଥା ଯଦି(pos>iter->limit) {
        pos=iter->limit;
    }

    ଫେରନ୍ତୁ iter->index=pos;
}

ସ୍ଥିର UBool U_CALLCONV
stringIteratorHasNext(UCharIterator *iter) {
    ଫେରନ୍ତୁ iter->index<iter->limit;
}

ସ୍ଥିର UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    ଫେରନ୍ତୁ iter->index>iter->start;
}

ସ୍ଥିର UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    ଯଦି(iter->index<iter->limit) {
        ଫେରନ୍ତୁ ((ଅଚଳ char16_t *)(iter->context))[iter->index];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
stringIteratorNext(UCharIterator *iter) {
    ଯଦି(iter->index<iter->limit) {
        ଫେରନ୍ତୁ ((ଅଚଳ char16_t *)(iter->context))[iter->index++];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    ଯଦି(iter->index>iter->start) {
        ଫେରନ୍ତୁ ((ଅଚଳ char16_t *)(iter->context))[--iter->index];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର uint32_t U_CALLCONV
stringIteratorGetState(ଅଚଳ UCharIterator *iter) {
    ଫେରନ୍ତୁ (uint32_t)iter->index;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ଅନ୍ୟଥା ଯଦି(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ଅନ୍ୟଥା ଯଦି((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } ଅନ୍ୟଥା {
        iter->index=(int32_t)state;
    }
}

ସ୍ଥିର ଅଚଳ UCharIterator stringIterator={
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

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uiter_setString(UCharIterator *iter, ଅଚଳ char16_t *s, int32_t length) {
    ଯଦି (iter != nullptr) {
        ଯଦି (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            ଯଦି(length>=0) {
                iter->length=length;
            } ଅନ୍ୟଥା {
                iter->length=u_strlen(s);
            }
            iter->limit=iter->length;
        } ଅନ୍ୟଥା {
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
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t index) {
    ଅଚଳ uint8_t *p=(ଅଚଳ uint8_t *)iter->context;
    ଫେରନ୍ତୁ ((char16_t)p[2*index]<<8)|(char16_t)p[2*index+1];
}

ସ୍ଥିର UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t index;

    ଯଦି((index=iter->index)<iter->limit) {
        ଫେରନ୍ତୁ utf16BEIteratorGet(iter, index);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
utf16BEIteratorNext(UCharIterator *iter) {
    int32_t index;

    ଯଦି((index=iter->index)<iter->limit) {
        iter->index=index+1;
        ଫେରନ୍ତୁ utf16BEIteratorGet(iter, index);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t index;

    ଯଦି((index=iter->index)>iter->start) {
        iter->index=--index;
        ଫେରନ୍ତୁ utf16BEIteratorGet(iter, index);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର ଅଚଳ UCharIterator utf16BEIterator={
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
ସ୍ଥିର int32_t
utf16BE_strlen(ଅଚଳ ଅକ୍ଷର *s) {
    ଯଦି(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        ଫେରନ୍ତୁ u_strlen((ଅଚଳ char16_t *)s);
    } ଅନ୍ୟଥା {
        /* odd-aligned, search for pair of 0 bytes */
        ଅଚଳ ଅକ୍ଷର *p=s;

        ଯେତେବେଳେ(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        ଫେରନ୍ତୁ (int32_t)((p-s)/2);
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, ଅଚଳ ଅକ୍ଷର *s, int32_t length) {
    ଯଦି(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        ଯଦି(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            ଯଦି(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (ଅଚଳ char16_t *)s, length);
                ଫେରନ୍ତୁ;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            ଯଦି(length>=0) {
                iter->length=length;
            } ଅନ୍ୟଥା {
                iter->length=utf16BE_strlen(s);
            }
            iter->limit=iter->length;
        } ଅନ୍ୟଥା {
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

ସ୍ଥିର int32_t U_CALLCONV
characterIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ବାଛ(origin) {
    ସ୍ଥିତି UITER_ZERO:
        ଫେରନ୍ତୁ 0;
    ସ୍ଥିତି UITER_START:
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->startIndex();
    ସ୍ଥିତି UITER_CURRENT:
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->getIndex();
    ସ୍ଥିତି UITER_LIMIT:
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->endIndex();
    ସ୍ଥିତି UITER_LENGTH:
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->getLength();
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* not a valid origin */
        /* Should never get here! */
        ଫେରନ୍ତୁ -1;
    }
}

ସ୍ଥିର int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ବାଛ(origin) {
    ସ୍ଥିତି UITER_ZERO:
        ((CharacterIterator *)(iter->context))->setIndex(delta);
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->getIndex();
    ସ୍ଥିତି UITER_START:
    ସ୍ଥିତି UITER_CURRENT:
    ସ୍ଥିତି UITER_LIMIT:
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    ସ୍ଥିତି UITER_LENGTH:
        ((CharacterIterator *)(iter->context))->setIndex(((CharacterIterator *)(iter->context))->getLength()+delta);
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->getIndex();
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* not a valid origin */
        /* Should never get here! */
        ଫେରନ୍ତୁ -1;
    }
}

ସ୍ଥିର UBool U_CALLCONV
characterIteratorHasNext(UCharIterator *iter) {
    ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->hasNext();
}

ସ୍ଥିର UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->hasPrevious();
}

ସ୍ଥିର UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    ଯଦି(c!=0xffff || ((CharacterIterator *)(iter->context))->hasNext()) {
        ଫେରନ୍ତୁ c;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
characterIteratorNext(UCharIterator *iter) {
    ଯଦି(((CharacterIterator *)(iter->context))->hasNext()) {
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->nextPostInc();
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    ଯଦି(((CharacterIterator *)(iter->context))->hasPrevious()) {
        ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->previous();
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର uint32_t U_CALLCONV
characterIteratorGetState(ଅଚଳ UCharIterator *iter) {
    ଫେରନ୍ତୁ ((CharacterIterator *)(iter->context))->getIndex();
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ଅନ୍ୟଥା ଯଦି(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ଅନ୍ୟଥା ଯଦି((int32_t)state<((CharacterIterator *)(iter->context))->startIndex() || ((CharacterIterator *)(iter->context))->endIndex()<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } ଅନ୍ୟଥା {
        ((CharacterIterator *)(iter->context))->setIndex((int32_t)state);
    }
}

ସ୍ଥିର ଅଚଳ UCharIterator characterIteratorWrapper={
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

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uiter_setCharacterIterator(UCharIterator *iter, CharacterIterator *charIter) {
    ଯଦି (iter != nullptr) {
        ଯଦି (charIter != nullptr) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } ଅନ୍ୟଥା {
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

ସ୍ଥିର UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    ଯଦି(iter->index<iter->limit) {
        ଫେରନ୍ତୁ ((Replaceable *)(iter->context))->charAt(iter->index);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
replaceableIteratorNext(UCharIterator *iter) {
    ଯଦି(iter->index<iter->limit) {
        ଫେରନ୍ତୁ ((Replaceable *)(iter->context))->charAt(iter->index++);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    ଯଦି(iter->index>iter->start) {
        ଫେରନ୍ତୁ ((Replaceable *)(iter->context))->charAt(--iter->index);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର ଅଚଳ UCharIterator replaceableIterator={
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

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uiter_setReplaceable(UCharIterator *iter, ଅଚଳ Replaceable *rep) {
    ଯଦି (iter != nullptr) {
        ଯଦି (rep != nullptr) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->limit=iter->length=rep->length();
        } ଅନ୍ୟଥା {
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

ସ୍ଥିର int32_t U_CALLCONV
utf8IteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ବାଛ(origin) {
    ସ୍ଥିତି UITER_ZERO:
    ସ୍ଥିତି UITER_START:
        ଫେରନ୍ତୁ 0;
    ସ୍ଥିତି UITER_CURRENT:
        ଯଦି(iter->index<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ଅଚଳ uint8_t *s;
            UChar32 c;
            int32_t i, limit, index;

            s=(ଅଚଳ uint8_t *)iter->context;
            i=index=0;
            limit=iter->start; /* count up to the UTF-8 index */
            ଯେତେବେଳେ(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                index+=U16_LENGTH(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            ଯଦି(i==iter->limit) {
                iter->length=index; /* in case it was <0 or wrong */
            }
            ଯଦି(iter->reservedField!=0) {
                --index; /* we are in the middle of a supplementary code point */
            }
            iter->index=index;
        }
        ଫେରନ୍ତୁ iter->index;
    ସ୍ଥିତି UITER_LIMIT:
    ସ୍ଥିତି UITER_LENGTH:
        ଯଦି(iter->length<0) {
            ଅଚଳ uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(ଅଚଳ uint8_t *)iter->context;
            ଯଦି(iter->index<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                ଯେତେବେଳେ(i<limit) {
                    U8_NEXT_OR_FFFD(s, i, limit, c);
                    length+=U16_LENGTH(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->index= iter->reservedField!=0 ? length-1 : length;
            } ଅନ୍ୟଥା {
                i=iter->start;
                length=iter->index;
                ଯଦି(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            ଯେତେବେଳେ(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                length+=U16_LENGTH(c);
            }
            iter->length=length;
        }
        ଫେରନ୍ତୁ iter->length;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* not a valid origin */
        /* Should never get here! */
        ଫେରନ୍ତୁ -1;
    }
}

ସ୍ଥିର int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ଅଚଳ uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    ବାଛ(origin) {
    ସ୍ଥିତି UITER_ZERO:
    ସ୍ଥିତି UITER_START:
        pos=delta;
        havePos=ସତ୍ୟ;
        /* iter->index<0 (unknown) is possible */
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UITER_CURRENT:
        ଯଦି(iter->index>=0) {
            pos=iter->index+delta;
            havePos=ସତ୍ୟ;
        } ଅନ୍ୟଥା {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=ମିଥ୍ୟା;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UITER_LIMIT:
    ସ୍ଥିତି UITER_LENGTH:
        ଯଦି(iter->length>=0) {
            pos=iter->length+delta;
            havePos=ସତ୍ୟ;
        } ଅନ୍ୟଥା {
            /* pin to the end, avoid counting the length */
            iter->index=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            ଯଦି(delta>=0) {
                ଫେରନ୍ତୁ UITER_UNKNOWN_INDEX;
            } ଅନ୍ୟଥା {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=ମିଥ୍ୟା;
            }
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ -1;  /* Error */
    }

    ଯଦି(havePos) {
        /* shortcuts: pinning to the edges of the string */
        ଯଦି(pos<=0) {
            iter->index=iter->start=iter->reservedField=0;
            ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା ଯଦି(iter->length>=0 && pos>=iter->length) {
            iter->index=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            ଫେରନ୍ତୁ iter->index;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        ଯଦି(iter->index<0 || pos<iter->index/2) {
            /* go forward from the start instead of backward from the current index */
            iter->index=iter->start=iter->reservedField=0;
        } ଅନ୍ୟଥା ଯଦି(iter->length>=0 && (iter->length-pos)<(pos-iter->index)) {
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
        ଯଦି(delta==0) {
            ଫେରନ୍ତୁ iter->index; /* nothing to do */
        }
    } ଅନ୍ୟଥା {
        /* move relative to unknown UTF-16 index */
        ଯଦି(delta==0) {
            ଫେରନ୍ତୁ UITER_UNKNOWN_INDEX; /* nothing to do */
        } ଅନ୍ୟଥା ଯଦି(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->index=iter->start=iter->reservedField=0;
            ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା ଯଦି(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->index=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            ଫେରନ୍ତୁ iter->index>=0 ? iter->index : (int32_t)UITER_UNKNOWN_INDEX;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ଅଚଳ uint8_t *)iter->context;
    pos=iter->index; /* could be <0 (unknown) */
    i=iter->start;
    ଯଦି(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        ଯଦି(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        ଯେତେବେଳେ(delta>0 && i<limit) {
            U8_NEXT_OR_FFFD(s, i, limit, c);
            ଯଦି(c<=0xffff) {
                ++pos;
                --delta;
            } ଅନ୍ୟଥା ଯଦି(delta>=2) {
                pos+=2;
                delta-=2;
            } ଅନ୍ୟଥା /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                ଭାଙ୍ଗ; /* delta=0; */
            }
        }
        ଯଦି(i==limit) {
            ଯଦି(iter->length<0 && iter->index>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } ଅନ୍ୟଥା ଯଦି(iter->index<0 && iter->length>=0) {
                iter->index= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } ଅନ୍ୟଥା /* delta<0 */ {
        /* go backward */
        ଯଦି(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        ଯେତେବେଳେ(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            ଯଦି(c<=0xffff) {
                --pos;
                ++delta;
            } ଅନ୍ୟଥା ଯଦି(delta<=-2) {
                pos-=2;
                delta+=2;
            } ଅନ୍ୟଥା /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                ଭାଙ୍ଗ; /* delta=0; */
            }
        }
    }

    iter->start=i;
    ଯଦି(iter->index>=0) {
        ଫେରନ୍ତୁ iter->index=pos;
    } ଅନ୍ୟଥା {
        /* we started with index<0 (unknown) so pos is bogus */
        ଯଦି(i<=1) {
            ଫେରନ୍ତୁ iter->index=i; /* reached the beginning */
        } ଅନ୍ୟଥା {
            /* we still don't know the UTF-16 index */
            ଫେରନ୍ତୁ UITER_UNKNOWN_INDEX;
        }
    }
}

ସ୍ଥିର UBool U_CALLCONV
utf8IteratorHasNext(UCharIterator *iter) {
    ଫେରନ୍ତୁ iter->start<iter->limit || iter->reservedField!=0;
}

ସ୍ଥିର UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    ଫେରନ୍ତୁ iter->start>0;
}

ସ୍ଥିର UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    ଯଦି(iter->reservedField!=0) {
        ଫେରନ୍ତୁ U16_TRAIL(iter->reservedField);
    } ଅନ୍ୟଥା ଯଦି(iter->start<iter->limit) {
        ଅଚଳ uint8_t *s=(ଅଚଳ uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_NEXT_OR_FFFD(s, i, iter->limit, c);
        ଯଦି(c<=0xffff) {
            ଫେରନ୍ତୁ c;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ U16_LEAD(c);
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
utf8IteratorNext(UCharIterator *iter) {
    int32_t index;

    ଯଦି(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        ଯଦି((index=iter->index)>=0) {
            iter->index=index+1;
        }
        ଫେରନ୍ତୁ trail;
    } ଅନ୍ୟଥା ଯଦି(iter->start<iter->limit) {
        ଅଚଳ uint8_t *s=(ଅଚଳ uint8_t *)iter->context;
        UChar32 c;

        U8_NEXT_OR_FFFD(s, iter->start, iter->limit, c);
        ଯଦି((index=iter->index)>=0) {
            iter->index=++index;
            ଯଦି(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? index : index+1;
            }
        } ଅନ୍ୟଥା ଯଦି(iter->start==iter->limit && iter->length>=0) {
            iter->index= c<=0xffff ? iter->length : iter->length-1;
        }
        ଯଦି(c<=0xffff) {
            ଫେରନ୍ତୁ c;
        } ଅନ୍ୟଥା {
            iter->reservedField=c;
            ଫେରନ୍ତୁ U16_LEAD(c);
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t index;

    ଯଦି(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        ଯଦି((index=iter->index)>0) {
            iter->index=index-1;
        }
        ଫେରନ୍ତୁ lead;
    } ଅନ୍ୟଥା ଯଦି(iter->start>0) {
        ଅଚଳ uint8_t *s=(ଅଚଳ uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->start, c);
        ଯଦି((index=iter->index)>0) {
            iter->index=index-1;
        } ଅନ୍ୟଥା ଯଦି(iter->start<=1) {
            iter->index= c<=0xffff ? iter->start : iter->start+1;
        }
        ଯଦି(c<=0xffff) {
            ଫେରନ୍ତୁ c;
        } ଅନ୍ୟଥା {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            ଫେରନ୍ତୁ U16_TRAIL(c);
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ U_SENTINEL;
    }
}

ସ୍ଥିର uint32_t U_CALLCONV
utf8IteratorGetState(ଅଚଳ UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    ଯଦି(iter->reservedField!=0) {
        state|=1;
    }
    ଫେରନ୍ତୁ state;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ଅନ୍ୟଥା ଯଦି(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ଅନ୍ୟଥା ଯଦି(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } ଅନ୍ୟଥା {
        int32_t index=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        ଯଦି((state==0 ? index<0 : index<4) || iter->limit<index) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        } ଅନ୍ୟଥା {
            iter->start=index; /* restore UTF-8 byte index */
            ଯଦି(index<=1) {
                iter->index=index;
            } ଅନ୍ୟଥା {
                iter->index=-1; /* unknown UTF-16 index */
            }
            ଯଦି(state==0) {
                iter->reservedField=0;
            } ଅନ୍ୟଥା {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((ଅଚଳ uint8_t *)iter->context, 0, index, c);
                ଯଦି(c<=0xffff) {
                    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                } ଅନ୍ୟଥା {
                    iter->reservedField=c;
                }
            }
        }
    }
}

ସ୍ଥିର ଅଚଳ UCharIterator utf8Iterator={
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

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uiter_setUTF8(UCharIterator *iter, ଅଚଳ ଅକ୍ଷର *s, int32_t length) {
    ଯଦି (iter != nullptr) {
        ଯଦି (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            ଯଦି(length>=0) {
                iter->limit=length;
            } ଅନ୍ୟଥା {
                iter->limit=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->limit<=1 ? iter->limit : -1;
        } ଅନ୍ୟଥା {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    ଯଦି(U16_IS_SURROGATE(c)) {
        ଯଦି(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            ଯଦି(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } ଅନ୍ୟଥା {
            ଯଦି(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            ଯଦି(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    ଫେରନ୍ତୁ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    ଯଦି(U16_IS_LEAD(c)) {
        ଯଦି(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } ଅନ୍ୟଥା ଯଦି(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    ଫେରନ୍ତୁ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    ଯଦି(U16_IS_TRAIL(c)) {
        ଯଦି(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } ଅନ୍ୟଥା ଯଦି(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    ଫେରନ୍ତୁ c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ଅଚଳ UCharIterator *iter) {
    ଯଦି(iter==nullptr || iter->getState==nullptr) {
        ଫେରନ୍ତୁ UITER_NO_STATE;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ iter->getState(iter);
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ଅନ୍ୟଥା ଯଦି(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ଅନ୍ୟଥା ଯଦି(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } ଅନ୍ୟଥା {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
