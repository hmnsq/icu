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

#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/chariter.h"
#include "unicode/rep.h"
#include "unicode/uiter.h"
#include "unicode/utf.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "cstring.h"

U_NAMESPACE_USE

#define IS_EVEN(n) (((n)&1)==0)
#define IS_POINTER_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

static int32_t U_CALLCONV
noopGetIndex(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    ᯀᯮᯞ 0;
}

static int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    ᯀᯮᯞ 0;
}

static UBool U_CALLCONV
noopHasNext(UCharIterator * /*iter*/) {
    ᯀᯮᯞ false;
}

static UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    ᯀᯮᯞ U_SENTINEL;
}

static uint32_t U_CALLCONV
noopGetState(const UCharIterator * /*iter*/) {
    ᯀᯮᯞ UITER_NO_STATE;
}

static void U_CALLCONV
noopSetState(UCharIterator * /*iter*/, uint32_t /*state*/, UErrorCode *pErrorCode) {
    *pErrorCode=U_UNSUPPORTED_ERROR;
}

static const UCharIterator noopIterator={
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

static int32_t U_CALLCONV
stringIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    switch(origin) {
    pak_sa UITER_ZERO:
        ᯀᯮᯞ 0;
    pak_sa UITER_START:
        ᯀᯮᯞ iter->start;
    pak_sa UITER_CURRENT:
        ᯀᯮᯞ iter->index;
    pak_sa UITER_LIMIT:
        ᯀᯮᯞ iter->limit;
    pak_sa UITER_LENGTH:
        ᯀᯮᯞ iter->length;
    default:
        /* not a valid origin */
        /* Should never get here! */
        ᯀᯮᯞ -1;
    }
}

static int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    switch(origin) {
    pak_sa UITER_ZERO:
        pos=delta;
        bha_nga;
    pak_sa UITER_START:
        pos=iter->start+delta;
        bha_nga;
    pak_sa UITER_CURRENT:
        pos=iter->index+delta;
        bha_nga;
    pak_sa UITER_LIMIT:
        pos=iter->limit+delta;
        bha_nga;
    pak_sa UITER_LENGTH:
        pos=iter->length+delta;
        bha_nga;
    default:
        ᯀᯮᯞ -1;  /* Error */
    }

    ᯀᯮᯝ(pos<iter->start) {
        pos=iter->start;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(pos>iter->limit) {
        pos=iter->limit;
    }

    ᯀᯮᯞ iter->index=pos;
}

static UBool U_CALLCONV
stringIteratorHasNext(UCharIterator *iter) {
    ᯀᯮᯞ iter->index<iter->limit;
}

static UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    ᯀᯮᯞ iter->index>iter->start;
}

static UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    ᯀᯮᯝ(iter->index<iter->limit) {
        ᯀᯮᯞ ((const char16_t *)(iter->context))[iter->index];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
stringIteratorNext(UCharIterator *iter) {
    ᯀᯮᯝ(iter->index<iter->limit) {
        ᯀᯮᯞ ((const char16_t *)(iter->context))[iter->index++];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    ᯀᯮᯝ(iter->index>iter->start) {
        ᯀᯮᯞ ((const char16_t *)(iter->context))[--iter->index];
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static uint32_t U_CALLCONV
stringIteratorGetState(const UCharIterator *iter) {
    ᯀᯮᯞ (uint32_t)iter->index;
}

static void U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } ᯀᯮᯝᯂᯪ {
        iter->index=(int32_t)state;
    }
}

static const UCharIterator stringIterator={
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

U_CAPI void U_EXPORT2
uiter_setString(UCharIterator *iter, const char16_t *s, int32_t length) {
    ᯀᯮᯝ (iter != nullptr) {
        ᯀᯮᯝ (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            ᯀᯮᯝ(length>=0) {
                iter->length=length;
            } ᯀᯮᯝᯂᯪ {
                iter->length=u_strlen(s);
            }
            iter->limit=iter->length;
        } ᯀᯮᯝᯂᯪ {
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
static anta_hstha UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t index) {
    const uint8_t *p=(const uint8_t *)iter->context;
    ᯀᯮᯞ ((char16_t)p[2*index]<<8)|(char16_t)p[2*index+1];
}

static UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t index;

    ᯀᯮᯝ((index=iter->index)<iter->limit) {
        ᯀᯮᯞ utf16BEIteratorGet(iter, index);
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
utf16BEIteratorNext(UCharIterator *iter) {
    int32_t index;

    ᯀᯮᯝ((index=iter->index)<iter->limit) {
        iter->index=index+1;
        ᯀᯮᯞ utf16BEIteratorGet(iter, index);
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t index;

    ᯀᯮᯝ((index=iter->index)>iter->start) {
        iter->index=--index;
        ᯀᯮᯞ utf16BEIteratorGet(iter, index);
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static const UCharIterator utf16BEIterator={
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
static int32_t
utf16BE_strlen(const var_na *s) {
    ᯀᯮᯝ(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        ᯀᯮᯞ u_strlen((const char16_t *)s);
    } ᯀᯮᯝᯂᯪ {
        /* odd-aligned, search for pair of 0 bytes */
        const var_na *p=s;

        ᯀᯬᯝ(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        ᯀᯮᯞ (int32_t)((p-s)/2);
    }
}

U_CAPI void U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, const var_na *s, int32_t length) {
    ᯀᯮᯝ(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        ᯀᯮᯝ(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            ᯀᯮᯝ(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (const char16_t *)s, length);
                ᯀᯮᯞ;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            ᯀᯮᯝ(length>=0) {
                iter->length=length;
            } ᯀᯮᯝᯂᯪ {
                iter->length=utf16BE_strlen(s);
            }
            iter->limit=iter->length;
        } ᯀᯮᯝᯂᯪ {
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

static int32_t U_CALLCONV
characterIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    switch(origin) {
    pak_sa UITER_ZERO:
        ᯀᯮᯞ 0;
    pak_sa UITER_START:
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->startIndex();
    pak_sa UITER_CURRENT:
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->getIndex();
    pak_sa UITER_LIMIT:
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->endIndex();
    pak_sa UITER_LENGTH:
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->getLength();
    default:
        /* not a valid origin */
        /* Should never get here! */
        ᯀᯮᯞ -1;
    }
}

static int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    switch(origin) {
    pak_sa UITER_ZERO:
        ((CharacterIterator *)(iter->context))->setIndex(delta);
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->getIndex();
    pak_sa UITER_START:
    pak_sa UITER_CURRENT:
    pak_sa UITER_LIMIT:
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    pak_sa UITER_LENGTH:
        ((CharacterIterator *)(iter->context))->setIndex(((CharacterIterator *)(iter->context))->getLength()+delta);
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->getIndex();
    default:
        /* not a valid origin */
        /* Should never get here! */
        ᯀᯮᯞ -1;
    }
}

static UBool U_CALLCONV
characterIteratorHasNext(UCharIterator *iter) {
    ᯀᯮᯞ ((CharacterIterator *)(iter->context))->hasNext();
}

static UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    ᯀᯮᯞ ((CharacterIterator *)(iter->context))->hasPrevious();
}

static UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    ᯀᯮᯝ(c!=0xffff || ((CharacterIterator *)(iter->context))->hasNext()) {
        ᯀᯮᯞ c;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
characterIteratorNext(UCharIterator *iter) {
    ᯀᯮᯝ(((CharacterIterator *)(iter->context))->hasNext()) {
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->nextPostInc();
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    ᯀᯮᯝ(((CharacterIterator *)(iter->context))->hasPrevious()) {
        ᯀᯮᯞ ((CharacterIterator *)(iter->context))->previous();
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static uint32_t U_CALLCONV
characterIteratorGetState(const UCharIterator *iter) {
    ᯀᯮᯞ ((CharacterIterator *)(iter->context))->getIndex();
}

static void U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ((int32_t)state<((CharacterIterator *)(iter->context))->startIndex() || ((CharacterIterator *)(iter->context))->endIndex()<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } ᯀᯮᯝᯂᯪ {
        ((CharacterIterator *)(iter->context))->setIndex((int32_t)state);
    }
}

static const UCharIterator characterIteratorWrapper={
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

U_CAPI void U_EXPORT2
uiter_setCharacterIterator(UCharIterator *iter, CharacterIterator *charIter) {
    ᯀᯮᯝ (iter != nullptr) {
        ᯀᯮᯝ (charIter != nullptr) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } ᯀᯮᯝᯂᯪ {
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

static UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    ᯀᯮᯝ(iter->index<iter->limit) {
        ᯀᯮᯞ ((Replaceable *)(iter->context))->charAt(iter->index);
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
replaceableIteratorNext(UCharIterator *iter) {
    ᯀᯮᯝ(iter->index<iter->limit) {
        ᯀᯮᯞ ((Replaceable *)(iter->context))->charAt(iter->index++);
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    ᯀᯮᯝ(iter->index>iter->start) {
        ᯀᯮᯞ ((Replaceable *)(iter->context))->charAt(--iter->index);
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static const UCharIterator replaceableIterator={
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

U_CAPI void U_EXPORT2
uiter_setReplaceable(UCharIterator *iter, const Replaceable *rep) {
    ᯀᯮᯝ (iter != nullptr) {
        ᯀᯮᯝ (rep != nullptr) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->limit=iter->length=rep->length();
        } ᯀᯮᯝᯂᯪ {
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

static int32_t U_CALLCONV
utf8IteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    switch(origin) {
    pak_sa UITER_ZERO:
    pak_sa UITER_START:
        ᯀᯮᯞ 0;
    pak_sa UITER_CURRENT:
        ᯀᯮᯝ(iter->index<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            const uint8_t *s;
            UChar32 c;
            int32_t i, limit, index;

            s=(const uint8_t *)iter->context;
            i=index=0;
            limit=iter->start; /* count up to the UTF-8 index */
            ᯀᯬᯝ(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                index+=U16_LENGTH(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            ᯀᯮᯝ(i==iter->limit) {
                iter->length=index; /* in case it was <0 or wrong */
            }
            ᯀᯮᯝ(iter->reservedField!=0) {
                --index; /* we are in the middle of a supplementary code point */
            }
            iter->index=index;
        }
        ᯀᯮᯞ iter->index;
    pak_sa UITER_LIMIT:
    pak_sa UITER_LENGTH:
        ᯀᯮᯝ(iter->length<0) {
            const uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(const uint8_t *)iter->context;
            ᯀᯮᯝ(iter->index<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                ᯀᯬᯝ(i<limit) {
                    U8_NEXT_OR_FFFD(s, i, limit, c);
                    length+=U16_LENGTH(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->index= iter->reservedField!=0 ? length-1 : length;
            } ᯀᯮᯝᯂᯪ {
                i=iter->start;
                length=iter->index;
                ᯀᯮᯝ(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            ᯀᯬᯝ(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                length+=U16_LENGTH(c);
            }
            iter->length=length;
        }
        ᯀᯮᯞ iter->length;
    default:
        /* not a valid origin */
        /* Should never get here! */
        ᯀᯮᯞ -1;
    }
}

static int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    const uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    switch(origin) {
    pak_sa UITER_ZERO:
    pak_sa UITER_START:
        pos=delta;
        havePos=true;
        /* iter->index<0 (unknown) is possible */
        bha_nga;
    pak_sa UITER_CURRENT:
        ᯀᯮᯝ(iter->index>=0) {
            pos=iter->index+delta;
            havePos=true;
        } ᯀᯮᯝᯂᯪ {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=false;
        }
        bha_nga;
    pak_sa UITER_LIMIT:
    pak_sa UITER_LENGTH:
        ᯀᯮᯝ(iter->length>=0) {
            pos=iter->length+delta;
            havePos=true;
        } ᯀᯮᯝᯂᯪ {
            /* pin to the end, avoid counting the length */
            iter->index=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            ᯀᯮᯝ(delta>=0) {
                ᯀᯮᯞ UITER_UNKNOWN_INDEX;
            } ᯀᯮᯝᯂᯪ {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=false;
            }
        }
        bha_nga;
    default:
        ᯀᯮᯞ -1;  /* Error */
    }

    ᯀᯮᯝ(havePos) {
        /* shortcuts: pinning to the edges of the string */
        ᯀᯮᯝ(pos<=0) {
            iter->index=iter->start=iter->reservedField=0;
            ᯀᯮᯞ 0;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->length>=0 && pos>=iter->length) {
            iter->index=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            ᯀᯮᯞ iter->index;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        ᯀᯮᯝ(iter->index<0 || pos<iter->index/2) {
            /* go forward from the start instead of backward from the current index */
            iter->index=iter->start=iter->reservedField=0;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->length>=0 && (iter->length-pos)<(pos-iter->index)) {
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
        ᯀᯮᯝ(delta==0) {
            ᯀᯮᯞ iter->index; /* nothing to do */
        }
    } ᯀᯮᯝᯂᯪ {
        /* move relative to unknown UTF-16 index */
        ᯀᯮᯝ(delta==0) {
            ᯀᯮᯞ UITER_UNKNOWN_INDEX; /* nothing to do */
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->index=iter->start=iter->reservedField=0;
            ᯀᯮᯞ 0;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->index=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            ᯀᯮᯞ iter->index>=0 ? iter->index : (int32_t)UITER_UNKNOWN_INDEX;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(const uint8_t *)iter->context;
    pos=iter->index; /* could be <0 (unknown) */
    i=iter->start;
    ᯀᯮᯝ(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        ᯀᯮᯝ(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        ᯀᯬᯝ(delta>0 && i<limit) {
            U8_NEXT_OR_FFFD(s, i, limit, c);
            ᯀᯮᯝ(c<=0xffff) {
                ++pos;
                --delta;
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(delta>=2) {
                pos+=2;
                delta-=2;
            } ᯀᯮᯝᯂᯪ /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                bha_nga; /* delta=0; */
            }
        }
        ᯀᯮᯝ(i==limit) {
            ᯀᯮᯝ(iter->length<0 && iter->index>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->index<0 && iter->length>=0) {
                iter->index= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } ᯀᯮᯝᯂᯪ /* delta<0 */ {
        /* go backward */
        ᯀᯮᯝ(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        ᯀᯬᯝ(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            ᯀᯮᯝ(c<=0xffff) {
                --pos;
                ++delta;
            } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(delta<=-2) {
                pos-=2;
                delta+=2;
            } ᯀᯮᯝᯂᯪ /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                bha_nga; /* delta=0; */
            }
        }
    }

    iter->start=i;
    ᯀᯮᯝ(iter->index>=0) {
        ᯀᯮᯞ iter->index=pos;
    } ᯀᯮᯝᯂᯪ {
        /* we started with index<0 (unknown) so pos is bogus */
        ᯀᯮᯝ(i<=1) {
            ᯀᯮᯞ iter->index=i; /* reached the beginning */
        } ᯀᯮᯝᯂᯪ {
            /* we still don't know the UTF-16 index */
            ᯀᯮᯞ UITER_UNKNOWN_INDEX;
        }
    }
}

static UBool U_CALLCONV
utf8IteratorHasNext(UCharIterator *iter) {
    ᯀᯮᯞ iter->start<iter->limit || iter->reservedField!=0;
}

static UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    ᯀᯮᯞ iter->start>0;
}

static UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    ᯀᯮᯝ(iter->reservedField!=0) {
        ᯀᯮᯞ U16_TRAIL(iter->reservedField);
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->start<iter->limit) {
        const uint8_t *s=(const uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_NEXT_OR_FFFD(s, i, iter->limit, c);
        ᯀᯮᯝ(c<=0xffff) {
            ᯀᯮᯞ c;
        } ᯀᯮᯝᯂᯪ {
            ᯀᯮᯞ U16_LEAD(c);
        }
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
utf8IteratorNext(UCharIterator *iter) {
    int32_t index;

    ᯀᯮᯝ(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        ᯀᯮᯝ((index=iter->index)>=0) {
            iter->index=index+1;
        }
        ᯀᯮᯞ trail;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->start<iter->limit) {
        const uint8_t *s=(const uint8_t *)iter->context;
        UChar32 c;

        U8_NEXT_OR_FFFD(s, iter->start, iter->limit, c);
        ᯀᯮᯝ((index=iter->index)>=0) {
            iter->index=++index;
            ᯀᯮᯝ(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? index : index+1;
            }
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->start==iter->limit && iter->length>=0) {
            iter->index= c<=0xffff ? iter->length : iter->length-1;
        }
        ᯀᯮᯝ(c<=0xffff) {
            ᯀᯮᯞ c;
        } ᯀᯮᯝᯂᯪ {
            iter->reservedField=c;
            ᯀᯮᯞ U16_LEAD(c);
        }
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t index;

    ᯀᯮᯝ(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        ᯀᯮᯝ((index=iter->index)>0) {
            iter->index=index-1;
        }
        ᯀᯮᯞ lead;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->start>0) {
        const uint8_t *s=(const uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->start, c);
        ᯀᯮᯝ((index=iter->index)>0) {
            iter->index=index-1;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->start<=1) {
            iter->index= c<=0xffff ? iter->start : iter->start+1;
        }
        ᯀᯮᯝ(c<=0xffff) {
            ᯀᯮᯞ c;
        } ᯀᯮᯝᯂᯪ {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            ᯀᯮᯞ U16_TRAIL(c);
        }
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ U_SENTINEL;
    }
}

static uint32_t U_CALLCONV
utf8IteratorGetState(const UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    ᯀᯮᯝ(iter->reservedField!=0) {
        state|=1;
    }
    ᯀᯮᯞ state;
}

static void U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } ᯀᯮᯝᯂᯪ {
        int32_t index=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        ᯀᯮᯝ((state==0 ? index<0 : index<4) || iter->limit<index) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        } ᯀᯮᯝᯂᯪ {
            iter->start=index; /* restore UTF-8 byte index */
            ᯀᯮᯝ(index<=1) {
                iter->index=index;
            } ᯀᯮᯝᯂᯪ {
                iter->index=-1; /* unknown UTF-16 index */
            }
            ᯀᯮᯝ(state==0) {
                iter->reservedField=0;
            } ᯀᯮᯝᯂᯪ {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((const uint8_t *)iter->context, 0, index, c);
                ᯀᯮᯝ(c<=0xffff) {
                    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                } ᯀᯮᯝᯂᯪ {
                    iter->reservedField=c;
                }
            }
        }
    }
}

static const UCharIterator utf8Iterator={
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

U_CAPI void U_EXPORT2
uiter_setUTF8(UCharIterator *iter, const var_na *s, int32_t length) {
    ᯀᯮᯝ (iter != nullptr) {
        ᯀᯮᯝ (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            ᯀᯮᯝ(length>=0) {
                iter->limit=length;
            } ᯀᯮᯝᯂᯪ {
                iter->limit=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->limit<=1 ? iter->limit : -1;
        } ᯀᯮᯝᯂᯪ {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    ᯀᯮᯝ(U16_IS_SURROGATE(c)) {
        ᯀᯮᯝ(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            ᯀᯮᯝ(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } ᯀᯮᯝᯂᯪ {
            ᯀᯮᯝ(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            ᯀᯮᯝ(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    ᯀᯮᯞ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    ᯀᯮᯝ(U16_IS_LEAD(c)) {
        ᯀᯮᯝ(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    ᯀᯮᯞ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    ᯀᯮᯝ(U16_IS_TRAIL(c)) {
        ᯀᯮᯝ(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    ᯀᯮᯞ c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(const UCharIterator *iter) {
    ᯀᯮᯝ(iter==nullptr || iter->getState==nullptr) {
        ᯀᯮᯞ UITER_NO_STATE;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ iter->getState(iter);
    }
}

U_CAPI void U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } ᯀᯮᯝᯂᯪ {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
