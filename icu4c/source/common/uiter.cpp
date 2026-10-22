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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/chariter.h"
#அந்தர்பாவ "unicode/rep.h"
#அந்தர்பாவ "unicode/uiter.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cstring.h"

U_NAMESPACE_USE

#define IS_EVEN(n) (((n)&1)==0)
#define IS_POINTER_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

நிலையான int32_t U_CALLCONV
noopGetIndex(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    திரும்பு 0;
}

நிலையான int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    திரும்பு 0;
}

நிலையான UBool U_CALLCONV
noopHasNext(UCharIterator * /*iter*/) {
    திரும்பு பொய்;
}

நிலையான UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    திரும்பு U_SENTINEL;
}

நிலையான uint32_t U_CALLCONV
noopGetState(மாறிலி UCharIterator * /*iter*/) {
    திரும்பு UITER_NO_STATE;
}

நிலையான வெறுமை U_CALLCONV
noopSetState(UCharIterator * /*iter*/, uint32_t /*state*/, UErrorCode *pErrorCode) {
    *pErrorCode=U_UNSUPPORTED_ERROR;
}

நிலையான மாறிலி UCharIterator noopIterator={
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

நிலையான int32_t U_CALLCONV
stringIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    தேர்வு(origin) {
    நிலை UITER_ZERO:
        திரும்பு 0;
    நிலை UITER_START:
        திரும்பு iter->start;
    நிலை UITER_CURRENT:
        திரும்பு iter->index;
    நிலை UITER_LIMIT:
        திரும்பு iter->limit;
    நிலை UITER_LENGTH:
        திரும்பு iter->length;
    இயல்பு:
        /* not a valid origin */
        /* Should never get here! */
        திரும்பு -1;
    }
}

நிலையான int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    தேர்வு(origin) {
    நிலை UITER_ZERO:
        pos=delta;
        நிறுத்து;
    நிலை UITER_START:
        pos=iter->start+delta;
        நிறுத்து;
    நிலை UITER_CURRENT:
        pos=iter->index+delta;
        நிறுத்து;
    நிலை UITER_LIMIT:
        pos=iter->limit+delta;
        நிறுத்து;
    நிலை UITER_LENGTH:
        pos=iter->length+delta;
        நிறுத்து;
    இயல்பு:
        திரும்பு -1;  /* Error */
    }

    எனில்(pos<iter->start) {
        pos=iter->start;
    } இல்லையெனில் எனில்(pos>iter->limit) {
        pos=iter->limit;
    }

    திரும்பு iter->index=pos;
}

நிலையான UBool U_CALLCONV
stringIteratorHasNext(UCharIterator *iter) {
    திரும்பு iter->index<iter->limit;
}

நிலையான UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    திரும்பு iter->index>iter->start;
}

