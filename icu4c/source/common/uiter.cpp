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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/chariter.h"
#अन्तर्भाव "unicode/rep.h"
#अन्तर्भाव "unicode/uiter.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cstring.h"

U_NAMESPACE_USE

#define IS_EVEN(n) (((n)&1)==0)
#define IS_POINTER_EVEN(p) IS_EVEN((माप_प्रकार)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

स्थिर int32_t U_CALLCONV
noopGetIndex(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    फर्काऊ 0;
}

स्थिर int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    फर्काऊ 0;
}

स्थिर UBool U_CALLCONV
noopHasNext(UCharIterator * /*iter*/) {
    फर्काऊ असत्य;
}

स्थिर UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    फर्काऊ U_SENTINEL;
}

स्थिर uint32_t U_CALLCONV
noopGetState(अचर UCharIterator * /*iter*/) {
    फर्काऊ UITER_NO_STATE;
}

स्थिर शून्य U_CALLCONV
noopSetState(UCharIterator * /*iter*/, uint32_t /*state*/, UErrorCode *pErrorCode) {
    *pErrorCode=U_UNSUPPORTED_ERROR;
}

स्थिर अचर UCharIterator noopIterator={
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

स्थिर int32_t U_CALLCONV
stringIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    विकल्प(origin) {
    पक्ष UITER_ZERO:
        फर्काऊ 0;
    पक्ष UITER_START:
        फर्काऊ iter->start;
    पक्ष UITER_CURRENT:
        फर्काऊ iter->index;
    पक्ष UITER_LIMIT:
        फर्काऊ iter->limit;
    पक्ष UITER_LENGTH:
        फर्काऊ iter->length;
    स्वभाव:
        /* not a valid origin */
        /* Should never get here! */
        फर्काऊ -1;
    }
}

स्थिर int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    विकल्प(origin) {
    पक्ष UITER_ZERO:
        pos=delta;
        रोक;
    पक्ष UITER_START:
        pos=iter->start+delta;
        रोक;
    पक्ष UITER_CURRENT:
        pos=iter->index+delta;
        रोक;
    पक्ष UITER_LIMIT:
        pos=iter->limit+delta;
        रोक;
    पक्ष UITER_LENGTH:
        pos=iter->length+delta;
        रोक;
    स्वभाव:
        फर्काऊ -1;  /* Error */
    }

    यदि(pos<iter->start) {
        pos=iter->start;
    } अन्यथा यदि(pos>iter->limit) {
        pos=iter->limit;
    }

    फर्काऊ iter->index=pos;
}

स्थिर UBool U_CALLCONV
stringIteratorHasNext(UCharIterator *iter) {
    फर्काऊ iter->index<iter->limit;
}

स्थिर UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    फर्काऊ iter->index>iter->start;
}

