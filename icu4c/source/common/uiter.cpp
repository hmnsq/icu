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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/chariter.h"
#অন্তর্ভাব "unicode/rep.h"
#অন্তর্ভাব "unicode/uiter.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cstring.h"

U_NAMESPACE_USE

#define IS_EVEN(n) (((n)&1)==0)
#define IS_POINTER_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

স্থির int32_t U_CALLCONV
noop_Get_সূচক(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    ফেরত 0;
}

স্থির int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    ফেরত 0;
}

স্থির UBool U_CALLCONV
noop_Has_পরবর্তী(UCharIterator * /*iter*/) {
    ফেরত মিথ্যা;
}

স্থির UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    ফেরত U_SENTINEL;
}

স্থির uint32_t U_CALLCONV
noopGetState(ধ্রুবক UCharIterator * /*iter*/) {
    ফেরত UITER_NO_STATE;
}

স্থির শূন্য U_CALLCONV
noopSetState(UCharIterator * /*iter*/, uint32_t /*state*/, UErrorCode *pErrorCode) {
    *pErrorCode=U_UNSUPPORTED_ERROR;
}

স্থির ধ্রুবক UCharIterator noopIterator={
    nullptr, 0, 0, 0, 0, 0,
    noop_Get_সূচক,
    noopMove,
    noop_Has_পরবর্তী,
    noop_Has_পরবর্তী,
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

স্থির int32_t U_CALLCONV
string_Iterator_Get_সূচক(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    নির্বাচন(origin) {
    ক্ষেত্র UITER_ZERO:
        ফেরত 0;
    ক্ষেত্র UITER_START:
        ফেরত iter->start;
    ক্ষেত্র UITER_CURRENT:
        ফেরত iter->সূচক;
    ক্ষেত্র UITER_LIMIT:
        ফেরত iter->limit;
    ক্ষেত্র UITER_দৈর্ঘ্য:
        ফেরত iter->length;
    পূর্বনির্ধারিত:
        /* not a valid origin */
        /* Should never get here! */
        ফেরত -1;
    }
}

স্থির int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    নির্বাচন(origin) {
    ক্ষেত্র UITER_ZERO:
        pos=delta;
        বিরতি;
    ক্ষেত্র UITER_START:
        pos=iter->start+delta;
        বিরতি;
    ক্ষেত্র UITER_CURRENT:
        pos=iter->সূচক+delta;
        বিরতি;
    ক্ষেত্র UITER_LIMIT:
        pos=iter->limit+delta;
        বিরতি;
    ক্ষেত্র UITER_দৈর্ঘ্য:
        pos=iter->length+delta;
        বিরতি;
    পূর্বনির্ধারিত:
        ফেরত -1;  /* Error */
    }

    যদি(pos<iter->start) {
        pos=iter->start;
    } অন্যথা যদি(pos>iter->limit) {
        pos=iter->limit;
    }

    ফেরত iter->সূচক=pos;
}

স্থির UBool U_CALLCONV
string_Iterator_Has_পরবর্তী(UCharIterator *iter) {
    ফেরত iter->সূচক<iter->limit;
}

স্থির UBool U_CALLCONV
stringIteratorHasPrevious(UCharIterator *iter) {
    ফেরত iter->সূচক>iter->start;
}

