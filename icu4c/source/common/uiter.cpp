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

#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/chariter.h"
#ضمن "unicode/rep.h"
#ضمن "unicode/uiter.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cstring.h"

U_NAMESPACE_USE

#define IS_EVEN(n) (((n)&1)==0)
#define IS_POINTER_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

ساكن int32_t U_CALLCONV
noopGetIndex(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    החזר 0;
}

ساكن int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    החזר 0;
}

ساكن UBool U_CALLCONV
noopHasNext(UCharIterator * /*iter*/) {
    החזר שקר;
}

ساكن UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    החזר U_SENTINEL;
}

ساكن uint32_t U_CALLCONV
noopGetState(ثابت UCharIterator * /*iter*/) {
    החזר UITER_NO_STATE;
}

ساكن فراغ U_CALLCONV
noopSetState(UCharIterator * /*iter*/, uint32_t /*state*/, UErrorCode *pErrorCode) {
    *pErrorCode=U_UNSUPPORTED_ERROR;
}

ساكن ثابت UCharIterator noopIterator={
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

ساكن int32_t U_CALLCONV
stringIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
        החזר 0;
    حالة UITER_START:
        החזר iter->start;
    حالة UITER_CURRENT:
        החזר iter->index;
    حالة UITER_LIMIT:
        החזר iter->limit;
    حالة UITER_LENGTH:
        החזר iter->length;
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        החזר -1;
    }
}

ساكن int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    بدل(origin) {
    حالة UITER_ZERO:
        pos=delta;
        اكسر;
    حالة UITER_START:
        pos=iter->start+delta;
        اكسر;
    حالة UITER_CURRENT:
        pos=iter->index+delta;
        اكسر;
    حالة UITER_LIMIT:
        pos=iter->limit+delta;
        اكسر;
    حالة UITER_LENGTH:
        pos=iter->length+delta;
        اكسر;
    افتراضي:
        החזר -1;  /* Error */
    }

    אם(pos<iter->start) {
        pos=iter->start;
    } אחרת אם(pos>iter->limit) {
        pos=iter->limit;
    }

    החזר iter->index=pos;
}

ساكن UBool U_CALLCONV
stringIteratorHasNext(UCharIterator *iter) {
    החזר iter->index<iter->limit;
}

ساكن UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    החזר iter->index>iter->start;
}

