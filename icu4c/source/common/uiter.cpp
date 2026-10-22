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

#समावेश "unicode/utypes.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/chariter.h"
#समावेश "unicode/rep.h"
#समावेश "unicode/uiter.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "cstring.h"

U_NAMESPACE_USE

#घोषणा IS_EVEN(n) (((n)&1)==0)
#घोषणा IS_POINTER_EVEN(p) IS_EVEN((माप_प्रकार)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

अटल int32_t U_CALLCONV
noop_Get_सूचकांक(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    वापस 0;
}

अटल int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    वापस 0;
}

अटल UBool U_CALLCONV
noop_Has_अग्र(UCharIterator * /*iter*/) {
    वापस false;
}

अटल UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    वापस U_SENTINEL;
}

अटल uint32_t U_CALLCONV
noopGetState(स्थिर UCharIterator * /*iter*/) {
    वापस UITER_NO_STATE;
}

अटल व्योम U_CALLCONV
noopSetState(UCharIterator * /*iter*/, uint32_t /*state*/, UErrorCode *pErrorCode) {
    *pErrorCode=U_UNSUPPORTED_ERROR;
}

अटल स्थिर UCharIterator noopIterator={
    nullptr, 0, 0, 0, 0, 0,
    noop_Get_सूचकांक,
    noopMove,
    noop_Has_अग्र,
    noop_Has_अग्र,
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

अटल int32_t U_CALLCONV
string_Iterator_Get_सूचकांक(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    चयन(origin) {
    हाल UITER_ZERO:
        वापस 0;
    हाल UITER_START:
        वापस iter->start;
    हाल UITER_CURRENT:
        वापस iter->सूचकांक;
    हाल UITER_LIMIT:
        वापस iter->limit;
    हाल UITER_लंबाई:
        वापस iter->length;
    शेष:
        /* not a valid origin */
        /* Should never get here! */
        वापस -1;
    }
}

अटल int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    चयन(origin) {
    हाल UITER_ZERO:
        pos=delta;
        अवरोध;
    हाल UITER_START:
        pos=iter->start+delta;
        अवरोध;
    हाल UITER_CURRENT:
        pos=iter->सूचकांक+delta;
        अवरोध;
    हाल UITER_LIMIT:
        pos=iter->limit+delta;
        अवरोध;
    हाल UITER_लंबाई:
        pos=iter->length+delta;
        अवरोध;
    शेष:
        वापस -1;  /* Error */
    }

    अगर(pos<iter->start) {
        pos=iter->start;
    } अन्यथा अगर(pos>iter->limit) {
        pos=iter->limit;
    }

    वापस iter->सूचकांक=pos;
}

अटल UBool U_CALLCONV
string_Iterator_Has_अग्र(UCharIterator *iter) {
    वापस iter->सूचकांक<iter->limit;
}

अटल UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    वापस iter->सूचकांक>iter->start;
}

