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
    врати 0;
}

static int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    врати 0;
}

static UBool U_CALLCONV
noopHasNext(UCharIterator * /*iter*/) {
    врати false;
}

static UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    врати U_SENTINEL;
}

static uint32_t U_CALLCONV
noopGetState(const UCharIterator * /*iter*/) {
    врати UITER_NO_STATE;
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
    case UITER_ZERO:
        врати 0;
    case UITER_START:
        врати iter->start;
    case UITER_CURRENT:
        врати iter->index;
    case UITER_LIMIT:
        врати iter->limit;
    case UITER_LENGTH:
        врати iter->length;
    default:
        /* not a valid origin */
        /* Should never get here! */
        врати -1;
    }
}

static int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    switch(origin) {
    case UITER_ZERO:
        pos=delta;
        break;
    case UITER_START:
        pos=iter->start+delta;
        break;
    case UITER_CURRENT:
        pos=iter->index+delta;
        break;
    case UITER_LIMIT:
        pos=iter->limit+delta;
        break;
    case UITER_LENGTH:
        pos=iter->length+delta;
        break;
    default:
        врати -1;  /* Error */
    }

    ако(pos<iter->start) {
        pos=iter->start;
    } инако ако(pos>iter->limit) {
        pos=iter->limit;
    }

    врати iter->index=pos;
}

static UBool U_CALLCONV
stringIteratorHasNext(UCharIterator *iter) {
    врати iter->index<iter->limit;
}

static UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    врати iter->index>iter->start;
}

static UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    ако(iter->index<iter->limit) {
        врати ((const char16_t *)(iter->context))[iter->index];
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
stringIteratorNext(UCharIterator *iter) {
    ако(iter->index<iter->limit) {
        врати ((const char16_t *)(iter->context))[iter->index++];
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    ако(iter->index>iter->start) {
        врати ((const char16_t *)(iter->context))[--iter->index];
    } инако {
        врати U_SENTINEL;
    }
}

static uint32_t U_CALLCONV
stringIteratorGetState(const UCharIterator *iter) {
    врати (uint32_t)iter->index;
}

static void U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ако(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } инако ако(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } инако ако((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } инако {
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
    ако (iter != nullptr) {
        ако (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            ако(length>=0) {
                iter->length=length;
            } инако {
                iter->length=u_strlen(s);
            }
            iter->limit=iter->length;
        } инако {
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
static inline UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t index) {
    const uint8_t *p=(const uint8_t *)iter->context;
    врати ((char16_t)p[2*index]<<8)|(char16_t)p[2*index+1];
}

static UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t index;

    ако((index=iter->index)<iter->limit) {
        врати utf16BEIteratorGet(iter, index);
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
utf16BEIteratorNext(UCharIterator *iter) {
    int32_t index;

    ако((index=iter->index)<iter->limit) {
        iter->index=index+1;
        врати utf16BEIteratorGet(iter, index);
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t index;

    ако((index=iter->index)>iter->start) {
        iter->index=--index;
        врати utf16BEIteratorGet(iter, index);
    } инако {
        врати U_SENTINEL;
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
utf16BE_strlen(const char *s) {
    ако(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        врати u_strlen((const char16_t *)s);
    } инако {
        /* odd-aligned, search for pair of 0 bytes */
        const char *p=s;

        док(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        врати (int32_t)((p-s)/2);
    }
}

U_CAPI void U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, const char *s, int32_t length) {
    ако(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        ако(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            ако(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (const char16_t *)s, length);
                врати;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            ако(length>=0) {
                iter->length=length;
            } инако {
                iter->length=utf16BE_strlen(s);
            }
            iter->limit=iter->length;
        } инако {
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
    case UITER_ZERO:
        врати 0;
    case UITER_START:
        врати ((CharacterIterator *)(iter->context))->startIndex();
    case UITER_CURRENT:
        врати ((CharacterIterator *)(iter->context))->getIndex();
    case UITER_LIMIT:
        врати ((CharacterIterator *)(iter->context))->endIndex();
    case UITER_LENGTH:
        врати ((CharacterIterator *)(iter->context))->getLength();
    default:
        /* not a valid origin */
        /* Should never get here! */
        врати -1;
    }
}

static int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    switch(origin) {
    case UITER_ZERO:
        ((CharacterIterator *)(iter->context))->setIndex(delta);
        врати ((CharacterIterator *)(iter->context))->getIndex();
    case UITER_START:
    case UITER_CURRENT:
    case UITER_LIMIT:
        врати ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    case UITER_LENGTH:
        ((CharacterIterator *)(iter->context))->setIndex(((CharacterIterator *)(iter->context))->getLength()+delta);
        врати ((CharacterIterator *)(iter->context))->getIndex();
    default:
        /* not a valid origin */
        /* Should never get here! */
        врати -1;
    }
}

static UBool U_CALLCONV
characterIteratorHasNext(UCharIterator *iter) {
    врати ((CharacterIterator *)(iter->context))->hasNext();
}

static UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    врати ((CharacterIterator *)(iter->context))->hasPrevious();
}

static UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    ако(c!=0xffff || ((CharacterIterator *)(iter->context))->hasNext()) {
        врати c;
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
characterIteratorNext(UCharIterator *iter) {
    ако(((CharacterIterator *)(iter->context))->hasNext()) {
        врати ((CharacterIterator *)(iter->context))->nextPostInc();
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    ако(((CharacterIterator *)(iter->context))->hasPrevious()) {
        врати ((CharacterIterator *)(iter->context))->previous();
    } инако {
        врати U_SENTINEL;
    }
}

static uint32_t U_CALLCONV
characterIteratorGetState(const UCharIterator *iter) {
    врати ((CharacterIterator *)(iter->context))->getIndex();
}

static void U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ако(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } инако ако(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } инако ако((int32_t)state<((CharacterIterator *)(iter->context))->startIndex() || ((CharacterIterator *)(iter->context))->endIndex()<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } инако {
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
    ако (iter != nullptr) {
        ако (charIter != nullptr) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } инако {
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
    ако(iter->index<iter->limit) {
        врати ((Replaceable *)(iter->context))->charAt(iter->index);
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
replaceableIteratorNext(UCharIterator *iter) {
    ако(iter->index<iter->limit) {
        врати ((Replaceable *)(iter->context))->charAt(iter->index++);
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    ако(iter->index>iter->start) {
        врати ((Replaceable *)(iter->context))->charAt(--iter->index);
    } инако {
        врати U_SENTINEL;
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
    ако (iter != nullptr) {
        ако (rep != nullptr) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->limit=iter->length=rep->length();
        } инако {
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
    case UITER_ZERO:
    case UITER_START:
        врати 0;
    case UITER_CURRENT:
        ако(iter->index<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            const uint8_t *s;
            UChar32 c;
            int32_t i, limit, index;

            s=(const uint8_t *)iter->context;
            i=index=0;
            limit=iter->start; /* count up to the UTF-8 index */
            док(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                index+=U16_LENGTH(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            ако(i==iter->limit) {
                iter->length=index; /* in case it was <0 or wrong */
            }
            ако(iter->reservedField!=0) {
                --index; /* we are in the middle of a supplementary code point */
            }
            iter->index=index;
        }
        врати iter->index;
    case UITER_LIMIT:
    case UITER_LENGTH:
        ако(iter->length<0) {
            const uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(const uint8_t *)iter->context;
            ако(iter->index<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                док(i<limit) {
                    U8_NEXT_OR_FFFD(s, i, limit, c);
                    length+=U16_LENGTH(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->index= iter->reservedField!=0 ? length-1 : length;
            } инако {
                i=iter->start;
                length=iter->index;
                ако(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            док(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                length+=U16_LENGTH(c);
            }
            iter->length=length;
        }
        врати iter->length;
    default:
        /* not a valid origin */
        /* Should never get here! */
        врати -1;
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
    case UITER_ZERO:
    case UITER_START:
        pos=delta;
        havePos=true;
        /* iter->index<0 (unknown) is possible */
        break;
    case UITER_CURRENT:
        ако(iter->index>=0) {
            pos=iter->index+delta;
            havePos=true;
        } инако {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=false;
        }
        break;
    case UITER_LIMIT:
    case UITER_LENGTH:
        ако(iter->length>=0) {
            pos=iter->length+delta;
            havePos=true;
        } инако {
            /* pin to the end, avoid counting the length */
            iter->index=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            ако(delta>=0) {
                врати UITER_UNKNOWN_INDEX;
            } инако {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=false;
            }
        }
        break;
    default:
        врати -1;  /* Error */
    }

    ако(havePos) {
        /* shortcuts: pinning to the edges of the string */
        ако(pos<=0) {
            iter->index=iter->start=iter->reservedField=0;
            врати 0;
        } инако ако(iter->length>=0 && pos>=iter->length) {
            iter->index=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            врати iter->index;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        ако(iter->index<0 || pos<iter->index/2) {
            /* go forward from the start instead of backward from the current index */
            iter->index=iter->start=iter->reservedField=0;
        } инако ако(iter->length>=0 && (iter->length-pos)<(pos-iter->index)) {
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
        ако(delta==0) {
            врати iter->index; /* nothing to do */
        }
    } инако {
        /* move relative to unknown UTF-16 index */
        ако(delta==0) {
            врати UITER_UNKNOWN_INDEX; /* nothing to do */
        } инако ако(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->index=iter->start=iter->reservedField=0;
            врати 0;
        } инако ако(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->index=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            врати iter->index>=0 ? iter->index : (int32_t)UITER_UNKNOWN_INDEX;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(const uint8_t *)iter->context;
    pos=iter->index; /* could be <0 (unknown) */
    i=iter->start;
    ако(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        ако(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        док(delta>0 && i<limit) {
            U8_NEXT_OR_FFFD(s, i, limit, c);
            ако(c<=0xffff) {
                ++pos;
                --delta;
            } инако ако(delta>=2) {
                pos+=2;
                delta-=2;
            } инако /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                break; /* delta=0; */
            }
        }
        ако(i==limit) {
            ако(iter->length<0 && iter->index>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } инако ако(iter->index<0 && iter->length>=0) {
                iter->index= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } инако /* delta<0 */ {
        /* go backward */
        ако(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        док(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            ако(c<=0xffff) {
                --pos;
                ++delta;
            } инако ако(delta<=-2) {
                pos-=2;
                delta+=2;
            } инако /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                break; /* delta=0; */
            }
        }
    }

    iter->start=i;
    ако(iter->index>=0) {
        врати iter->index=pos;
    } инако {
        /* we started with index<0 (unknown) so pos is bogus */
        ако(i<=1) {
            врати iter->index=i; /* reached the beginning */
        } инако {
            /* we still don't know the UTF-16 index */
            врати UITER_UNKNOWN_INDEX;
        }
    }
}

static UBool U_CALLCONV
utf8IteratorHasNext(UCharIterator *iter) {
    врати iter->start<iter->limit || iter->reservedField!=0;
}

static UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    врати iter->start>0;
}

static UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    ако(iter->reservedField!=0) {
        врати U16_TRAIL(iter->reservedField);
    } инако ако(iter->start<iter->limit) {
        const uint8_t *s=(const uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_NEXT_OR_FFFD(s, i, iter->limit, c);
        ако(c<=0xffff) {
            врати c;
        } инако {
            врати U16_LEAD(c);
        }
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
utf8IteratorNext(UCharIterator *iter) {
    int32_t index;

    ако(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        ако((index=iter->index)>=0) {
            iter->index=index+1;
        }
        врати trail;
    } инако ако(iter->start<iter->limit) {
        const uint8_t *s=(const uint8_t *)iter->context;
        UChar32 c;

        U8_NEXT_OR_FFFD(s, iter->start, iter->limit, c);
        ако((index=iter->index)>=0) {
            iter->index=++index;
            ако(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? index : index+1;
            }
        } инако ако(iter->start==iter->limit && iter->length>=0) {
            iter->index= c<=0xffff ? iter->length : iter->length-1;
        }
        ако(c<=0xffff) {
            врати c;
        } инако {
            iter->reservedField=c;
            врати U16_LEAD(c);
        }
    } инако {
        врати U_SENTINEL;
    }
}

static UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t index;

    ако(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        ако((index=iter->index)>0) {
            iter->index=index-1;
        }
        врати lead;
    } инако ако(iter->start>0) {
        const uint8_t *s=(const uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->start, c);
        ако((index=iter->index)>0) {
            iter->index=index-1;
        } инако ако(iter->start<=1) {
            iter->index= c<=0xffff ? iter->start : iter->start+1;
        }
        ако(c<=0xffff) {
            врати c;
        } инако {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            врати U16_TRAIL(c);
        }
    } инако {
        врати U_SENTINEL;
    }
}

static uint32_t U_CALLCONV
utf8IteratorGetState(const UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    ако(iter->reservedField!=0) {
        state|=1;
    }
    врати state;
}

static void U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    ако(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } инако ако(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } инако ако(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } инако {
        int32_t index=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        ако((state==0 ? index<0 : index<4) || iter->limit<index) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        } инако {
            iter->start=index; /* restore UTF-8 byte index */
            ако(index<=1) {
                iter->index=index;
            } инако {
                iter->index=-1; /* unknown UTF-16 index */
            }
            ако(state==0) {
                iter->reservedField=0;
            } инако {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((const uint8_t *)iter->context, 0, index, c);
                ако(c<=0xffff) {
                    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                } инако {
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
uiter_setUTF8(UCharIterator *iter, const char *s, int32_t length) {
    ако (iter != nullptr) {
        ако (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            ако(length>=0) {
                iter->limit=length;
            } инако {
                iter->limit=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->limit<=1 ? iter->limit : -1;
        } инако {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    ако(U16_IS_SURROGATE(c)) {
        ако(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            ако(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } инако {
            ако(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            ако(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    врати c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    ако(U16_IS_LEAD(c)) {
        ако(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } инако ако(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    врати c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    ако(U16_IS_TRAIL(c)) {
        ако(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } инако ако(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    врати c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(const UCharIterator *iter) {
    ако(iter==nullptr || iter->getState==nullptr) {
        врати UITER_NO_STATE;
    } инако {
        врати iter->getState(iter);
    }
}

U_CAPI void U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    ако(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } инако ако(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } инако ако(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } инако {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