ساكن UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    אם(iter->index<iter->limit) {
        החזר ((ثابت char16_t *)(iter->context))[iter->index];
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
stringIteratorNext(UCharIterator *iter) {
    אם(iter->index<iter->limit) {
        החזר ((ثابت char16_t *)(iter->context))[iter->index++];
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    אם(iter->index>iter->start) {
        החזר ((ثابت char16_t *)(iter->context))[--iter->index];
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
stringIteratorGetState(ثابت UCharIterator *iter) {
    החזר (uint32_t)iter->index;
}

ساكن فراغ U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    אם(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } אחרת אם(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } אחרת אם((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } אחרת {
        iter->index=(int32_t)state;
    }
}

ساكن ثابت UCharIterator stringIterator={
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

U_CAPI فراغ U_EXPORT2
uiter_setString(UCharIterator *iter, ثابت char16_t *s, int32_t length) {
    אם (iter != nullptr) {
        אם (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            אם(length>=0) {
                iter->length=length;
            } אחרת {
                iter->length=u_strlen(s);
            }
            iter->limit=iter->length;
        } אחרת {
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
ساكن مضمن UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t index) {
    ثابت uint8_t *p=(ثابت uint8_t *)iter->context;
    החזר ((char16_t)p[2*index]<<8)|(char16_t)p[2*index+1];
}

ساكن UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t index;

    אם((index=iter->index)<iter->limit) {
        החזר utf16BEIteratorGet(iter, index);
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf16BEIteratorNext(UCharIterator *iter) {
    int32_t index;

    אם((index=iter->index)<iter->limit) {
        iter->index=index+1;
        החזר utf16BEIteratorGet(iter, index);
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t index;

    אם((index=iter->index)>iter->start) {
        iter->index=--index;
        החזר utf16BEIteratorGet(iter, index);
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن ثابت UCharIterator utf16BEIterator={
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
ساكن int32_t
utf16BE_strlen(ثابت حرف *s) {
    אם(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        החזר u_strlen((ثابت char16_t *)s);
    } אחרת {
        /* odd-aligned, search for pair of 0 bytes */
        ثابت حرف *p=s;

        כלעוד(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        החזר (int32_t)((p-s)/2);
    }
}

U_CAPI فراغ U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, ثابت حرف *s, int32_t length) {
    אם(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        אם(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            אם(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (ثابت char16_t *)s, length);
                החזר;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            אם(length>=0) {
                iter->length=length;
            } אחרת {
                iter->length=utf16BE_strlen(s);
            }
            iter->limit=iter->length;
        } אחרת {
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

ساكن int32_t U_CALLCONV
characterIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
        החזר 0;
    حالة UITER_START:
        החזר ((CharacterIterator *)(iter->context))->startIndex();
    حالة UITER_CURRENT:
        החזר ((CharacterIterator *)(iter->context))->getIndex();
    حالة UITER_LIMIT:
        החזר ((CharacterIterator *)(iter->context))->endIndex();
    حالة UITER_LENGTH:
        החזר ((CharacterIterator *)(iter->context))->getLength();
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        החזר -1;
    }
}

ساكن int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
        ((CharacterIterator *)(iter->context))->setIndex(delta);
        החזר ((CharacterIterator *)(iter->context))->getIndex();
    حالة UITER_START:
    حالة UITER_CURRENT:
    حالة UITER_LIMIT:
        החזר ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    حالة UITER_LENGTH:
        ((CharacterIterator *)(iter->context))->setIndex(((CharacterIterator *)(iter->context))->getLength()+delta);
        החזר ((CharacterIterator *)(iter->context))->getIndex();
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        החזר -1;
    }
}

ساكن UBool U_CALLCONV
characterIteratorHasNext(UCharIterator *iter) {
    החזר ((CharacterIterator *)(iter->context))->hasNext();
}

ساكن UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    החזר ((CharacterIterator *)(iter->context))->hasPrevious();
}

ساكن UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    אם(c!=0xffff || ((CharacterIterator *)(iter->context))->hasNext()) {
        החזר c;
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
characterIteratorNext(UCharIterator *iter) {
    אם(((CharacterIterator *)(iter->context))->hasNext()) {
        החזר ((CharacterIterator *)(iter->context))->nextPostInc();
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    אם(((CharacterIterator *)(iter->context))->hasPrevious()) {
        החזר ((CharacterIterator *)(iter->context))->previous();
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
characterIteratorGetState(ثابت UCharIterator *iter) {
    החזר ((CharacterIterator *)(iter->context))->getIndex();
}

ساكن فراغ U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    אם(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } אחרת אם(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } אחרת אם((int32_t)state<((CharacterIterator *)(iter->context))->startIndex() || ((CharacterIterator *)(iter->context))->endIndex()<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } אחרת {
        ((CharacterIterator *)(iter->context))->setIndex((int32_t)state);
    }
}

ساكن ثابت UCharIterator characterIteratorWrapper={
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

U_CAPI فراغ U_EXPORT2
uiter_setCharacterIterator(UCharIterator *iter, CharacterIterator *charIter) {
    אם (iter != nullptr) {
        אם (charIter != nullptr) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } אחרת {
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

ساكن UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    אם(iter->index<iter->limit) {
        החזר ((Replaceable *)(iter->context))->charAt(iter->index);
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
replaceableIteratorNext(UCharIterator *iter) {
    אם(iter->index<iter->limit) {
        החזר ((Replaceable *)(iter->context))->charAt(iter->index++);
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    אם(iter->index>iter->start) {
        החזר ((Replaceable *)(iter->context))->charAt(--iter->index);
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن ثابت UCharIterator replaceableIterator={
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

U_CAPI فراغ U_EXPORT2
uiter_setReplaceable(UCharIterator *iter, ثابت Replaceable *rep) {
    אם (iter != nullptr) {
        אם (rep != nullptr) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->limit=iter->length=rep->length();
        } אחרת {
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

ساكن int32_t U_CALLCONV
utf8IteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
    حالة UITER_START:
        החזר 0;
    حالة UITER_CURRENT:
        אם(iter->index<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ثابت uint8_t *s;
            UChar32 c;
            int32_t i, limit, index;

            s=(ثابت uint8_t *)iter->context;
            i=index=0;
            limit=iter->start; /* count up to the UTF-8 index */
            כלעוד(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                index+=U16_LENGTH(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            אם(i==iter->limit) {
                iter->length=index; /* in case it was <0 or wrong */
            }
            אם(iter->reservedField!=0) {
                --index; /* we are in the middle of a supplementary code point */
            }
            iter->index=index;
        }
        החזר iter->index;
    حالة UITER_LIMIT:
    حالة UITER_LENGTH:
        אם(iter->length<0) {
            ثابت uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(ثابت uint8_t *)iter->context;
            אם(iter->index<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                כלעוד(i<limit) {
                    U8_NEXT_OR_FFFD(s, i, limit, c);
                    length+=U16_LENGTH(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->index= iter->reservedField!=0 ? length-1 : length;
            } אחרת {
                i=iter->start;
                length=iter->index;
                אם(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            כלעוד(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                length+=U16_LENGTH(c);
            }
            iter->length=length;
        }
        החזר iter->length;
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        החזר -1;
    }
}

ساكن int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ثابت uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    بدل(origin) {
    حالة UITER_ZERO:
    حالة UITER_START:
        pos=delta;
        havePos=אמת;
        /* iter->index<0 (unknown) is possible */
        اكسر;
    حالة UITER_CURRENT:
        אם(iter->index>=0) {
            pos=iter->index+delta;
            havePos=אמת;
        } אחרת {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=שקר;
        }
        اكسر;
    حالة UITER_LIMIT:
    حالة UITER_LENGTH:
        אם(iter->length>=0) {
            pos=iter->length+delta;
            havePos=אמת;
        } אחרת {
            /* pin to the end, avoid counting the length */
            iter->index=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            אם(delta>=0) {
                החזר UITER_UNKNOWN_INDEX;
            } אחרת {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=שקר;
            }
        }
        اكسر;
    افتراضي:
        החזר -1;  /* Error */
    }

    אם(havePos) {
        /* shortcuts: pinning to the edges of the string */
        אם(pos<=0) {
            iter->index=iter->start=iter->reservedField=0;
            החזר 0;
        } אחרת אם(iter->length>=0 && pos>=iter->length) {
            iter->index=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            החזר iter->index;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        אם(iter->index<0 || pos<iter->index/2) {
            /* go forward from the start instead of backward from the current index */
            iter->index=iter->start=iter->reservedField=0;
        } אחרת אם(iter->length>=0 && (iter->length-pos)<(pos-iter->index)) {
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
        אם(delta==0) {
            החזר iter->index; /* nothing to do */
        }
    } אחרת {
        /* move relative to unknown UTF-16 index */
        אם(delta==0) {
            החזר UITER_UNKNOWN_INDEX; /* nothing to do */
        } אחרת אם(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->index=iter->start=iter->reservedField=0;
            החזר 0;
        } אחרת אם(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->index=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            החזר iter->index>=0 ? iter->index : (int32_t)UITER_UNKNOWN_INDEX;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ثابت uint8_t *)iter->context;
    pos=iter->index; /* could be <0 (unknown) */
    i=iter->start;
    אם(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        אם(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        כלעוד(delta>0 && i<limit) {
            U8_NEXT_OR_FFFD(s, i, limit, c);
            אם(c<=0xffff) {
                ++pos;
                --delta;
            } אחרת אם(delta>=2) {
                pos+=2;
                delta-=2;
            } אחרת /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                اكسر; /* delta=0; */
            }
        }
        אם(i==limit) {
            אם(iter->length<0 && iter->index>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } אחרת אם(iter->index<0 && iter->length>=0) {
                iter->index= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } אחרת /* delta<0 */ {
        /* go backward */
        אם(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        כלעוד(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            אם(c<=0xffff) {
                --pos;
                ++delta;
            } אחרת אם(delta<=-2) {
                pos-=2;
                delta+=2;
            } אחרת /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                اكسر; /* delta=0; */
            }
        }
    }

    iter->start=i;
    אם(iter->index>=0) {
        החזר iter->index=pos;
    } אחרת {
        /* we started with index<0 (unknown) so pos is bogus */
        אם(i<=1) {
            החזר iter->index=i; /* reached the beginning */
        } אחרת {
            /* we still don't know the UTF-16 index */
            החזר UITER_UNKNOWN_INDEX;
        }
    }
}

ساكن UBool U_CALLCONV
utf8IteratorHasNext(UCharIterator *iter) {
    החזר iter->start<iter->limit || iter->reservedField!=0;
}

ساكن UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    החזר iter->start>0;
}

ساكن UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    אם(iter->reservedField!=0) {
        החזר U16_TRAIL(iter->reservedField);
    } אחרת אם(iter->start<iter->limit) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_NEXT_OR_FFFD(s, i, iter->limit, c);
        אם(c<=0xffff) {
            החזר c;
        } אחרת {
            החזר U16_LEAD(c);
        }
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf8IteratorNext(UCharIterator *iter) {
    int32_t index;

    אם(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        אם((index=iter->index)>=0) {
            iter->index=index+1;
        }
        החזר trail;
    } אחרת אם(iter->start<iter->limit) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;

        U8_NEXT_OR_FFFD(s, iter->start, iter->limit, c);
        אם((index=iter->index)>=0) {
            iter->index=++index;
            אם(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? index : index+1;
            }
        } אחרת אם(iter->start==iter->limit && iter->length>=0) {
            iter->index= c<=0xffff ? iter->length : iter->length-1;
        }
        אם(c<=0xffff) {
            החזר c;
        } אחרת {
            iter->reservedField=c;
            החזר U16_LEAD(c);
        }
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t index;

    אם(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        אם((index=iter->index)>0) {
            iter->index=index-1;
        }
        החזר lead;
    } אחרת אם(iter->start>0) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->start, c);
        אם((index=iter->index)>0) {
            iter->index=index-1;
        } אחרת אם(iter->start<=1) {
            iter->index= c<=0xffff ? iter->start : iter->start+1;
        }
        אם(c<=0xffff) {
            החזר c;
        } אחרת {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            החזר U16_TRAIL(c);
        }
    } אחרת {
        החזר U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
utf8IteratorGetState(ثابت UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    אם(iter->reservedField!=0) {
        state|=1;
    }
    החזר state;
}

ساكن فراغ U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    אם(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } אחרת אם(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } אחרת אם(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } אחרת {
        int32_t index=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        אם((state==0 ? index<0 : index<4) || iter->limit<index) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        } אחרת {
            iter->start=index; /* restore UTF-8 byte index */
            אם(index<=1) {
                iter->index=index;
            } אחרת {
                iter->index=-1; /* unknown UTF-16 index */
            }
            אם(state==0) {
                iter->reservedField=0;
            } אחרת {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((ثابت uint8_t *)iter->context, 0, index, c);
                אם(c<=0xffff) {
                    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                } אחרת {
                    iter->reservedField=c;
                }
            }
        }
    }
}

ساكن ثابت UCharIterator utf8Iterator={
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

U_CAPI فراغ U_EXPORT2
uiter_setUTF8(UCharIterator *iter, ثابت حرف *s, int32_t length) {
    אם (iter != nullptr) {
        אם (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            אם(length>=0) {
                iter->limit=length;
            } אחרת {
                iter->limit=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->limit<=1 ? iter->limit : -1;
        } אחרת {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    אם(U16_IS_SURROGATE(c)) {
        אם(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            אם(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } אחרת {
            אם(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            אם(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    החזר c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    אם(U16_IS_LEAD(c)) {
        אם(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } אחרת אם(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    החזר c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    אם(U16_IS_TRAIL(c)) {
        אם(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } אחרת אם(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    החזר c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ثابت UCharIterator *iter) {
    אם(iter==nullptr || iter->getState==nullptr) {
        החזר UITER_NO_STATE;
    } אחרת {
        החזר iter->getState(iter);
    }
}

U_CAPI فراغ U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    אם(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } אחרת אם(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } אחרת אם(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } אחרת {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