স্থির UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    যদি(iter->সূচক<iter->limit) {
        ফেরত ((ধ্রুবক char16_t *)(iter->context))[iter->সূচক];
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
string_Iterator_পরবর্তী(UCharIterator *iter) {
    যদি(iter->সূচক<iter->limit) {
        ফেরত ((ধ্রুবক char16_t *)(iter->context))[iter->সূচক++];
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
stringIteratorPrevious(UCharIterator *iter) {
    যদি(iter->সূচক>iter->start) {
        ফেরত ((ধ্রুবক char16_t *)(iter->context))[--iter->সূচক];
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির uint32_t U_CALLCONV
stringIteratorGetState(ধ্রুবক UCharIterator *iter) {
    ফেরত (uint32_t)iter->সূচক;
}

স্থির শূন্য U_CALLCONV
stringIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } অন্যথা যদি(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } অন্যথা যদি((int32_t)state<iter->start || iter->limit<(int32_t)state) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
    } অন্যথা {
        iter->সূচক=(int32_t)state;
    }
}

স্থির ধ্রুবক UCharIterator stringIterator={
    nullptr, 0, 0, 0, 0, 0,
    string_Iterator_Get_সূচক,
    stringIteratorMove,
    string_Iterator_Has_পরবর্তী,
    stringIteratorHasPrevious,
    stringIteratorCurrent,
    string_Iterator_পরবর্তী,
    stringIteratorPrevious,
    nullptr,
    stringIteratorGetState,
    stringIteratorSetState
};

U_CAPI শূন্য U_EXPORT2
uiter_setString(UCharIterator *iter, ধ্রুবক char16_t *s, int32_t length) {
    যদি (iter != nullptr) {
        যদি (s != nullptr && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            যদি(length>=0) {
                iter->length=length;
            } অন্যথা {
                iter->length=u_strlen(s);
            }
            iter->limit=iter->length;
        } অন্যথা {
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
স্থির অন্তঃরেখ UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t সূচক) {
    ধ্রুবক uint8_t *p=(ধ্রুবক uint8_t *)iter->context;
    ফেরত ((char16_t)p[2*সূচক]<<8)|(char16_t)p[2*সূচক+1];
}

স্থির UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t সূচক;

    যদি((সূচক=iter->সূচক)<iter->limit) {
        ফেরত utf16BEIteratorGet(iter, সূচক);
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
utf16_BEIterator_পরবর্তী(UCharIterator *iter) {
    int32_t সূচক;

    যদি((সূচক=iter->সূচক)<iter->limit) {
        iter->সূচক=সূচক+1;
        ফেরত utf16BEIteratorGet(iter, সূচক);
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
utf16BEIteratorPrevious(UCharIterator *iter) {
    int32_t সূচক;

    যদি((সূচক=iter->সূচক)>iter->start) {
        iter->সূচক=--সূচক;
        ফেরত utf16BEIteratorGet(iter, সূচক);
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির ধ্রুবক UCharIterator utf16BEIterator={
    nullptr, 0, 0, 0, 0, 0,
    string_Iterator_Get_সূচক,
    stringIteratorMove,
    string_Iterator_Has_পরবর্তী,
    stringIteratorHasPrevious,
    utf16BEIteratorCurrent,
    utf16_BEIterator_পরবর্তী,
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
স্থির int32_t
utf16BE_strlen(ধ্রুবক অক্ষর *s) {
    যদি(IS_POINTER_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        ফেরত u_strlen((ধ্রুবক char16_t *)s);
    } অন্যথা {
        /* odd-aligned, search for pair of 0 bytes */
        ধ্রুবক অক্ষর *p=s;

        যখন(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        ফেরত (int32_t)((p-s)/2);
    }
}

U_CAPI শূন্য U_EXPORT2
uiter_setUTF16BE(UCharIterator *iter, ধ্রুবক অক্ষর *s, int32_t length) {
    যদি(iter!=nullptr) {
        /* allow only even-length strings (the input length counts bytes) */
        যদি(s!=nullptr && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            যদি(U_IS_BIG_ENDIAN && IS_POINTER_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_setString(iter, (ধ্রুবক char16_t *)s, length);
                ফেরত;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            যদি(length>=0) {
                iter->length=length;
            } অন্যথা {
                iter->length=utf16BE_strlen(s);
            }
            iter->limit=iter->length;
        } অন্যথা {
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

স্থির int32_t U_CALLCONV
character_Iterator_Get_সূচক(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    নির্বাচন(origin) {
    ক্ষেত্র UITER_ZERO:
        ফেরত 0;
    ক্ষেত্র UITER_START:
        ফেরত ((CharacterIterator *)(iter->context))->start_সূচক();
    ক্ষেত্র UITER_CURRENT:
        ফেরত ((CharacterIterator *)(iter->context))->get_সূচক();
    ক্ষেত্র UITER_LIMIT:
        ফেরত ((CharacterIterator *)(iter->context))->end_সূচক();
    ক্ষেত্র UITER_দৈর্ঘ্য:
        ফেরত ((CharacterIterator *)(iter->context))->get_দৈর্ঘ্য();
    পূর্বনির্ধারিত:
        /* not a valid origin */
        /* Should never get here! */
        ফেরত -1;
    }
}

স্থির int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    নির্বাচন(origin) {
    ক্ষেত্র UITER_ZERO:
        ((CharacterIterator *)(iter->context))->set_সূচক(delta);
        ফেরত ((CharacterIterator *)(iter->context))->get_সূচক();
    ক্ষেত্র UITER_START:
    ক্ষেত্র UITER_CURRENT:
    ক্ষেত্র UITER_LIMIT:
        ফেরত ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    ক্ষেত্র UITER_দৈর্ঘ্য:
        ((CharacterIterator *)(iter->context))->set_সূচক(((CharacterIterator *)(iter->context))->get_দৈর্ঘ্য()+delta);
        ফেরত ((CharacterIterator *)(iter->context))->get_সূচক();
    পূর্বনির্ধারিত:
        /* not a valid origin */
        /* Should never get here! */
        ফেরত -1;
    }
}

স্থির UBool U_CALLCONV
character_Iterator_Has_পরবর্তী(UCharIterator *iter) {
    ফেরত ((CharacterIterator *)(iter->context))->has_পরবর্তী();
}

স্থির UBool U_CALLCONV
characterIteratorHasPrevious(UCharIterator *iter) {
    ফেরত ((CharacterIterator *)(iter->context))->hasPrevious();
}

স্থির UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    যদি(c!=0xffff || ((CharacterIterator *)(iter->context))->has_পরবর্তী()) {
        ফেরত c;
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
character_Iterator_পরবর্তী(UCharIterator *iter) {
    যদি(((CharacterIterator *)(iter->context))->has_পরবর্তী()) {
        ফেরত ((CharacterIterator *)(iter->context))->পরবর্তী_Post_Inc();
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
characterIteratorPrevious(UCharIterator *iter) {
    যদি(((CharacterIterator *)(iter->context))->hasPrevious()) {
        ফেরত ((CharacterIterator *)(iter->context))->previous();
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির uint32_t U_CALLCONV
characterIteratorGetState(ধ্রুবক UCharIterator *iter) {
    ফেরত ((CharacterIterator *)(iter->context))->get_সূচক();
}

স্থির শূন্য U_CALLCONV
characterIteratorSetState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } অন্যথা যদি(iter==nullptr || iter->context==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } অন্যথা যদি((int32_t)state<((CharacterIterator *)(iter->context))->start_সূচক() || ((CharacterIterator *)(iter->context))->end_সূচক()<(int32_t)state) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
    } অন্যথা {
        ((CharacterIterator *)(iter->context))->set_সূচক((int32_t)state);
    }
}

স্থির ধ্রুবক UCharIterator characterIteratorWrapper={
    nullptr, 0, 0, 0, 0, 0,
    character_Iterator_Get_সূচক,
    characterIteratorMove,
    character_Iterator_Has_পরবর্তী,
    characterIteratorHasPrevious,
    characterIteratorCurrent,
    character_Iterator_পরবর্তী,
    characterIteratorPrevious,
    nullptr,
    characterIteratorGetState,
    characterIteratorSetState
};

U_CAPI শূন্য U_EXPORT2
uiter_setCharacterIterator(UCharIterator *iter, CharacterIterator *charIter) {
    যদি (iter != nullptr) {
        যদি (charIter != nullptr) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } অন্যথা {
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

স্থির UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    যদি(iter->সূচক<iter->limit) {
        ফেরত ((Replaceable *)(iter->context))->charAt(iter->সূচক);
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
replaceable_Iterator_পরবর্তী(UCharIterator *iter) {
    যদি(iter->সূচক<iter->limit) {
        ফেরত ((Replaceable *)(iter->context))->charAt(iter->সূচক++);
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
replaceableIteratorPrevious(UCharIterator *iter) {
    যদি(iter->সূচক>iter->start) {
        ফেরত ((Replaceable *)(iter->context))->charAt(--iter->সূচক);
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির ধ্রুবক UCharIterator replaceableIterator={
    nullptr, 0, 0, 0, 0, 0,
    string_Iterator_Get_সূচক,
    stringIteratorMove,
    string_Iterator_Has_পরবর্তী,
    stringIteratorHasPrevious,
    replaceableIteratorCurrent,
    replaceable_Iterator_পরবর্তী,
    replaceableIteratorPrevious,
    nullptr,
    stringIteratorGetState,
    stringIteratorSetState
};

U_CAPI শূন্য U_EXPORT2
uiter_setReplaceable(UCharIterator *iter, ধ্রুবক Replaceable *rep) {
    যদি (iter != nullptr) {
        যদি (rep != nullptr) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->limit=iter->length=rep->length();
        } অন্যথা {
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

স্থির int32_t U_CALLCONV
utf8_Iterator_Get_সূচক(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    নির্বাচন(origin) {
    ক্ষেত্র UITER_ZERO:
    ক্ষেত্র UITER_START:
        ফেরত 0;
    ক্ষেত্র UITER_CURRENT:
        যদি(iter->সূচক<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ধ্রুবক uint8_t *s;
            UChar32 c;
            int32_t i, limit, সূচক;

            s=(ধ্রুবক uint8_t *)iter->context;
            i=সূচক=0;
            limit=iter->start; /* count up to the UTF-8 index */
            যখন(i<limit) {
                U8_পরবর্তী_OR_FFFD(s, i, limit, c);
                সূচক+=U16_দৈর্ঘ্য(c);
            }

            iter->start=i; /* just in case setState() did not get us to a code point boundary */
            যদি(i==iter->limit) {
                iter->length=সূচক; /* in case it was <0 or wrong */
            }
            যদি(iter->reservedField!=0) {
                --সূচক; /* we are in the middle of a supplementary code point */
            }
            iter->সূচক=সূচক;
        }
        ফেরত iter->সূচক;
    ক্ষেত্র UITER_LIMIT:
    ক্ষেত্র UITER_দৈর্ঘ্য:
        যদি(iter->length<0) {
            ধ্রুবক uint8_t *s;
            UChar32 c;
            int32_t i, limit, length;

            s=(ধ্রুবক uint8_t *)iter->context;
            যদি(iter->সূচক<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                limit=iter->start;

                /* count from the beginning to the current index */
                যখন(i<limit) {
                    U8_পরবর্তী_OR_FFFD(s, i, limit, c);
                    length+=U16_দৈর্ঘ্য(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->start=i; /* just in case setState() did not get us to a code point boundary */
                iter->সূচক= iter->reservedField!=0 ? length-1 : length;
            } অন্যথা {
                i=iter->start;
                length=iter->সূচক;
                যদি(iter->reservedField!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            limit=iter->limit;
            যখন(i<limit) {
                U8_পরবর্তী_OR_FFFD(s, i, limit, c);
                length+=U16_দৈর্ঘ্য(c);
            }
            iter->length=length;
        }
        ফেরত iter->length;
    পূর্বনির্ধারিত:
        /* not a valid origin */
        /* Should never get here! */
        ফেরত -1;
    }
}

স্থির int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ধ্রুবক uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    নির্বাচন(origin) {
    ক্ষেত্র UITER_ZERO:
    ক্ষেত্র UITER_START:
        pos=delta;
        havePos=সত্য;
        /* iter->index<0 (unknown) is possible */
        বিরতি;
    ক্ষেত্র UITER_CURRENT:
        যদি(iter->সূচক>=0) {
            pos=iter->সূচক+delta;
            havePos=সত্য;
        } অন্যথা {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=মিথ্যা;
        }
        বিরতি;
    ক্ষেত্র UITER_LIMIT:
    ক্ষেত্র UITER_দৈর্ঘ্য:
        যদি(iter->length>=0) {
            pos=iter->length+delta;
            havePos=সত্য;
        } অন্যথা {
            /* pin to the end, avoid counting the length */
            iter->সূচক=-1;
            iter->start=iter->limit;
            iter->reservedField=0;
            যদি(delta>=0) {
                ফেরত UITER_UNKNOWN_সূচক;
            } অন্যথা {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=মিথ্যা;
            }
        }
        বিরতি;
    পূর্বনির্ধারিত:
        ফেরত -1;  /* Error */
    }

    যদি(havePos) {
        /* shortcuts: pinning to the edges of the string */
        যদি(pos<=0) {
            iter->সূচক=iter->start=iter->reservedField=0;
            ফেরত 0;
        } অন্যথা যদি(iter->length>=0 && pos>=iter->length) {
            iter->সূচক=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
            ফেরত iter->সূচক;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        যদি(iter->সূচক<0 || pos<iter->সূচক/2) {
            /* go forward from the start instead of backward from the current index */
            iter->সূচক=iter->start=iter->reservedField=0;
        } অন্যথা যদি(iter->length>=0 && (iter->length-pos)<(pos-iter->সূচক)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->সূচক=iter->length;
            iter->start=iter->limit;
            iter->reservedField=0;
        }

        delta=pos-iter->সূচক;
        যদি(delta==0) {
            ফেরত iter->সূচক; /* nothing to do */
        }
    } অন্যথা {
        /* move relative to unknown UTF-16 index */
        যদি(delta==0) {
            ফেরত UITER_UNKNOWN_সূচক; /* nothing to do */
        } অন্যথা যদি(-delta>=iter->start) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->সূচক=iter->start=iter->reservedField=0;
            ফেরত 0;
        } অন্যথা যদি(delta>=(iter->limit-iter->start)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->সূচক=iter->length; /* may or may not be <0 (unknown) */
            iter->start=iter->limit;
            iter->reservedField=0;
            ফেরত iter->সূচক>=0 ? iter->সূচক : (int32_t)UITER_UNKNOWN_সূচক;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ধ্রুবক uint8_t *)iter->context;
    pos=iter->সূচক; /* could be <0 (unknown) */
    i=iter->start;
    যদি(delta>0) {
        /* go forward */
        int32_t limit=iter->limit;
        যদি(iter->reservedField!=0) {
            iter->reservedField=0;
            ++pos;
            --delta;
        }
        যখন(delta>0 && i<limit) {
            U8_পরবর্তী_OR_FFFD(s, i, limit, c);
            যদি(c<=0xffff) {
                ++pos;
                --delta;
            } অন্যথা যদি(delta>=2) {
                pos+=2;
                delta-=2;
            } অন্যথা /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reservedField=c;
                ++pos;
                বিরতি; /* delta=0; */
            }
        }
        যদি(i==limit) {
            যদি(iter->length<0 && iter->সূচক>=0) {
                iter->length= iter->reservedField==0 ? pos : pos+1;
            } অন্যথা যদি(iter->সূচক<0 && iter->length>=0) {
                iter->সূচক= iter->reservedField==0 ? iter->length : iter->length-1;
            }
        }
    } অন্যথা /* delta<0 */ {
        /* go backward */
        যদি(iter->reservedField!=0) {
            iter->reservedField=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        যখন(delta<0 && i>0) {
            U8_পূর্ব_OR_FFFD(s, 0, i, c);
            যদি(c<=0xffff) {
                --pos;
                ++delta;
            } অন্যথা যদি(delta<=-2) {
                pos-=2;
                delta+=2;
            } অন্যথা /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reservedField=c;
                --pos;
                বিরতি; /* delta=0; */
            }
        }
    }

    iter->start=i;
    যদি(iter->সূচক>=0) {
        ফেরত iter->সূচক=pos;
    } অন্যথা {
        /* we started with index<0 (unknown) so pos is bogus */
        যদি(i<=1) {
            ফেরত iter->সূচক=i; /* reached the beginning */
        } অন্যথা {
            /* we still don't know the UTF-16 index */
            ফেরত UITER_UNKNOWN_সূচক;
        }
    }
}

স্থির UBool U_CALLCONV
utf8_Iterator_Has_পরবর্তী(UCharIterator *iter) {
    ফেরত iter->start<iter->limit || iter->reservedField!=0;
}

স্থির UBool U_CALLCONV
utf8IteratorHasPrevious(UCharIterator *iter) {
    ফেরত iter->start>0;
}

স্থির UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    যদি(iter->reservedField!=0) {
        ফেরত U16_TRAIL(iter->reservedField);
    } অন্যথা যদি(iter->start<iter->limit) {
        ধ্রুবক uint8_t *s=(ধ্রুবক uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->start;

        U8_পরবর্তী_OR_FFFD(s, i, iter->limit, c);
        যদি(c<=0xffff) {
            ফেরত c;
        } অন্যথা {
            ফেরত U16_LEAD(c);
        }
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
utf8_Iterator_পরবর্তী(UCharIterator *iter) {
    int32_t সূচক;

    যদি(iter->reservedField!=0) {
        char16_t trail=U16_TRAIL(iter->reservedField);
        iter->reservedField=0;
        যদি((সূচক=iter->সূচক)>=0) {
            iter->সূচক=সূচক+1;
        }
        ফেরত trail;
    } অন্যথা যদি(iter->start<iter->limit) {
        ধ্রুবক uint8_t *s=(ধ্রুবক uint8_t *)iter->context;
        UChar32 c;

        U8_পরবর্তী_OR_FFFD(s, iter->start, iter->limit, c);
        যদি((সূচক=iter->সূচক)>=0) {
            iter->সূচক=++সূচক;
            যদি(iter->length<0 && iter->start==iter->limit) {
                iter->length= c<=0xffff ? সূচক : সূচক+1;
            }
        } অন্যথা যদি(iter->start==iter->limit && iter->length>=0) {
            iter->সূচক= c<=0xffff ? iter->length : iter->length-1;
        }
        যদি(c<=0xffff) {
            ফেরত c;
        } অন্যথা {
            iter->reservedField=c;
            ফেরত U16_LEAD(c);
        }
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির UChar32 U_CALLCONV
utf8IteratorPrevious(UCharIterator *iter) {
    int32_t সূচক;

    যদি(iter->reservedField!=0) {
        char16_t lead=U16_LEAD(iter->reservedField);
        iter->reservedField=0;
        iter->start-=4; /* we stayed behind the supplementary code point; go before it now */
        যদি((সূচক=iter->সূচক)>0) {
            iter->সূচক=সূচক-1;
        }
        ফেরত lead;
    } অন্যথা যদি(iter->start>0) {
        ধ্রুবক uint8_t *s=(ধ্রুবক uint8_t *)iter->context;
        UChar32 c;

        U8_পূর্ব_OR_FFFD(s, 0, iter->start, c);
        যদি((সূচক=iter->সূচক)>0) {
            iter->সূচক=সূচক-1;
        } অন্যথা যদি(iter->start<=1) {
            iter->সূচক= c<=0xffff ? iter->start : iter->start+1;
        }
        যদি(c<=0xffff) {
            ফেরত c;
        } অন্যথা {
            iter->start+=4; /* back to behind this supplementary code point for consistent state */
            iter->reservedField=c;
            ফেরত U16_TRAIL(c);
        }
    } অন্যথা {
        ফেরত U_SENTINEL;
    }
}

স্থির uint32_t U_CALLCONV
utf8IteratorGetState(ধ্রুবক UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->start<<1);
    যদি(iter->reservedField!=0) {
        state|=1;
    }
    ফেরত state;
}

স্থির শূন্য U_CALLCONV
utf8IteratorSetState(UCharIterator *iter,
                     uint32_t state,
                     UErrorCode *pErrorCode)
{
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } অন্যথা যদি(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } অন্যথা যদি(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } অন্যথা {
        int32_t সূচক=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        যদি((state==0 ? সূচক<0 : সূচক<4) || iter->limit<সূচক) {
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        } অন্যথা {
            iter->start=সূচক; /* restore UTF-8 byte index */
            যদি(সূচক<=1) {
                iter->সূচক=সূচক;
            } অন্যথা {
                iter->সূচক=-1; /* unknown UTF-16 index */
            }
            যদি(state==0) {
                iter->reservedField=0;
            } অন্যথা {
                /* verified index>=4 above */
                UChar32 c;
                U8_পূর্ব_OR_FFFD((ধ্রুবক uint8_t *)iter->context, 0, সূচক, c);
                যদি(c<=0xffff) {
                    *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                } অন্যথা {
                    iter->reservedField=c;
                }
            }
        }
    }
}

স্থির ধ্রুবক UCharIterator utf8Iterator={
    nullptr, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_সূচক,
    utf8IteratorMove,
    utf8_Iterator_Has_পরবর্তী,
    utf8IteratorHasPrevious,
    utf8IteratorCurrent,
    utf8_Iterator_পরবর্তী,
    utf8IteratorPrevious,
    nullptr,
    utf8IteratorGetState,
    utf8IteratorSetState
};

U_CAPI শূন্য U_EXPORT2
uiter_setUTF8(UCharIterator *iter, ধ্রুবক অক্ষর *s, int32_t length) {
    যদি (iter != nullptr) {
        যদি (s != nullptr && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            যদি(length>=0) {
                iter->limit=length;
            } অন্যথা {
                iter->limit=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->limit<=1 ? iter->limit : -1;
        } অন্যথা {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    যদি(U16_IS_SURROGATE(c)) {
        যদি(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            যদি(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } অন্যথা {
            যদি(U16_IS_LEAD(c2=iter->previous(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            যদি(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    ফেরত c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    যদি(U16_IS_LEAD(c)) {
        যদি(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } অন্যথা যদি(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    ফেরত c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->previous(iter);
    যদি(U16_IS_TRAIL(c)) {
        যদি(U16_IS_LEAD(c2=iter->previous(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } অন্যথা যদি(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    ফেরত c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ধ্রুবক UCharIterator *iter) {
    যদি(iter==nullptr || iter->getState==nullptr) {
        ফেরত UITER_NO_STATE;
    } অন্যথা {
        ফেরত iter->getState(iter);
    }
}

U_CAPI শূন্য U_EXPORT2
uiter_setState(UCharIterator *iter, uint32_t state, UErrorCode *pErrorCode) {
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        /* do nothing */
    } অন্যথা যদি(iter==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } অন্যথা যদি(iter->setState==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
    } অন্যথা {
        iter->setState(iter, state, pErrorCode);
    }
}

U_CDECL_END
