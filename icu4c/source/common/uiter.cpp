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

#परिभाषय IS_EVEN(n) (((n)&1)==0)
#परिभाषय IS_POINTER_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

स्थिर int32_t U_CALLCONV
noop_Get_सूचकांक(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    प्रत्यागम 0;
}

स्थिर int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    प्रत्यागम 0;
}

स्थिर UBool U_CALLCONV
noop_Has_अग्र(UCharIterator * /*iter*/) {
    प्रत्यागम असत्य;
}

स्थिर UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    प्रत्यागम U_SENTINEL;
}

स्थिर uint32_t U_CALLCONV
noopGetState(ध्रुव UCharIterator * /*iter*/) {
    प्रत्यागम UITER_NO_STATE;
}

स्थिर शून्य U_CALLCONV
noop_समुच्चयः_State(UCharIterator * /*iter*/, uint32_t /*state*/, UError_संहिता *p_Error_संहिता) {
    *p_Error_संहिता=U_UNSUPPORTED_ERROR;
}

स्थिर ध्रुव UCharIterator noopIterator={
    शून्यसूचकः, 0, 0, 0, 0, 0,
    noop_Get_सूचकांक,
    noopMove,
    noop_Has_अग्र,
    noop_Has_अग्र,
    noopCurrent,
    noopCurrent,
    noopCurrent,
    शून्यसूचकः,
    noopGetState,
    noop_समुच्चयः_State
};

/* UCharIterator implementation for simple strings -------------------------- */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * for char16_t * strings.
 *
 * The UCharIterator.context field holds a pointer to the string.
 */

स्थिर int32_t U_CALLCONV
string_Iterator_Get_सूचकांक(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    विकल्प(origin) {
    पक्ष UITER_ZERO:
        प्रत्यागम 0;
    पक्ष UITER_START:
        प्रत्यागम iter->start;
    पक्ष UITER_CURRENT:
        प्रत्यागम iter->सूचकांक;
    पक्ष UITER_सीमा:
        प्रत्यागम iter->सीमा;
    पक्ष UITER_दीर्घता:
        प्रत्यागम iter->length;
    स्वभाव:
        /* not a valid origin */
        /* Should never get here! */
        प्रत्यागम -1;
    }
}

स्थिर int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    विकल्प(origin) {
    पक्ष UITER_ZERO:
        pos=delta;
        भङ्ग;
    पक्ष UITER_START:
        pos=iter->start+delta;
        भङ्ग;
    पक्ष UITER_CURRENT:
        pos=iter->सूचकांक+delta;
        भङ्ग;
    पक्ष UITER_सीमा:
        pos=iter->सीमा+delta;
        भङ्ग;
    पक्ष UITER_दीर्घता:
        pos=iter->length+delta;
        भङ्ग;
    स्वभाव:
        प्रत्यागम -1;  /* Error */
    }

    यदि(pos<iter->start) {
        pos=iter->start;
    } अन्यथा यदि(pos>iter->सीमा) {
        pos=iter->सीमा;
    }

    प्रत्यागम iter->सूचकांक=pos;
}

स्थिर UBool U_CALLCONV
string_Iterator_Has_अग्र(UCharIterator *iter) {
    प्रत्यागम iter->सूचकांक<iter->सीमा;
}

स्थिर UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    प्रत्यागम iter->सूचकांक>iter->start;
}