अटल UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    अगर(iter->सूचकांक<iter->limit) {
        वापस ((स्थिर char16_t *)(iter->context))[iter->सूचकांक];
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
string_Iterator_अग्र(UCharIterator *iter) {
    अगर(iter->सूचकांक<iter->limit) {
        वापस ((स्थिर char16_t *)(iter->context))[iter->सूचकांक++];
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    अगर(iter->सूचकांक>iter->start) {
        वापस ((स्थिर char16_t *)(iter->context))[--iter->सूचकांक];
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल uint32_t U_CALLCONV
stringIteratorGetState(स्थिर UCharIterator *iter) {
    वापस (uint32_t)iter->सूचकांक;
}

अटल व्योम U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } अन्यथा अगर(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा अगर((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
    } अन्यथा {
        iter->सूचकांक=(int32_t)state;
    }
}

अटल स्थिर UCharIterator stringIterator={
    nullptr, 0, 0, 0, 0, 0,
    string_Iterator_Get_सूचकांक,
    stringIteratorMove,
    string_Iterator_Has_अग्र,
    stringIteratorHasPrevious,
    stringIteratorCurrent,
    string_Iterator_अग्र,
    stringIteratorPrevious,
    nullptr,
    stringIteratorGetState,
    stringIteratorSetState
};

U_CAPI व्योम U_EXPORT2
uiter_setString(UCharIterator *iter, स्थिर char16_t *s, int32_t length) {
    अगर (iter != nullptr) {
        अगर (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            अगर(length>=0) {
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
अटल अंतरभूत UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t सूचकांक) {
    स्थिर uint8_t *p=(स्थिर uint8_t *)iter->context;
    वापस ((char16_t)p[2*सूचकांक]<<8)|(char16_t)p[2*सूचकांक+1];
}

अटल UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t सूचकांक;

    अगर((सूचकांक=iter->सूचकांक)<iter->limit) {
        वापस utf16BEIteratorGet(iter, सूचकांक);
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
utf16_BEIterator_अग्र(UCharIterator *iter) {
    int32_t सूचकांक;

    अगर((सूचकांक=iter->सूचकांक)<iter->limit) {
        iter->सूचकांक=सूचकांक+1;
        वापस utf16BEIteratorGet(iter, सूचकांक);
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t सूचकांक;

    अगर((सूचकांक=iter->सूचकांक)>iter->start) {
        iter->सूचकांक=--सूचकांक;
        वापस utf16BEIteratorGet(iter, सूचकांक);
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल स्थिर UCharIterator utf16BEIterator={
    nullptr, 0, 0, 0, 0, 0,
    string_Iterator_Get_सूचकांक,
    stringIteratorMove,
    string_Iterator_Has_अग्र,
    stringIteratorHasPrevious,
    utf16BEIteratorCurrent,
    utf16_BEIterator_अग्र,
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
अटल int32_t
utf16BE_strlen(स्थिर अक्षर *s) {
    अगर(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        वापस u_strlen((स्थिर char16_t *)s);
    } अन्यथा {
        /* odd-aligned, search for pair of 0 bytes */
        स्थिर अक्षर *p=s;

        जबतक(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        वापस (int32_t)((p-s)/2);
    }
}

U_CAPI व्योम U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, स्थिर अक्षर *s, int32_t length) {
    अगर(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        अगर(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            अगर(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (स्थिर char16_t *)s, length);
                वापस;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            अगर(length>=0) {
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

अटल int32_t U_CALLCONV
character_Iterator_Get_सूचकांक(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    चयन(origin) {
    हाल UITER_ZERO:
        वापस 0;
    हाल UITER_START:
        वापस ((CharacterIterator *)(iter->context))->start_सूचकांक();
    हाल UITER_CURRENT:
        वापस ((CharacterIterator *)(iter->context))->get_सूचकांक();
    हाल UITER_LIMIT:
        वापस ((CharacterIterator *)(iter->context))->end_सूचकांक();
    हाल UITER_लंबाई:
        वापस ((CharacterIterator *)(iter->context))->get_लंबाई();
    शेष:
        /* not a valid origin */
        /* Should never get here! */
        वापस -1;
    }
}

अटल int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    चयन(origin) {
    हाल UITER_ZERO:
        ((CharacterIterator *)(iter->context))->set_सूचकांक(delta);
        वापस ((CharacterIterator *)(iter->context))->get_सूचकांक();
    हाल UITER_START:
    हाल UITER_CURRENT:
    हाल UITER_LIMIT:
        वापस ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    हाल UITER_लंबाई:
        ((CharacterIterator *)(iter->context))->set_सूचकांक(((CharacterIterator *)(iter->context))->get_लंबाई()+delta);
        वापस ((CharacterIterator *)(iter->context))->get_सूचकांक();
    शेष:
        /* not a valid origin */
        /* Should never get here! */
        वापस -1;
    }
}

अटल UBool U_CALLCONV
character_Iterator_Has_अग्र(UCharIterator *iter) {
    वापस ((CharacterIterator *)(iter->context))->has_अग्र();
}

अटल UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    वापस ((CharacterIterator *)(iter->context))->hasPrevious();
}

अटल UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    अगर(c!=0xffff || ((CharacterIterator *)(iter->context))->has_अग्र()) {
        वापस c;
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
character_Iterator_अग्र(UCharIterator *iter) {
    अगर(((CharacterIterator *)(iter->context))->has_अग्र()) {
        वापस ((CharacterIterator *)(iter->context))->अग्र_Post_Inc();
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    अगर(((CharacterIterator *)(iter->context))->hasPrevious()) {
        वापस ((CharacterIterator *)(iter->context))->previous();
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल uint32_t U_CALLCONV
characterIteratorGetState(स्थिर UCharIterator *iter) {
    वापस ((CharacterIterator *)(iter->context))->get_सूचकांक();
}

अटल व्योम U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } अन्यथा अगर(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा अगर((int32_t)state<((CharacterIterator *)(iter->context))->start_सूचकांक() || ((CharacterIterator *)(iter->context))->end_सूचकांक()<(int32_t)state) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
    } अन्यथा {
        ((CharacterIterator *)(iter->context))->set_सूचकांक((int32_t)state);
    }
}

अटल स्थिर UCharIterator characterIteratorWrapper={
    nullptr, 0, 0, 0, 0, 0,
    character_Iterator_Get_सूचकांक,
    characterIteratorMove,
    character_Iterator_Has_अग्र,
    characterIteratorHasPrevious,
    characterIteratorCurrent,
    character_Iterator_अग्र,
    characterIteratorPrevious,
    nullptr,
    characterIteratorGetState,
    characterIteratorSetState
};

U_CAPI व्योम U_EXPORT2
uiter_setCharacterIterator(UCharIterator *iter, CharacterIterator *charIter) {
    अगर (iter != nullptr) {
        अगर (charIter != nullptr) {
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

अटल UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    अगर(iter->सूचकांक<iter->limit) {
        वापस ((Replaceable *)(iter->context))->charAt(iter->सूचकांक);
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
replaceable_Iterator_अग्र(UCharIterator *iter) {
    अगर(iter->सूचकांक<iter->limit) {
        वापस ((Replaceable *)(iter->context))->charAt(iter->सूचकांक++);
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    अगर(iter->सूचकांक>iter->start) {
        वापस ((Replaceable *)(iter->context))->charAt(--iter->सूचकांक);
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल स्थिर UCharIterator replaceableIterator={
    nullptr, 0, 0, 0, 0, 0,
    string_Iterator_Get_सूचकांक,
    stringIteratorMove,
    string_Iterator_Has_अग्र,
    stringIteratorHasPrevious,
    replaceableIteratorCurrent,
    replaceable_Iterator_अग्र,
    replaceableIteratorPrevious,
    nullptr,
    stringIteratorGetState,
    stringIteratorSetState
};

U_CAPI व्योम U_EXPORT2
uiter_setReplaceable(UCharIterator *iter, स्थिर Replaceable *rep) {
    अगर (iter != nullptr) {
        अगर (rep != nullptr) {
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

#घोषणा UITER_CNV_CAPACITY 16

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

अटल int32_t U_CALLCONV
utf8_Iterator_Get_सूचकांक(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    चयन(origin) {
    हाल UITER_ZERO:
    हाल UITER_START:
        वापस 0;
    हाल UITER_CURRENT:
        अगर(iter->सूचकांक<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            स्थिर uint8_t *s;
            UChar32 c;
            int32_t i, limit, सूचकांक;

            s=(स्थिर uint8_t *)iter->context;
            i=सूचकांक=0;
            limit=iter->start; /* count up to the UTF-8 index */
            जबतक(i<limit) {
                U8_अग्र_OR_FFFD(s, i, limit, c);
                सूचकांक+=U16_लंबाई(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            अगर(i==iter->limit) {
                iter->length=सूचकांक; /* in case it was <0 or wrong */
            }
            अगर(iter->reservedField!=0) {
                --सूचकांक; /* we are in the middle of a supplementary code point */
            }
            iter->सूचकांक=सूचकांक;
        }
        वापस iter->सूचकांक;
    हाल UITER_LIMIT:
    हाल UITER_लंबाई:
        अगर(iter->length<0) {
            स्थिर uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(स्थिर uint8_t *)iter->context;
            अगर(iter->सूचकांक<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                जबतक(i<limit) {
                    U8_अग्र_OR_FFFD(s, i, limit, c);
                    length+=U16_लंबाई(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->सूचकांक= iter->reservedField!=0 ? length-1 : length;
            } अन्यथा {
                i=iter->start;
                length=iter->सूचकांक;
                अगर(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            जबतक(i<limit) {
                U8_अग्र_OR_FFFD(s, i, limit, c);
                length+=U16_लंबाई(c);
            }
            iter->length=length;
        }
        वापस iter->length;
    शेष:
        /* not a valid origin */
        /* Should never get here! */
        वापस -1;
    }
}

अटल int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    स्थिर uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    चयन(origin) {
    हाल UITER_ZERO:
    हाल UITER_START:
        pos=delta;
        havePos=true;
        /* iter->index<0 (unknown) is possible */
        अवरोध;
    हाल UITER_CURRENT:
        अगर(iter->सूचकांक>=0) {
            pos=iter->सूचकांक+delta;
            havePos=true;
        } अन्यथा {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=false;
        }
        अवरोध;
    हाल UITER_LIMIT:
    हाल UITER_लंबाई:
        अगर(iter->length>=0) {
            pos=iter->length+delta;
            havePos=true;
        } अन्यथा {
            /* pin to the end, avoid counting the length */
            iter->सूचकांक=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            अगर(delta>=0) {
                वापस UITER_UNKNOWN_सूचकांक;
            } अन्यथा {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=false;
            }
        }
        अवरोध;
    शेष:
        वापस -1;  /* Error */
    }

    अगर(havePos) {
        /* shortcuts: pinning to the edges of the string */
        अगर(pos<=0) {
            iter->सूचकांक=iter->start=iter->reservedField=0;
            वापस 0;
        } अन्यथा अगर(iter->length>=0 && pos>=iter->length) {
            iter->सूचकांक=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            वापस iter->सूचकांक;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        अगर(iter->सूचकांक<0 || pos<iter->सूचकांक/2) {
            /* go forward from the start instead of backward from the current index */
            iter->सूचकांक=iter->start=iter->reservedField=0;
        } अन्यथा अगर(iter->length>=0 && (iter->length-pos)<(pos-iter->सूचकांक)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->सूचकांक=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
        }

        delta=pos-iter->सूचकांक;
        अगर(delta==0) {
            वापस iter->सूचकांक; /* nothing to do */
        }
    } अन्यथा {
        /* move relative to unknown UTF-16 index */
        अगर(delta==0) {
            वापस UITER_UNKNOWN_सूचकांक; /* nothing to do */
        } अन्यथा अगर(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->सूचकांक=iter->start=iter->reservedField=0;
            वापस 0;
        } अन्यथा अगर(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->सूचकांक=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            वापस iter->सूचकांक>=0 ? iter->सूचकांक : (int32_t)UITER_UNKNOWN_सूचकांक;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(स्थिर uint8_t *)iter->context;
    pos=iter->सूचकांक; /* could be <0 (unknown) */
    i=iter->start;
    अगर(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        अगर(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        जबतक(delta>0 && i<limit) {
            U8_अग्र_OR_FFFD(s, i, limit, c);
            अगर(c<=0xffff) {
                ++pos;
                --delta;
            } अन्यथा अगर(delta>=2) {
                pos+=2;
                delta-=2;
            } अन्यथा /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                अवरोध; /* delta=0; */
            }
        }
        अगर(i==limit) {
            अगर(iter->length<0 && iter->सूचकांक>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } अन्यथा अगर(iter->सूचकांक<0 && iter->length>=0) {
                iter->सूचकांक= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } अन्यथा /* delta<0 */ {
        /* go backward */
        अगर(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        जबतक(delta<0 && i>0) {
            U8_पूर्व_OR_FFFD(s, 0, i, c);
            अगर(c<=0xffff) {
                --pos;
                ++delta;
            } अन्यथा अगर(delta<=-2) {
                pos-=2;
                delta+=2;
            } अन्यथा /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                अवरोध; /* delta=0; */
            }
        }
    }

    iter->start=i;
    अगर(iter->सूचकांक>=0) {
        वापस iter->सूचकांक=pos;
    } अन्यथा {
        /* we started with index<0 (unknown) so pos is bogus */
        अगर(i<=1) {
            वापस iter->सूचकांक=i; /* reached the beginning */
        } अन्यथा {
            /* we still don't know the UTF-16 index */
            वापस UITER_UNKNOWN_सूचकांक;
        }
    }
}

अटल UBool U_CALLCONV
utf8_Iterator_Has_अग्र(UCharIterator *iter) {
    वापस iter->start<iter->limit || iter->reservedField!=0;
}

अटल UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    वापस iter->start>0;
}

अटल UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    अगर(iter->reservedField!=0) {
        वापस U16_TRAIL(iter->reservedField);
    } अन्यथा अगर(iter->start<iter->limit) {
        स्थिर uint8_t *s=(स्थिर uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_अग्र_OR_FFFD(s, i, iter->limit, c);
        अगर(c<=0xffff) {
            वापस c;
        } अन्यथा {
            वापस U16_LEAD(c);
        }
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
utf8_Iterator_अग्र(UCharIterator *iter) {
    int32_t सूचकांक;

    अगर(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        अगर((सूचकांक=iter->सूचकांक)>=0) {
            iter->सूचकांक=सूचकांक+1;
        }
        वापस trail;
    } अन्यथा अगर(iter->start<iter->limit) {
        स्थिर uint8_t *s=(स्थिर uint8_t *)iter->context;
        UChar32 c;

        U8_अग्र_OR_FFFD(s, iter->start, iter->limit, c);
        अगर((सूचकांक=iter->सूचकांक)>=0) {
            iter->सूचकांक=++सूचकांक;
            अगर(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? सूचकांक : सूचकांक+1;
            }
        } अन्यथा अगर(iter->start==iter->limit && iter->length>=0) {
            iter->सूचकांक= c<=0xffff ? iter->length : iter->length-1;
        }
        अगर(c<=0xffff) {
            वापस c;
        } अन्यथा {
            iter->reservedField=c;
            वापस U16_LEAD(c);
        }
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t सूचकांक;

    अगर(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        अगर((सूचकांक=iter->सूचकांक)>0) {
            iter->सूचकांक=सूचकांक-1;
        }
        वापस lead;
    } अन्यथा अगर(iter->start>0) {
        स्थिर uint8_t *s=(स्थिर uint8_t *)iter->context;
        UChar32 c;

        U8_पूर्व_OR_FFFD(s, 0, iter->start, c);
        अगर((सूचकांक=iter->सूचकांक)>0) {
            iter->सूचकांक=सूचकांक-1;
        } अन्यथा अगर(iter->start<=1) {
            iter->सूचकांक= c<=0xffff ? iter->start : iter->start+1;
        }
        अगर(c<=0xffff) {
            वापस c;
        } अन्यथा {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            वापस U16_TRAIL(c);
        }
    } अन्यथा {
        वापस U_SENTINEL;
    }
}

अटल uint32_t U_CALLCONV
utf8IteratorGetState(स्थिर UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    अगर(iter->reservedField!=0) {
        state|=1;
    }
    वापस state;
}

अटल व्योम U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } अन्यथा अगर(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा अगर(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } अन्यथा {
        int32_t सूचकांक=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        अगर((state==0 ? सूचकांक<0 : सूचकांक<4) || iter->limit<सूचकांक) {
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        } अन्यथा {
            iter->start=सूचकांक; /* restore UTF-8 byte index */
            अगर(सूचकांक<=1) {
                iter->सूचकांक=सूचकांक;
            } अन्यथा {
                iter->सूचकांक=-1; /* unknown UTF-16 index */
            }
            अगर(state==0) {
                iter->reservedField=0;
            } अन्यथा {
                /* verified index>=4 above */
                UChar32 c;
                U8_पूर्व_OR_FFFD((स्थिर uint8_t *)iter->context, 0, सूचकांक, c);
                अगर(c<=0xffff) {
                    *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                } अन्यथा {
                    iter->reservedField=c;
                }
            }
        }
    }
}

अटल स्थिर UCharIterator utf8Iterator={
    nullptr, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_सूचकांक,
    utf8IteratorMove,
    utf8_Iterator_Has_अग्र,
    utf8IteratorHasPrevious,
    utf8IteratorCurrent,
    utf8_Iterator_अग्र,
    utf8IteratorPrevious,
    nullptr,
    utf8IteratorGetState,
    utf8IteratorSetState
};

U_CAPI व्योम U_EXPORT2
uiter_setUTF8(UCharIterator *iter, स्थिर अक्षर *s, int32_t length) {
    अगर (iter != nullptr) {
        अगर (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            अगर(length>=0) {
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
    अगर(U16_IS_SURROGATE(c)) {
        अगर(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            अगर(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } अन्यथा {
            अगर(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            अगर(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    वापस c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    अगर(U16_IS_LEAD(c)) {
        अगर(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } अन्यथा अगर(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    वापस c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    अगर(U16_IS_TRAIL(c)) {
        अगर(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } अन्यथा अगर(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    वापस c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(स्थिर UCharIterator *iter) {
    अगर(iter==nullptr || iter->getState==nullptr) {
        वापस UITER_NO_STATE;
    } अन्यथा {
        वापस iter->getState(iter);
    }
}

U_CAPI व्योम U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } अन्यथा अगर(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा अगर(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } अन्यथा {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