நிலையான UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    எனில்(iter->index<iter->limit) {
        திரும்பு ((மாறிலி char16_t *)(iter->context))[iter->index];
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
stringIteratorNext(UCharIterator *iter) {
    எனில்(iter->index<iter->limit) {
        திரும்பு ((மாறிலி char16_t *)(iter->context))[iter->index++];
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    எனில்(iter->index>iter->start) {
        திரும்பு ((மாறிலி char16_t *)(iter->context))[--iter->index];
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான uint32_t U_CALLCONV
stringIteratorGetState(மாறிலி UCharIterator *iter) {
    திரும்பு (uint32_t)iter->index;
}

நிலையான வெறுமை U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } இல்லையெனில் எனில்(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } இல்லையெனில் எனில்((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } இல்லையெனில் {
        iter->index=(int32_t)state;
    }
}

நிலையான மாறிலி UCharIterator stringIterator={
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

U_CAPI வெறுமை U_EXPORT2
uiter_setString(UCharIterator *iter, மாறிலி char16_t *s, int32_t length) {
    எனில் (iter != nullptr) {
        எனில் (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            எனில்(length>=0) {
                iter->length=length;
            } இல்லையெனில் {
                iter->length=u_strlen(s);
            }
            iter->limit=iter->length;
        } இல்லையெனில் {
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
நிலையான உள்வரி UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t index) {
    மாறிலி uint8_t *p=(மாறிலி uint8_t *)iter->context;
    திரும்பு ((char16_t)p[2*index]<<8)|(char16_t)p[2*index+1];
}

நிலையான UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t index;

    எனில்((index=iter->index)<iter->limit) {
        திரும்பு utf16BEIteratorGet(iter, index);
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
utf16BEIteratorNext(UCharIterator *iter) {
    int32_t index;

    எனில்((index=iter->index)<iter->limit) {
        iter->index=index+1;
        திரும்பு utf16BEIteratorGet(iter, index);
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t index;

    எனில்((index=iter->index)>iter->start) {
        iter->index=--index;
        திரும்பு utf16BEIteratorGet(iter, index);
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான மாறிலி UCharIterator utf16BEIterator={
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
நிலையான int32_t
utf16BE_strlen(மாறிலி எழுத்து *s) {
    எனில்(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        திரும்பு u_strlen((மாறிலி char16_t *)s);
    } இல்லையெனில் {
        /* odd-aligned, search for pair of 0 bytes */
        மாறிலி எழுத்து *p=s;

        போது(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        திரும்பு (int32_t)((p-s)/2);
    }
}

U_CAPI வெறுமை U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, மாறிலி எழுத்து *s, int32_t length) {
    எனில்(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        எனில்(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            எனில்(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (மாறிலி char16_t *)s, length);
                திரும்பு;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            எனில்(length>=0) {
                iter->length=length;
            } இல்லையெனில் {
                iter->length=utf16BE_strlen(s);
            }
            iter->limit=iter->length;
        } இல்லையெனில் {
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

நிலையான int32_t U_CALLCONV
characterIteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    தேர்வு(origin) {
    நிலை UITER_ZERO:
        திரும்பு 0;
    நிலை UITER_START:
        திரும்பு ((CharacterIterator *)(iter->context))->startIndex();
    நிலை UITER_CURRENT:
        திரும்பு ((CharacterIterator *)(iter->context))->getIndex();
    நிலை UITER_LIMIT:
        திரும்பு ((CharacterIterator *)(iter->context))->endIndex();
    நிலை UITER_LENGTH:
        திரும்பு ((CharacterIterator *)(iter->context))->getLength();
    இயல்பு:
        /* not a valid origin */
        /* Should never get here! */
        திரும்பு -1;
    }
}

நிலையான int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    தேர்வு(origin) {
    நிலை UITER_ZERO:
        ((CharacterIterator *)(iter->context))->setIndex(delta);
        திரும்பு ((CharacterIterator *)(iter->context))->getIndex();
    நிலை UITER_START:
    நிலை UITER_CURRENT:
    நிலை UITER_LIMIT:
        திரும்பு ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    நிலை UITER_LENGTH:
        ((CharacterIterator *)(iter->context))->setIndex(((CharacterIterator *)(iter->context))->getLength()+delta);
        திரும்பு ((CharacterIterator *)(iter->context))->getIndex();
    இயல்பு:
        /* not a valid origin */
        /* Should never get here! */
        திரும்பு -1;
    }
}

நிலையான UBool U_CALLCONV
characterIteratorHasNext(UCharIterator *iter) {
    திரும்பு ((CharacterIterator *)(iter->context))->hasNext();
}

நிலையான UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    திரும்பு ((CharacterIterator *)(iter->context))->hasPrevious();
}

நிலையான UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    எனில்(c!=0xffff || ((CharacterIterator *)(iter->context))->hasNext()) {
        திரும்பு c;
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
characterIteratorNext(UCharIterator *iter) {
    எனில்(((CharacterIterator *)(iter->context))->hasNext()) {
        திரும்பு ((CharacterIterator *)(iter->context))->nextPostInc();
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    எனில்(((CharacterIterator *)(iter->context))->hasPrevious()) {
        திரும்பு ((CharacterIterator *)(iter->context))->previous();
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான uint32_t U_CALLCONV
characterIteratorGetState(மாறிலி UCharIterator *iter) {
    திரும்பு ((CharacterIterator *)(iter->context))->getIndex();
}

நிலையான வெறுமை U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } இல்லையெனில் எனில்(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } இல்லையெனில் எனில்((int32_t)state<((CharacterIterator *)(iter->context))->startIndex() || ((CharacterIterator *)(iter->context))->endIndex()<(int32_t)state) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    } இல்லையெனில் {
        ((CharacterIterator *)(iter->context))->setIndex((int32_t)state);
    }
}

நிலையான மாறிலி UCharIterator characterIteratorWrapper={
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

U_CAPI வெறுமை U_EXPORT2
uiter_setCharacterIterator(UCharIterator *iter, CharacterIterator *charIter) {
    எனில் (iter != nullptr) {
        எனில் (charIter != nullptr) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } இல்லையெனில் {
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

நிலையான UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    எனில்(iter->index<iter->limit) {
        திரும்பு ((Replaceable *)(iter->context))->charAt(iter->index);
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
replaceableIteratorNext(UCharIterator *iter) {
    எனில்(iter->index<iter->limit) {
        திரும்பு ((Replaceable *)(iter->context))->charAt(iter->index++);
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    எனில்(iter->index>iter->start) {
        திரும்பு ((Replaceable *)(iter->context))->charAt(--iter->index);
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான மாறிலி UCharIterator replaceableIterator={
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

U_CAPI வெறுமை U_EXPORT2
uiter_setReplaceable(UCharIterator *iter, மாறிலி Replaceable *rep) {
    எனில் (iter != nullptr) {
        எனில் (rep != nullptr) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->limit=iter->length=rep->length();
        } இல்லையெனில் {
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

நிலையான int32_t U_CALLCONV
utf8IteratorGetIndex(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    தேர்வு(origin) {
    நிலை UITER_ZERO:
    நிலை UITER_START:
        திரும்பு 0;
    நிலை UITER_CURRENT:
        எனில்(iter->index<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            மாறிலி uint8_t *s;
            UChar32 c;
            int32_t i, limit, index;

            s=(மாறிலி uint8_t *)iter->context;
            i=index=0;
            limit=iter->start; /* count up to the UTF-8 index */
            போது(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                index+=U16_LENGTH(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            எனில்(i==iter->limit) {
                iter->length=index; /* in case it was <0 or wrong */
            }
            எனில்(iter->reservedField!=0) {
                --index; /* we are in the middle of a supplementary code point */
            }
            iter->index=index;
        }
        திரும்பு iter->index;
    நிலை UITER_LIMIT:
    நிலை UITER_LENGTH:
        எனில்(iter->length<0) {
            மாறிலி uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(மாறிலி uint8_t *)iter->context;
            எனில்(iter->index<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                போது(i<limit) {
                    U8_NEXT_OR_FFFD(s, i, limit, c);
                    length+=U16_LENGTH(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->index= iter->reservedField!=0 ? length-1 : length;
            } இல்லையெனில் {
                i=iter->start;
                length=iter->index;
                எனில்(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            போது(i<limit) {
                U8_NEXT_OR_FFFD(s, i, limit, c);
                length+=U16_LENGTH(c);
            }
            iter->length=length;
        }
        திரும்பு iter->length;
    இயல்பு:
        /* not a valid origin */
        /* Should never get here! */
        திரும்பு -1;
    }
}

நிலையான int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    மாறிலி uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    தேர்வு(origin) {
    நிலை UITER_ZERO:
    நிலை UITER_START:
        pos=delta;
        havePos=உண்மை;
        /* iter->index<0 (unknown) is possible */
        நிறுத்து;
    நிலை UITER_CURRENT:
        எனில்(iter->index>=0) {
            pos=iter->index+delta;
            havePos=உண்மை;
        } இல்லையெனில் {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=பொய்;
        }
        நிறுத்து;
    நிலை UITER_LIMIT:
    நிலை UITER_LENGTH:
        எனில்(iter->length>=0) {
            pos=iter->length+delta;
            havePos=உண்மை;
        } இல்லையெனில் {
            /* pin to the end, avoid counting the length */
            iter->index=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            எனில்(delta>=0) {
                திரும்பு UITER_UNKNOWN_INDEX;
            } இல்லையெனில் {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=பொய்;
            }
        }
        நிறுத்து;
    இயல்பு:
        திரும்பு -1;  /* Error */
    }

    எனில்(havePos) {
        /* shortcuts: pinning to the edges of the string */
        எனில்(pos<=0) {
            iter->index=iter->start=iter->reservedField=0;
            திரும்பு 0;
        } இல்லையெனில் எனில்(iter->length>=0 && pos>=iter->length) {
            iter->index=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            திரும்பு iter->index;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        எனில்(iter->index<0 || pos<iter->index/2) {
            /* go forward from the start instead of backward from the current index */
            iter->index=iter->start=iter->reservedField=0;
        } இல்லையெனில் எனில்(iter->length>=0 && (iter->length-pos)<(pos-iter->index)) {
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
        எனில்(delta==0) {
            திரும்பு iter->index; /* nothing to do */
        }
    } இல்லையெனில் {
        /* move relative to unknown UTF-16 index */
        எனில்(delta==0) {
            திரும்பு UITER_UNKNOWN_INDEX; /* nothing to do */
        } இல்லையெனில் எனில்(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->index=iter->start=iter->reservedField=0;
            திரும்பு 0;
        } இல்லையெனில் எனில்(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->index=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            திரும்பு iter->index>=0 ? iter->index : (int32_t)UITER_UNKNOWN_INDEX;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(மாறிலி uint8_t *)iter->context;
    pos=iter->index; /* could be <0 (unknown) */
    i=iter->start;
    எனில்(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        எனில்(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        போது(delta>0 && i<limit) {
            U8_NEXT_OR_FFFD(s, i, limit, c);
            எனில்(c<=0xffff) {
                ++pos;
                --delta;
            } இல்லையெனில் எனில்(delta>=2) {
                pos+=2;
                delta-=2;
            } இல்லையெனில் /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                நிறுத்து; /* delta=0; */
            }
        }
        எனில்(i==limit) {
            எனில்(iter->length<0 && iter->index>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } இல்லையெனில் எனில்(iter->index<0 && iter->length>=0) {
                iter->index= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } இல்லையெனில் /* delta<0 */ {
        /* go backward */
        எனில்(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        போது(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            எனில்(c<=0xffff) {
                --pos;
                ++delta;
            } இல்லையெனில் எனில்(delta<=-2) {
                pos-=2;
                delta+=2;
            } இல்லையெனில் /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                நிறுத்து; /* delta=0; */
            }
        }
    }

    iter->start=i;
    எனில்(iter->index>=0) {
        திரும்பு iter->index=pos;
    } இல்லையெனில் {
        /* we started with index<0 (unknown) so pos is bogus */
        எனில்(i<=1) {
            திரும்பு iter->index=i; /* reached the beginning */
        } இல்லையெனில் {
            /* we still don't know the UTF-16 index */
            திரும்பு UITER_UNKNOWN_INDEX;
        }
    }
}

நிலையான UBool U_CALLCONV
utf8IteratorHasNext(UCharIterator *iter) {
    திரும்பு iter->start<iter->limit || iter->reservedField!=0;
}

நிலையான UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    திரும்பு iter->start>0;
}

நிலையான UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    எனில்(iter->reservedField!=0) {
        திரும்பு U16_TRAIL(iter->reservedField);
    } இல்லையெனில் எனில்(iter->start<iter->limit) {
        மாறிலி uint8_t *s=(மாறிலி uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_NEXT_OR_FFFD(s, i, iter->limit, c);
        எனில்(c<=0xffff) {
            திரும்பு c;
        } இல்லையெனில் {
            திரும்பு U16_LEAD(c);
        }
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
utf8IteratorNext(UCharIterator *iter) {
    int32_t index;

    எனில்(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        எனில்((index=iter->index)>=0) {
            iter->index=index+1;
        }
        திரும்பு trail;
    } இல்லையெனில் எனில்(iter->start<iter->limit) {
        மாறிலி uint8_t *s=(மாறிலி uint8_t *)iter->context;
        UChar32 c;

        U8_NEXT_OR_FFFD(s, iter->start, iter->limit, c);
        எனில்((index=iter->index)>=0) {
            iter->index=++index;
            எனில்(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? index : index+1;
            }
        } இல்லையெனில் எனில்(iter->start==iter->limit && iter->length>=0) {
            iter->index= c<=0xffff ? iter->length : iter->length-1;
        }
        எனில்(c<=0xffff) {
            திரும்பு c;
        } இல்லையெனில் {
            iter->reservedField=c;
            திரும்பு U16_LEAD(c);
        }
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t index;

    எனில்(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        எனில்((index=iter->index)>0) {
            iter->index=index-1;
        }
        திரும்பு lead;
    } இல்லையெனில் எனில்(iter->start>0) {
        மாறிலி uint8_t *s=(மாறிலி uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->start, c);
        எனில்((index=iter->index)>0) {
            iter->index=index-1;
        } இல்லையெனில் எனில்(iter->start<=1) {
            iter->index= c<=0xffff ? iter->start : iter->start+1;
        }
        எனில்(c<=0xffff) {
            திரும்பு c;
        } இல்லையெனில் {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            திரும்பு U16_TRAIL(c);
        }
    } இல்லையெனில் {
        திரும்பு U_SENTINEL;
    }
}

நிலையான uint32_t U_CALLCONV
utf8IteratorGetState(மாறிலி UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    எனில்(iter->reservedField!=0) {
        state|=1;
    }
    திரும்பு state;
}

நிலையான வெறுமை U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } இல்லையெனில் எனில்(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } இல்லையெனில் எனில்(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } இல்லையெனில் {
        int32_t index=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        எனில்((state==0 ? index<0 : index<4) || iter->limit<index) {
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        } இல்லையெனில் {
            iter->start=index; /* restore UTF-8 byte index */
            எனில்(index<=1) {
                iter->index=index;
            } இல்லையெனில் {
                iter->index=-1; /* unknown UTF-16 index */
            }
            எனில்(state==0) {
                iter->reservedField=0;
            } இல்லையெனில் {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((மாறிலி uint8_t *)iter->context, 0, index, c);
                எனில்(c<=0xffff) {
                    *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                } இல்லையெனில் {
                    iter->reservedField=c;
                }
            }
        }
    }
}

நிலையான மாறிலி UCharIterator utf8Iterator={
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

U_CAPI வெறுமை U_EXPORT2
uiter_setUTF8(UCharIterator *iter, மாறிலி எழுத்து *s, int32_t length) {
    எனில் (iter != nullptr) {
        எனில் (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            எனில்(length>=0) {
                iter->limit=length;
            } இல்லையெனில் {
                iter->limit=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->limit<=1 ? iter->limit : -1;
        } இல்லையெனில் {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    எனில்(U16_IS_SURROGATE(c)) {
        எனில்(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            எனில்(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } இல்லையெனில் {
            எனில்(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            எனில்(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    திரும்பு c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    எனில்(U16_IS_LEAD(c)) {
        எனில்(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } இல்லையெனில் எனில்(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    திரும்பு c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    எனில்(U16_IS_TRAIL(c)) {
        எனில்(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } இல்லையெனில் எனில்(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    திரும்பு c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(மாறிலி UCharIterator *iter) {
    எனில்(iter==nullptr || iter->getState==nullptr) {
        திரும்பு UITER_NO_STATE;
    } இல்லையெனில் {
        திரும்பு iter->getState(iter);
    }
}

U_CAPI வெறுமை U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } இல்லையெனில் எனில்(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } இல்லையெனில் எனில்(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } இல்லையெனில் {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