स्थिर UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    यदि(iter->सूचकांक<iter->सीमा) {
        प्रत्यागम ((ध्रुव char16_t *)(iter->context))[iter->सूचकांक];
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
string_Iterator_अग्र(UCharIterator *iter) {
    यदि(iter->सूचकांक<iter->सीमा) {
        प्रत्यागम ((ध्रुव char16_t *)(iter->context))[iter->सूचकांक++];
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    यदि(iter->सूचकांक>iter->start) {
        प्रत्यागम ((ध्रुव char16_t *)(iter->context))[--iter->सूचकांक];
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर uint32_t U_CALLCONV
stringIteratorGetState(ध्रुव UCharIterator *iter) {
    प्रत्यागम (uint32_t)iter->सूचकांक;
}

स्थिर शून्य U_CALLCONV
string_Iterator_समुच्चयः_State(UCharIterator *iter, uint32_t state, UError_संहिता *p_Error_संहिता) {
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        /* do nothing */
    } अन्यथा यदि(iter==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    } अन्यथा यदि((int32_t)state<iter->start || iter->सीमा<(int32_t)state) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
    } अन्यथा {
        iter->सूचकांक=(int32_t)state;
    }
}

स्थिर ध्रुव UCharIterator stringIterator={
    शून्यसूचकः, 0, 0, 0, 0, 0,
    string_Iterator_Get_सूचकांक,
    stringIteratorMove,
    string_Iterator_Has_अग्र,
    stringIteratorHasPrevious,
    stringIteratorCurrent,
    string_Iterator_अग्र,
    stringIteratorPrevious,
    शून्यसूचकः,
    stringIteratorGetState,
    string_Iterator_समुच्चयः_State
};

U_CAPI शून्य U_EXPORT2
uiter_समुच्चयः_String(UCharIterator *iter, ध्रुव char16_t *s, int32_t length) {
    यदि (iter != शून्यसूचकः) {
        यदि (s != शून्यसूचकः && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            यदि(length>=0) {
                iter->length=length;
            } अन्यथा {
                iter->length=u_strlen(s);
            }
            iter->सीमा=iter->length;
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
utf16BEIteratorGet(UCharIterator *iter, int32_t सूचकांक) {
    ध्रुव uint8_t *p=(ध्रुव uint8_t *)iter->context;
    प्रत्यागम ((char16_t)p[2*सूचकांक]<<8)|(char16_t)p[2*सूचकांक+1];
}

स्थिर UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t सूचकांक;

    यदि((सूचकांक=iter->सूचकांक)<iter->सीमा) {
        प्रत्यागम utf16BEIteratorGet(iter, सूचकांक);
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
utf16_BEIterator_अग्र(UCharIterator *iter) {
    int32_t सूचकांक;

    यदि((सूचकांक=iter->सूचकांक)<iter->सीमा) {
        iter->सूचकांक=सूचकांक+1;
        प्रत्यागम utf16BEIteratorGet(iter, सूचकांक);
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t सूचकांक;

    यदि((सूचकांक=iter->सूचकांक)>iter->start) {
        iter->सूचकांक=--सूचकांक;
        प्रत्यागम utf16BEIteratorGet(iter, सूचकांक);
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर ध्रुव UCharIterator utf16BEIterator={
    शून्यसूचकः, 0, 0, 0, 0, 0,
    string_Iterator_Get_सूचकांक,
    stringIteratorMove,
    string_Iterator_Has_अग्र,
    stringIteratorHasPrevious,
    utf16BEIteratorCurrent,
    utf16_BEIterator_अग्र,
    utf16BEIteratorPrevious,
    शून्यसूचकः,
    stringIteratorGetState,
    string_Iterator_समुच्चयः_State
};

/*
 * Count the number of UChars in a UTF-16BE string before a terminating char16_t NUL,
 * i.e., before a pair of 0 bytes where the first 0 byte is at an even
 * offset from s.
 */
स्थिर int32_t
utf16BE_strlen(ध्रुव वर्ण *s) {
    यदि(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        प्रत्यागम u_strlen((ध्रुव char16_t *)s);
    } अन्यथा {
        /* odd-aligned, search for pair of 0 bytes */
        ध्रुव वर्ण *p=s;

        यावत्(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        प्रत्यागम (int32_t)((p-s)/2);
    }
}

U_CAPI शून्य U_EXPORT2
uiter_समुच्चयः_UTF16_BE(UCharIterator *iter, ध्रुव वर्ण *s, int32_t length) {
    यदि(iter!=शून्यसूचकः) {
        /* allow only even-length strings (the input length counts bytes) */
        यदि(s!=शून्यसूचकः && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            यदि(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_समुच्चयः_String(iter, (ध्रुव char16_t *)s, length);
                प्रत्यागम;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            यदि(length>=0) {
                iter->length=length;
            } अन्यथा {
                iter->length=utf16BE_strlen(s);
            }
            iter->सीमा=iter->length;
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
character_Iterator_Get_सूचकांक(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    विकल्प(origin) {
    पक्ष UITER_ZERO:
        प्रत्यागम 0;
    पक्ष UITER_START:
        प्रत्यागम ((CharacterIterator *)(iter->context))->start_सूचकांक();
    पक्ष UITER_CURRENT:
        प्रत्यागम ((CharacterIterator *)(iter->context))->get_सूचकांक();
    पक्ष UITER_सीमा:
        प्रत्यागम ((CharacterIterator *)(iter->context))->अन्तः_सूचकांक();
    पक्ष UITER_दीर्घता:
        प्रत्यागम ((CharacterIterator *)(iter->context))->get_दीर्घता();
    स्वभाव:
        /* not a valid origin */
        /* Should never get here! */
        प्रत्यागम -1;
    }
}

स्थिर int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    विकल्प(origin) {
    पक्ष UITER_ZERO:
        ((CharacterIterator *)(iter->context))->समुच्चयः_सूचकांक(delta);
        प्रत्यागम ((CharacterIterator *)(iter->context))->get_सूचकांक();
    पक्ष UITER_START:
    पक्ष UITER_CURRENT:
    पक्ष UITER_सीमा:
        प्रत्यागम ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    पक्ष UITER_दीर्घता:
        ((CharacterIterator *)(iter->context))->समुच्चयः_सूचकांक(((CharacterIterator *)(iter->context))->get_दीर्घता()+delta);
        प्रत्यागम ((CharacterIterator *)(iter->context))->get_सूचकांक();
    स्वभाव:
        /* not a valid origin */
        /* Should never get here! */
        प्रत्यागम -1;
    }
}

स्थिर UBool U_CALLCONV
character_Iterator_Has_अग्र(UCharIterator *iter) {
    प्रत्यागम ((CharacterIterator *)(iter->context))->has_अग्र();
}

स्थिर UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    प्रत्यागम ((CharacterIterator *)(iter->context))->hasPrevious();
}

स्थिर UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    यदि(c!=0xffff || ((CharacterIterator *)(iter->context))->has_अग्र()) {
        प्रत्यागम c;
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
character_Iterator_अग्र(UCharIterator *iter) {
    यदि(((CharacterIterator *)(iter->context))->has_अग्र()) {
        प्रत्यागम ((CharacterIterator *)(iter->context))->अग्र_Post_Inc();
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    यदि(((CharacterIterator *)(iter->context))->hasPrevious()) {
        प्रत्यागम ((CharacterIterator *)(iter->context))->previous();
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर uint32_t U_CALLCONV
characterIteratorGetState(ध्रुव UCharIterator *iter) {
    प्रत्यागम ((CharacterIterator *)(iter->context))->get_सूचकांक();
}

स्थिर शून्य U_CALLCONV
character_Iterator_समुच्चयः_State(UCharIterator *iter, uint32_t state, UError_संहिता *p_Error_संहिता) {
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        /* do nothing */
    } अन्यथा यदि(iter==शून्यसूचकः || iter->context==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    } अन्यथा यदि((int32_t)state<((CharacterIterator *)(iter->context))->start_सूचकांक() || ((CharacterIterator *)(iter->context))->अन्तः_सूचकांक()<(int32_t)state) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
    } अन्यथा {
        ((CharacterIterator *)(iter->context))->समुच्चयः_सूचकांक((int32_t)state);
    }
}

स्थिर ध्रुव UCharIterator characterIteratorWrapper={
    शून्यसूचकः, 0, 0, 0, 0, 0,
    character_Iterator_Get_सूचकांक,
    characterIteratorMove,
    character_Iterator_Has_अग्र,
    characterIteratorHasPrevious,
    characterIteratorCurrent,
    character_Iterator_अग्र,
    characterIteratorPrevious,
    शून्यसूचकः,
    characterIteratorGetState,
    character_Iterator_समुच्चयः_State
};

U_CAPI शून्य U_EXPORT2
uiter_समुच्चयः_Character_Iterator(UCharIterator *iter, CharacterIterator *charIter) {
    यदि (iter != शून्यसूचकः) {
        यदि (charIter != शून्यसूचकः) {
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
    यदि(iter->सूचकांक<iter->सीमा) {
        प्रत्यागम ((Replaceable *)(iter->context))->charAt(iter->सूचकांक);
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
replaceable_Iterator_अग्र(UCharIterator *iter) {
    यदि(iter->सूचकांक<iter->सीमा) {
        प्रत्यागम ((Replaceable *)(iter->context))->charAt(iter->सूचकांक++);
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    यदि(iter->सूचकांक>iter->start) {
        प्रत्यागम ((Replaceable *)(iter->context))->charAt(--iter->सूचकांक);
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर ध्रुव UCharIterator replaceableIterator={
    शून्यसूचकः, 0, 0, 0, 0, 0,
    string_Iterator_Get_सूचकांक,
    stringIteratorMove,
    string_Iterator_Has_अग्र,
    stringIteratorHasPrevious,
    replaceableIteratorCurrent,
    replaceable_Iterator_अग्र,
    replaceableIteratorPrevious,
    शून्यसूचकः,
    stringIteratorGetState,
    string_Iterator_समुच्चयः_State
};

U_CAPI शून्य U_EXPORT2
uiter_समुच्चयः_Replaceable(UCharIterator *iter, ध्रुव Replaceable *rep) {
    यदि (iter != शून्यसूचकः) {
        यदि (rep != शून्यसूचकः) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->सीमा=iter->length=rep->length();
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

#परिभाषय UITER_CNV_क्षमता 16

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
utf8_Iterator_Get_सूचकांक(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    विकल्प(origin) {
    पक्ष UITER_ZERO:
    पक्ष UITER_START:
        प्रत्यागम 0;
    पक्ष UITER_CURRENT:
        यदि(iter->सूचकांक<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ध्रुव uint8_t *s;
            UChar32 c;
            int32_t i, सीमा, सूचकांक;

            s=(ध्रुव uint8_t *)iter->context;
            i=सूचकांक=0;
            सीमा=iter->start; /* count up to the UTF-8 index */
            यावत्(i<सीमा) {
                U8_अग्र_OR_FFFD(s, i, सीमा, c);
                सूचकांक+=U16_दीर्घता(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            यदि(i==iter->सीमा) {
                iter->length=सूचकांक; /* in case it was <0 or wrong */
            }
            यदि(iter->reserved_क्षेत्रम्!=0) {
                --सूचकांक; /* we are in the middle of a supplementary code point */
            }
            iter->सूचकांक=सूचकांक;
        }
        प्रत्यागम iter->सूचकांक;
    पक्ष UITER_सीमा:
    पक्ष UITER_दीर्घता:
        यदि(iter->length<0) {
            ध्रुव uint8_t *s;
            UChar32 c;
            int32_t i, सीमा, length;

            s=(ध्रुव uint8_t *)iter->context;
            यदि(iter->सूचकांक<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                सीमा=iter->start;

                /* count from the beginning to the current index */
                यावत्(i<सीमा) {
                    U8_अग्र_OR_FFFD(s, i, सीमा, c);
                    length+=U16_दीर्घता(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->सूचकांक= iter->reserved_क्षेत्रम्!=0 ? length-1 : length;
            } अन्यथा {
                i=iter->start;
                length=iter->सूचकांक;
                यदि(iter->reserved_क्षेत्रम्!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            सीमा=iter->सीमा;
            यावत्(i<सीमा) {
                U8_अग्र_OR_FFFD(s, i, सीमा, c);
                length+=U16_दीर्घता(c);
            }
            iter->length=length;
        }
        प्रत्यागम iter->length;
    स्वभाव:
        /* not a valid origin */
        /* Should never get here! */
        प्रत्यागम -1;
    }
}

स्थिर int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ध्रुव uint8_t *s;
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
        भङ्ग;
    पक्ष UITER_CURRENT:
        यदि(iter->सूचकांक>=0) {
            pos=iter->सूचकांक+delta;
            havePos=सत्य;
        } अन्यथा {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=असत्य;
        }
        भङ्ग;
    पक्ष UITER_सीमा:
    पक्ष UITER_दीर्घता:
        यदि(iter->length>=0) {
            pos=iter->length+delta;
            havePos=सत्य;
        } अन्यथा {
            /* pin to the end, avoid counting the length */
            iter->सूचकांक=-1;
            iter->start=iter->सीमा;
            iter->reserved_क्षेत्रम्=0;
            यदि(delta>=0) {
                प्रत्यागम UITER_UNKNOWN_सूचकांक;
            } अन्यथा {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=असत्य;
            }
        }
        भङ्ग;
    स्वभाव:
        प्रत्यागम -1;  /* Error */
    }

    यदि(havePos) {
        /* shortcuts: pinning to the edges of the string */
        यदि(pos<=0) {
            iter->सूचकांक=iter->start=iter->reserved_क्षेत्रम्=0;
            प्रत्यागम 0;
        } अन्यथा यदि(iter->length>=0 && pos>=iter->length) {
            iter->सूचकांक=iter->length;
            iter->start=iter->सीमा;
            iter->reserved_क्षेत्रम्=0;
            प्रत्यागम iter->सूचकांक;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        यदि(iter->सूचकांक<0 || pos<iter->सूचकांक/2) {
            /* go forward from the start instead of backward from the current index */
            iter->सूचकांक=iter->start=iter->reserved_क्षेत्रम्=0;
        } अन्यथा यदि(iter->length>=0 && (iter->length-pos)<(pos-iter->सूचकांक)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->सूचकांक=iter->length;
            iter->start=iter->सीमा;
            iter->reserved_क्षेत्रम्=0;
        }

        delta=pos-iter->सूचकांक;
        यदि(delta==0) {
            प्रत्यागम iter->सूचकांक; /* nothing to do */
        }
    } अन्यथा {
        /* move relative to unknown UTF-16 index */
        यदि(delta==0) {
            प्रत्यागम UITER_UNKNOWN_सूचकांक; /* nothing to do */
        } अन्यथा यदि(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->सूचकांक=iter->start=iter->reserved_क्षेत्रम्=0;
            प्रत्यागम 0;
        } अन्यथा यदि(delta>=(iter->सीमा-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->सूचकांक=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->सीमा;
            iter->reserved_क्षेत्रम्=0;
            प्रत्यागम iter->सूचकांक>=0 ? iter->सूचकांक : (int32_t)UITER_UNKNOWN_सूचकांक;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ध्रुव uint8_t *)iter->context;
    pos=iter->सूचकांक; /* could be <0 (unknown) */
    i=iter->start;
    यदि(delta>0) {
        /* go forward */
        int32_t सीमा=iter->सीमा;
        यदि(iter->reserved_क्षेत्रम्!=0) {
            iter->reserved_क्षेत्रम्=0;
            ++pos;
            --delta;
        }
        यावत्(delta>0 && i<सीमा) {
            U8_अग्र_OR_FFFD(s, i, सीमा, c);
            यदि(c<=0xffff) {
                ++pos;
                --delta;
            } अन्यथा यदि(delta>=2) {
                pos+=2;
                delta-=2;
            } अन्यथा /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reserved_क्षेत्रम्=c;
                ++pos;
                भङ्ग; /* delta=0; */
            }
        }
        यदि(i==सीमा) {
            यदि(iter->length<0 && iter->सूचकांक>=0) {
                iter->length= iter->reserved_क्षेत्रम्==0 ? pos : pos+1;
            } अन्यथा यदि(iter->सूचकांक<0 && iter->length>=0) {
                iter->सूचकांक= iter->reserved_क्षेत्रम्==0 ? iter->length : iter->length-1;
            }
        }
    } अन्यथा /* delta<0 */ {
        /* go backward */
        यदि(iter->reserved_क्षेत्रम्!=0) {
            iter->reserved_क्षेत्रम्=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        यावत्(delta<0 && i>0) {
            U8_पूर्व_OR_FFFD(s, 0, i, c);
            यदि(c<=0xffff) {
                --pos;
                ++delta;
            } अन्यथा यदि(delta<=-2) {
                pos-=2;
                delta+=2;
            } अन्यथा /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reserved_क्षेत्रम्=c;
                --pos;
                भङ्ग; /* delta=0; */
            }
        }
    }

    iter->start=i;
    यदि(iter->सूचकांक>=0) {
        प्रत्यागम iter->सूचकांक=pos;
    } अन्यथा {
        /* we started with index<0 (unknown) so pos is bogus */
        यदि(i<=1) {
            प्रत्यागम iter->सूचकांक=i; /* reached the beginning */
        } अन्यथा {
            /* we still don't know the UTF-16 index */
            प्रत्यागम UITER_UNKNOWN_सूचकांक;
        }
    }
}

स्थिर UBool U_CALLCONV
utf8_Iterator_Has_अग्र(UCharIterator *iter) {
    प्रत्यागम iter->start<iter->सीमा || iter->reserved_क्षेत्रम्!=0;
}

स्थिर UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    प्रत्यागम iter->start>0;
}

स्थिर UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    यदि(iter->reserved_क्षेत्रम्!=0) {
        प्रत्यागम U16_TRAIL(iter->reserved_क्षेत्रम्);
    } अन्यथा यदि(iter->start<iter->सीमा) {
        ध्रुव uint8_t *s=(ध्रुव uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_अग्र_OR_FFFD(s, i, iter->सीमा, c);
        यदि(c<=0xffff) {
            प्रत्यागम c;
        } अन्यथा {
            प्रत्यागम U16_LEAD(c);
        }
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
utf8_Iterator_अग्र(UCharIterator *iter) {
    int32_t सूचकांक;

    यदि(iter->reserved_क्षेत्रम्!=0) {
        char16_t trail=U16_TRAIL(iter->reserved_क्षेत्रम्);
        iter->reserved_क्षेत्रम्=0;
        यदि((सूचकांक=iter->सूचकांक)>=0) {
            iter->सूचकांक=सूचकांक+1;
        }
        प्रत्यागम trail;
    } अन्यथा यदि(iter->start<iter->सीमा) {
        ध्रुव uint8_t *s=(ध्रुव uint8_t *)iter->context;
        UChar32 c;

        U8_अग्र_OR_FFFD(s, iter->start, iter->सीमा, c);
        यदि((सूचकांक=iter->सूचकांक)>=0) {
            iter->सूचकांक=++सूचकांक;
            यदि(iter->length<0 && iter->start==iter->सीमा) {
                iter->length= c<=0xffff ? सूचकांक : सूचकांक+1;
            }
        } अन्यथा यदि(iter->start==iter->सीमा && iter->length>=0) {
            iter->सूचकांक= c<=0xffff ? iter->length : iter->length-1;
        }
        यदि(c<=0xffff) {
            प्रत्यागम c;
        } अन्यथा {
            iter->reserved_क्षेत्रम्=c;
            प्रत्यागम U16_LEAD(c);
        }
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t सूचकांक;

    यदि(iter->reserved_क्षेत्रम्!=0) {
        char16_t lead=U16_LEAD(iter->reserved_क्षेत्रम्);
        iter->reserved_क्षेत्रम्=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        यदि((सूचकांक=iter->सूचकांक)>0) {
            iter->सूचकांक=सूचकांक-1;
        }
        प्रत्यागम lead;
    } अन्यथा यदि(iter->start>0) {
        ध्रुव uint8_t *s=(ध्रुव uint8_t *)iter->context;
        UChar32 c;

        U8_पूर्व_OR_FFFD(s, 0, iter->start, c);
        यदि((सूचकांक=iter->सूचकांक)>0) {
            iter->सूचकांक=सूचकांक-1;
        } अन्यथा यदि(iter->start<=1) {
            iter->सूचकांक= c<=0xffff ? iter->start : iter->start+1;
        }
        यदि(c<=0xffff) {
            प्रत्यागम c;
        } अन्यथा {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reserved_क्षेत्रम्=c;
            प्रत्यागम U16_TRAIL(c);
        }
    } अन्यथा {
        प्रत्यागम U_SENTINEL;
    }
}

स्थिर uint32_t U_CALLCONV
utf8IteratorGetState(ध्रुव UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    यदि(iter->reserved_क्षेत्रम्!=0) {
        state|=1;
    }
    प्रत्यागम state;
}

स्थिर शून्य U_CALLCONV
utf8_Iterator_समुच्चयः_State(UCharIterator *iter,
                     uint32_t state,
                     UError_संहिता *p_Error_संहिता)
{
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        /* do nothing */
    } अन्यथा यदि(iter==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    } अन्यथा यदि(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } अन्यथा {
        int32_t सूचकांक=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        यदि((state==0 ? सूचकांक<0 : सूचकांक<4) || iter->सीमा<सूचकांक) {
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        } अन्यथा {
            iter->start=सूचकांक; /* restore UTF-8 byte index */
            यदि(सूचकांक<=1) {
                iter->सूचकांक=सूचकांक;
            } अन्यथा {
                iter->सूचकांक=-1; /* unknown UTF-16 index */
            }
            यदि(state==0) {
                iter->reserved_क्षेत्रम्=0;
            } अन्यथा {
                /* verified index>=4 above */
                UChar32 c;
                U8_पूर्व_OR_FFFD((ध्रुव uint8_t *)iter->context, 0, सूचकांक, c);
                यदि(c<=0xffff) {
                    *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                } अन्यथा {
                    iter->reserved_क्षेत्रम्=c;
                }
            }
        }
    }
}

स्थिर ध्रुव UCharIterator utf8Iterator={
    शून्यसूचकः, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_सूचकांक,
    utf8IteratorMove,
    utf8_Iterator_Has_अग्र,
    utf8IteratorHasPrevious,
    utf8IteratorCurrent,
    utf8_Iterator_अग्र,
    utf8IteratorPrevious,
    शून्यसूचकः,
    utf8IteratorGetState,
    utf8_Iterator_समुच्चयः_State
};

U_CAPI शून्य U_EXPORT2
uiter_समुच्चयः_UTF8(UCharIterator *iter, ध्रुव वर्ण *s, int32_t length) {
    यदि (iter != शून्यसूचकः) {
        यदि (s != शून्यसूचकः && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            यदि(length>=0) {
                iter->सीमा=length;
            } अन्यथा {
                iter->सीमा=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->सीमा<=1 ? iter->सीमा : -1;
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
    प्रत्यागम c;
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
    प्रत्यागम c;
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
    प्रत्यागम c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ध्रुव UCharIterator *iter) {
    यदि(iter==शून्यसूचकः || iter->getState==शून्यसूचकः) {
        प्रत्यागम UITER_NO_STATE;
    } अन्यथा {
        प्रत्यागम iter->getState(iter);
    }
}

U_CAPI शून्य U_EXPORT2
uiter_समुच्चयः_State(UCharIterator *iter, uint32_t state, UError_संहिता *p_Error_संहिता) {
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        /* do nothing */
    } अन्यथा यदि(iter==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    } अन्यथा यदि(iter->समुच्चयः_State==शून्यसूचकः) {
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
    } अन्यथा {
        iter->समुच्चयः_State(iter, state, p_Error_संहिता);
    }
}

U_CDECL_अन्तः