स्थिर UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    यदि(iter->index<iter->limit) {
        फर्काऊ ((अचर char16_t *)(iter->context))[iter->index];
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
stringIteratorNext(UCharIterator *iter) {
    यदि(iter->index<iter->limit) {
        फर्काऊ ((अचर char16_t *)(iter->context))[iter->index++];
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    यदि(iter->index>iter->start) {
        फर्काऊ ((अचर char16_t *)(iter->context))[--iter->index];
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर uint32_t U_CALLCONV
stringIteratorGetState(अचर UCharIterator *iter) {
    फर्काऊ (uint32_t)iter->index;
}

स्थिर शून्य U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } अन्यथा यदि(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा यदि((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } अन्यथा {
        iter->index=(int32_t)state;
    }
}

स्थिर अचर UCharIterator stringIterator={
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

U_CAPI शून्य U_EXPORT2
uiter_setString(UCharIterator *iter, अचर char16_t *s, int32_t length) {
    यदि (iter != nullptr) {
        यदि (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            यदि(length>=0) {
                iter->length=length;
            } अन्यथा {
                iter->length=u_strlen(s);
            }
            iter->limit=iter->length;
        } अन्यथा {
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
स्थिर अन्तःस्थ UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t index) {
    अचर uint8_t *p=(अचर uint8_t *)iter->context;
    फर्काऊ ((char16_t)p[2*index]<<8)|(char16_t)p[2*index+1];
}

स्थिर UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t index;

    यदि((index=iter->index)<iter->limit) {
        फर्काऊ utf16BEIteratorGet(iter, index);
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
utf16BEIteratorNext(UCharIterator *iter) {
    int32_t index;

    यदि((index=iter->index)<iter->limit) {
        iter->index=index+1;
        फर्काऊ utf16BEIteratorGet(iter, index);
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t index;

    यदि((index=iter->index)>iter->start) {
        iter->index=--index;
        फर्काऊ utf16BEIteratorGet(iter, index);
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर अचर UCharIterator utf16BEIterator={
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
स्थिर int32_t
utf16BE_strlen(अचर वर्ण *s) {
    यदि(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        फर्काऊ u_strlen((अचर char16_t *)s);
    } अन्यथा {
        /* odd-aligned, search for pair of 0 bytes */
        अचर वर्ण *p=s;

        जबसम्म(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        फर्काऊ (int32_t)((p-s)/2);
    }
}

U_CAPI शून्य U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, अचर वर्ण *s, int32_t length) {
    यदि(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        यदि(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            यदि(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (अचर char16_t *)s, length);
                फर्काऊ;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            यदि(length>=0) {
                iter->length=length;
            } अन्यथा {
                iter->length=utf16BE_strlen(s);
            }
            iter->limit=iter->length;
        } अन्यथा {
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

स्थिर int32_t U_CALLCONV
characterIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    विकल्प(origin) {
    पक्ष UITER_ZERO:
        फर्काऊ 0;
    पक्ष UITER_START:
        फर्काऊ ((CharacterIterator *)(iter->context))->startIndex();
    पक्ष UITER_CURRENT:
        फर्काऊ ((CharacterIterator *)(iter->context))->getIndex();
    पक्ष UITER_LIMIT:
        फर्काऊ ((CharacterIterator *)(iter->context))->endIndex();
    पक्ष UITER_LENGTH:
        फर्काऊ ((CharacterIterator *)(iter->context))->getLength();
    स्वभाव:
        /* not a valid origin */
        /* Should never get here! */
        फर्काऊ -1;
    }
}

स्थिर int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    विकल्प(origin) {
    पक्ष UITER_ZERO:
        ((CharacterIterator *)(iter->context))->setIndex(delta);
        फर्काऊ ((CharacterIterator *)(iter->context))->getIndex();
    पक्ष UITER_START:
    पक्ष UITER_CURRENT:
    पक्ष UITER_LIMIT:
        फर्काऊ ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    पक्ष UITER_LENGTH:
        ((CharacterIterator *)(iter->context))->setIndex(((CharacterIterator *)(iter->context))->getLength()+delta);
        फर्काऊ ((CharacterIterator *)(iter->context))->getIndex();
    स्वभाव:
        /* not a valid origin */
        /* Should never get here! */
        फर्काऊ -1;
    }
}

स्थिर UBool U_CALLCONV
characterIteratorHasNext(UCharIterator *iter) {
    फर्काऊ ((CharacterIterator *)(iter->context))->hasNext();
}

स्थिर UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    फर्काऊ ((CharacterIterator *)(iter->context))->hasPrevious();
}

स्थिर UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    यदि(c!=0xffff || ((CharacterIterator *)(iter->context))->hasNext()) {
        फर्काऊ c;
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
characterIteratorNext(UCharIterator *iter) {
    यदि(((CharacterIterator *)(iter->context))->hasNext()) {
        फर्काऊ ((CharacterIterator *)(iter->context))->nextPostInc();
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    यदि(((CharacterIterator *)(iter->context))->hasPrevious()) {
        फर्काऊ ((CharacterIterator *)(iter->context))->previous();
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर uint32_t U_CALLCONV
characterIteratorGetState(अचर UCharIterator *iter) {
    फर्काऊ ((CharacterIterator *)(iter->context))->getIndex();
}

स्थिर शून्य U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } अन्यथा यदि(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा यदि((int32_t)state<((CharacterIterator *)(iter->context))->startIndex() || ((CharacterIterator *)(iter->context))->endIndex()<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } अन्यथा {
        ((CharacterIterator *)(iter->context))->setIndex((int32_t)state);
    }
}

स्थिर अचर UCharIterator characterIteratorWrapper={
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

U_CAPI शून्य U_EXPORT2
uiter_setCharacterIterator(UCharIterator *iter, CharacterIterator *charIter) {
    यदि (iter != nullptr) {
        यदि (charIter != nullptr) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } अन्यथा {
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

स्थिर UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    यदि(iter->index<iter->limit) {
        फर्काऊ ((Replaceable *)(iter->context))->charAt(iter->index);
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
replaceableIteratorNext(UCharIterator *iter) {
    यदि(iter->index<iter->limit) {
        फर्काऊ ((Replaceable *)(iter->context))->charAt(iter->index++);
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    यदि(iter->index>iter->start) {
        फर्काऊ ((Replaceable *)(iter->context))->charAt(--iter->index);
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर अचर UCharIterator replaceableIterator={
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

U_CAPI शून्य U_EXPORT2
uiter_setReplaceable(UCharIterator *iter, अचर Replaceable *rep) {
    यदि (iter != nullptr) {
        यदि (rep != nullptr) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->limit=iter->length=rep->length();
        } अन्यथा {
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

स्थिर int32_t U_CALLCONV
utf8IteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    विकल्प(origin) {
    पक्ष UITER_ZERO:
    पक्ष UITER_START:
        फर्काऊ 0;
    पक्ष UITER_CURRENT:
        यदि(iter->index<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            अचर uint8_t *s;
            UChar32 c;
            int32_t i, limit, index;

            s=(अचर uint8_t *)iter->context;
            i=index=0;
            limit=iter->start; /* count up to the UTF-8 index */
            जबसम्म(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                index+=U16_LENGTH(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            यदि(i==iter->limit) {
                iter->length=index; /* in case it was <0 or wrong */
            }
            यदि(iter->reservedField!=0) {
                --index; /* we are in the middle of a supplementary code point */
            }
            iter->index=index;
        }
        फर्काऊ iter->index;
    पक्ष UITER_LIMIT:
    पक्ष UITER_LENGTH:
        यदि(iter->length<0) {
            अचर uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(अचर uint8_t *)iter->context;
            यदि(iter->index<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                जबसम्म(i<limit) {
                    U8_NEXT_OR_FFFD(s, i, limit, c);
                    length+=U16_LENGTH(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->index= iter->reservedField!=0 ? length-1 : length;
            } अन्यथा {
                i=iter->start;
                length=iter->index;
                यदि(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            जबसम्म(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                length+=U16_LENGTH(c);
            }
            iter->length=length;
        }
        फर्काऊ iter->length;
    स्वभाव:
        /* not a valid origin */
        /* Should never get here! */
        फर्काऊ -1;
    }
}

स्थिर int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    अचर uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    विकल्प(origin) {
    पक्ष UITER_ZERO:
    पक्ष UITER_START:
        pos=delta;
        havePos=सत्य;
        /* iter->index<0 (unknown) is possible */
        रोक;
    पक्ष UITER_CURRENT:
        यदि(iter->index>=0) {
            pos=iter->index+delta;
            havePos=सत्य;
        } अन्यथा {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=असत्य;
        }
        रोक;
    पक्ष UITER_LIMIT:
    पक्ष UITER_LENGTH:
        यदि(iter->length>=0) {
            pos=iter->length+delta;
            havePos=सत्य;
        } अन्यथा {
            /* pin to the end, avoid counting the length */
            iter->index=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            यदि(delta>=0) {
                फर्काऊ UITER_UNKNOWN_INDEX;
            } अन्यथा {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=असत्य;
            }
        }
        रोक;
    स्वभाव:
        फर्काऊ -1;  /* Error */
    }

    यदि(havePos) {
        /* shortcuts: pinning to the edges of the string */
        यदि(pos<=0) {
            iter->index=iter->start=iter->reservedField=0;
            फर्काऊ 0;
        } अन्यथा यदि(iter->length>=0 && pos>=iter->length) {
            iter->index=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            फर्काऊ iter->index;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        यदि(iter->index<0 || pos<iter->index/2) {
            /* go forward from the start instead of backward from the current index */
            iter->index=iter->start=iter->reservedField=0;
        } अन्यथा यदि(iter->length>=0 && (iter->length-pos)<(pos-iter->index)) {
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
        यदि(delta==0) {
            फर्काऊ iter->index; /* nothing to do */
        }
    } अन्यथा {
        /* move relative to unknown UTF-16 index */
        यदि(delta==0) {
            फर्काऊ UITER_UNKNOWN_INDEX; /* nothing to do */
        } अन्यथा यदि(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->index=iter->start=iter->reservedField=0;
            फर्काऊ 0;
        } अन्यथा यदि(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->index=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            फर्काऊ iter->index>=0 ? iter->index : (int32_t)UITER_UNKNOWN_INDEX;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(अचर uint8_t *)iter->context;
    pos=iter->index; /* could be <0 (unknown) */
    i=iter->start;
    यदि(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        यदि(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        जबसम्म(delta>0 && i<limit) {
            U8_NEXT_OR_FFFD(s, i, limit, c);
            यदि(c<=0xffff) {
                ++pos;
                --delta;
            } अन्यथा यदि(delta>=2) {
                pos+=2;
                delta-=2;
            } अन्यथा /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                रोक; /* delta=0; */
            }
        }
        यदि(i==limit) {
            यदि(iter->length<0 && iter->index>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } अन्यथा यदि(iter->index<0 && iter->length>=0) {
                iter->index= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } अन्यथा /* delta<0 */ {
        /* go backward */
        यदि(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        जबसम्म(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            यदि(c<=0xffff) {
                --pos;
                ++delta;
            } अन्यथा यदि(delta<=-2) {
                pos-=2;
                delta+=2;
            } अन्यथा /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                रोक; /* delta=0; */
            }
        }
    }

    iter->start=i;
    यदि(iter->index>=0) {
        फर्काऊ iter->index=pos;
    } अन्यथा {
        /* we started with index<0 (unknown) so pos is bogus */
        यदि(i<=1) {
            फर्काऊ iter->index=i; /* reached the beginning */
        } अन्यथा {
            /* we still don't know the UTF-16 index */
            फर्काऊ UITER_UNKNOWN_INDEX;
        }
    }
}

स्थिर UBool U_CALLCONV
utf8IteratorHasNext(UCharIterator *iter) {
    फर्काऊ iter->start<iter->limit || iter->reservedField!=0;
}

स्थिर UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    फर्काऊ iter->start>0;
}

स्थिर UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    यदि(iter->reservedField!=0) {
        फर्काऊ U16_TRAIL(iter->reservedField);
    } अन्यथा यदि(iter->start<iter->limit) {
        अचर uint8_t *s=(अचर uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_NEXT_OR_FFFD(s, i, iter->limit, c);
        यदि(c<=0xffff) {
            फर्काऊ c;
        } अन्यथा {
            फर्काऊ U16_LEAD(c);
        }
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
utf8IteratorNext(UCharIterator *iter) {
    int32_t index;

    यदि(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        यदि((index=iter->index)>=0) {
            iter->index=index+1;
        }
        फर्काऊ trail;
    } अन्यथा यदि(iter->start<iter->limit) {
        अचर uint8_t *s=(अचर uint8_t *)iter->context;
        UChar32 c;

        U8_NEXT_OR_FFFD(s, iter->start, iter->limit, c);
        यदि((index=iter->index)>=0) {
            iter->index=++index;
            यदि(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? index : index+1;
            }
        } अन्यथा यदि(iter->start==iter->limit && iter->length>=0) {
            iter->index= c<=0xffff ? iter->length : iter->length-1;
        }
        यदि(c<=0xffff) {
            फर्काऊ c;
        } अन्यथा {
            iter->reservedField=c;
            फर्काऊ U16_LEAD(c);
        }
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t index;

    यदि(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        यदि((index=iter->index)>0) {
            iter->index=index-1;
        }
        फर्काऊ lead;
    } अन्यथा यदि(iter->start>0) {
        अचर uint8_t *s=(अचर uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->start, c);
        यदि((index=iter->index)>0) {
            iter->index=index-1;
        } अन्यथा यदि(iter->start<=1) {
            iter->index= c<=0xffff ? iter->start : iter->start+1;
        }
        यदि(c<=0xffff) {
            फर्काऊ c;
        } अन्यथा {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            फर्काऊ U16_TRAIL(c);
        }
    } अन्यथा {
        फर्काऊ U_SENTINEL;
    }
}

स्थिर uint32_t U_CALLCONV
utf8IteratorGetState(अचर UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    यदि(iter->reservedField!=0) {
        state|=1;
    }
    फर्काऊ state;
}

स्थिर शून्य U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } अन्यथा यदि(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा यदि(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } अन्यथा {
        int32_t index=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        यदि((state==0 ? index<0 : index<4) || iter->limit<index) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        } अन्यथा {
            iter->start=index; /* restore UTF-8 byte index */
            यदि(index<=1) {
                iter->index=index;
            } अन्यथा {
                iter->index=-1; /* unknown UTF-16 index */
            }
            यदि(state==0) {
                iter->reservedField=0;
            } अन्यथा {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((अचर uint8_t *)iter->context, 0, index, c);
                यदि(c<=0xffff) {
                    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                } अन्यथा {
                    iter->reservedField=c;
                }
            }
        }
    }
}

स्थिर अचर UCharIterator utf8Iterator={
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

U_CAPI शून्य U_EXPORT2
uiter_setUTF8(UCharIterator *iter, अचर वर्ण *s, int32_t length) {
    यदि (iter != nullptr) {
        यदि (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            यदि(length>=0) {
                iter->limit=length;
            } अन्यथा {
                iter->limit=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->limit<=1 ? iter->limit : -1;
        } अन्यथा {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    यदि(U16_IS_SURROGATE(c)) {
        यदि(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            यदि(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } अन्यथा {
            यदि(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            यदि(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    फर्काऊ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    यदि(U16_IS_LEAD(c)) {
        यदि(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } अन्यथा यदि(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    फर्काऊ c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    यदि(U16_IS_TRAIL(c)) {
        यदि(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } अन्यथा यदि(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    फर्काऊ c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(अचर UCharIterator *iter) {
    यदि(iter==nullptr || iter->getState==nullptr) {
        फर्काऊ UITER_NO_STATE;
    } अन्यथा {
        फर्काऊ iter->getState(iter);
    }
}

U_CAPI शून्य U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } अन्यथा यदि(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा यदि(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } अन्यथा {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
