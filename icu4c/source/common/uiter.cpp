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

#شامل "unicode/utypes.h"
#شامل "unicode/ustring.h"
#شامل "unicode/chariter.h"
#شامل "unicode/rep.h"
#شامل "unicode/uiter.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "cstring.h"

U_NAMESPACE_USE

#تعریف_کرو IS_EVEN(n) (((n)&1)==0)
#تعریف_کرو IS_اشارہ_گر_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

ساکن int32_t U_CALLCONV
noop_Get_اشاریہ(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    واپس 0;
}

ساکن int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    واپس 0;
}

ساکن UBool U_CALLCONV
noop_Has_اگلا(UCharIterator * /*iter*/) {
    واپس جھوٹ;
}

ساکن UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    واپس U_SENTINEL;
}

ساکن uint32_t U_CALLCONV
noopGetState(ثابت UCharIterator * /*iter*/) {
    واپس UITER_NO_STATE;
}

ساکن خالی U_CALLCONV
noop_سیٹ_State(UCharIterator * /*iter*/, uint32_t /*state*/, UError_رمز *p_Error_رمز) {
    *p_Error_رمز=U_UNSUPPORTED_ERROR;
}

ساکن ثابت UCharIterator noopIterator={
    خالی_اشاریہ, 0, 0, 0, 0, 0,
    noop_Get_اشاریہ,
    noopMove,
    noop_Has_اگلا,
    noop_Has_اگلا,
    noopCurrent,
    noopCurrent,
    noopCurrent,
    خالی_اشاریہ,
    noopGetState,
    noop_سیٹ_State
};

/* UCharIterator implementation for simple strings -------------------------- */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * for char16_t * strings.
 *
 * The UCharIterator.context field holds a pointer to the string.
 */

ساکن int32_t U_CALLCONV
string_Iterator_Get_اشاریہ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    انتخاب(origin) {
    حالت UITER_ZERO:
        واپس 0;
    حالت UITER_آغاز:
        واپس iter->آغاز;
    حالت UITER_CURRENT:
        واپس iter->اشاریہ;
    حالت UITER_حد_بندی:
        واپس iter->حد_بندی;
    حالت UITER_لمبائی:
        واپس iter->length;
    طےشدہ:
        /* not a valid origin */
        /* Should never get here! */
        واپس -1;
    }
}

ساکن int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    انتخاب(origin) {
    حالت UITER_ZERO:
        pos=delta;
        توڑو;
    حالت UITER_آغاز:
        pos=iter->آغاز+delta;
        توڑو;
    حالت UITER_CURRENT:
        pos=iter->اشاریہ+delta;
        توڑو;
    حالت UITER_حد_بندی:
        pos=iter->حد_بندی+delta;
        توڑو;
    حالت UITER_لمبائی:
        pos=iter->length+delta;
        توڑو;
    طےشدہ:
        واپس -1;  /* Error */
    }

    اگر(pos<iter->آغاز) {
        pos=iter->آغاز;
    } ورنہ اگر(pos>iter->حد_بندی) {
        pos=iter->حد_بندی;
    }

    واپس iter->اشاریہ=pos;
}

ساکن UBool U_CALLCONV
string_Iterator_Has_اگلا(UCharIterator *iter) {
    واپس iter->اشاریہ<iter->حد_بندی;
}

ساکن UBool U_CALLCONV
string_Iterator_Has_پچھلا(UCharIterator *iter) {
    واپس iter->اشاریہ>iter->آغاز;
}

ساکن UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    اگر(iter->اشاریہ<iter->حد_بندی) {
        واپس ((ثابت char16_t *)(iter->context))[iter->اشاریہ];
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
string_Iterator_اگلا(UCharIterator *iter) {
    اگر(iter->اشاریہ<iter->حد_بندی) {
        واپس ((ثابت char16_t *)(iter->context))[iter->اشاریہ++];
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
string_Iterator_پچھلا(UCharIterator *iter) {
    اگر(iter->اشاریہ>iter->آغاز) {
        واپس ((ثابت char16_t *)(iter->context))[--iter->اشاریہ];
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن uint32_t U_CALLCONV
stringIteratorGetState(ثابت UCharIterator *iter) {
    واپس (uint32_t)iter->اشاریہ;
}

ساکن خالی U_CALLCONV
string_Iterator_سیٹ_State(UCharIterator *iter, uint32_t state, UError_رمز *p_Error_رمز) {
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        /* do nothing */
    } ورنہ اگر(iter==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    } ورنہ اگر((int32_t)state<iter->آغاز || iter->حد_بندی<(int32_t)state) {
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
    } ورنہ {
        iter->اشاریہ=(int32_t)state;
    }
}

ساکن ثابت UCharIterator stringIterator={
    خالی_اشاریہ, 0, 0, 0, 0, 0,
    string_Iterator_Get_اشاریہ,
    stringIteratorMove,
    string_Iterator_Has_اگلا,
    string_Iterator_Has_پچھلا,
    stringIteratorCurrent,
    string_Iterator_اگلا,
    string_Iterator_پچھلا,
    خالی_اشاریہ,
    stringIteratorGetState,
    string_Iterator_سیٹ_State
};

U_CAPI خالی U_EXPORT2
uiter_سیٹ_String(UCharIterator *iter, ثابت char16_t *s, int32_t length) {
    اگر (iter != خالی_اشاریہ) {
        اگر (s != خالی_اشاریہ && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            اگر(length>=0) {
                iter->length=length;
            } ورنہ {
                iter->length=u_strlen(s);
            }
            iter->حد_بندی=iter->length;
        } ورنہ {
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
ساکن اندرونی UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t اشاریہ) {
    ثابت uint8_t *p=(ثابت uint8_t *)iter->context;
    واپس ((char16_t)p[2*اشاریہ]<<8)|(char16_t)p[2*اشاریہ+1];
}

ساکن UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t اشاریہ;

    اگر((اشاریہ=iter->اشاریہ)<iter->حد_بندی) {
        واپس utf16BEIteratorGet(iter, اشاریہ);
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
utf16_BEIterator_اگلا(UCharIterator *iter) {
    int32_t اشاریہ;

    اگر((اشاریہ=iter->اشاریہ)<iter->حد_بندی) {
        iter->اشاریہ=اشاریہ+1;
        واپس utf16BEIteratorGet(iter, اشاریہ);
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
utf16_BEIterator_پچھلا(UCharIterator *iter) {
    int32_t اشاریہ;

    اگر((اشاریہ=iter->اشاریہ)>iter->آغاز) {
        iter->اشاریہ=--اشاریہ;
        واپس utf16BEIteratorGet(iter, اشاریہ);
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن ثابت UCharIterator utf16BEIterator={
    خالی_اشاریہ, 0, 0, 0, 0, 0,
    string_Iterator_Get_اشاریہ,
    stringIteratorMove,
    string_Iterator_Has_اگلا,
    string_Iterator_Has_پچھلا,
    utf16BEIteratorCurrent,
    utf16_BEIterator_اگلا,
    utf16_BEIterator_پچھلا,
    خالی_اشاریہ,
    stringIteratorGetState,
    string_Iterator_سیٹ_State
};

/*
 * Count the number of UChars in a UTF-16BE string before a terminating char16_t NUL,
 * i.e., before a pair of 0 bytes where the first 0 byte is at an even
 * offset from s.
 */
ساکن int32_t
utf16BE_strlen(ثابت حرف *s) {
    اگر(IS_اشارہ_گر_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        واپس u_strlen((ثابت char16_t *)s);
    } ورنہ {
        /* odd-aligned, search for pair of 0 bytes */
        ثابت حرف *p=s;

        جب(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        واپس (int32_t)((p-s)/2);
    }
}

U_CAPI خالی U_EXPORT2
uiter_سیٹ_UTF16_BE(UCharIterator *iter, ثابت حرف *s, int32_t length) {
    اگر(iter!=خالی_اشاریہ) {
        /* allow only even-length strings (the input length counts bytes) */
        اگر(s!=خالی_اشاریہ && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            اگر(U_IS_BIG_ENDIAN && IS_اشارہ_گر_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_سیٹ_String(iter, (ثابت char16_t *)s, length);
                واپس;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            اگر(length>=0) {
                iter->length=length;
            } ورنہ {
                iter->length=utf16BE_strlen(s);
            }
            iter->حد_بندی=iter->length;
        } ورنہ {
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

ساکن int32_t U_CALLCONV
character_Iterator_Get_اشاریہ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    انتخاب(origin) {
    حالت UITER_ZERO:
        واپس 0;
    حالت UITER_آغاز:
        واپس ((CharacterIterator *)(iter->context))->آغاز_اشاریہ();
    حالت UITER_CURRENT:
        واپس ((CharacterIterator *)(iter->context))->get_اشاریہ();
    حالت UITER_حد_بندی:
        واپس ((CharacterIterator *)(iter->context))->انتہا_اشاریہ();
    حالت UITER_لمبائی:
        واپس ((CharacterIterator *)(iter->context))->get_لمبائی();
    طےشدہ:
        /* not a valid origin */
        /* Should never get here! */
        واپس -1;
    }
}

ساکن int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    انتخاب(origin) {
    حالت UITER_ZERO:
        ((CharacterIterator *)(iter->context))->سیٹ_اشاریہ(delta);
        واپس ((CharacterIterator *)(iter->context))->get_اشاریہ();
    حالت UITER_آغاز:
    حالت UITER_CURRENT:
    حالت UITER_حد_بندی:
        واپس ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    حالت UITER_لمبائی:
        ((CharacterIterator *)(iter->context))->سیٹ_اشاریہ(((CharacterIterator *)(iter->context))->get_لمبائی()+delta);
        واپس ((CharacterIterator *)(iter->context))->get_اشاریہ();
    طےشدہ:
        /* not a valid origin */
        /* Should never get here! */
        واپس -1;
    }
}

ساکن UBool U_CALLCONV
character_Iterator_Has_اگلا(UCharIterator *iter) {
    واپس ((CharacterIterator *)(iter->context))->has_اگلا();
}

ساکن UBool U_CALLCONV
character_Iterator_Has_پچھلا(UCharIterator *iter) {
    واپس ((CharacterIterator *)(iter->context))->has_پچھلا();
}

ساکن UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    اگر(c!=0xffff || ((CharacterIterator *)(iter->context))->has_اگلا()) {
        واپس c;
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
character_Iterator_اگلا(UCharIterator *iter) {
    اگر(((CharacterIterator *)(iter->context))->has_اگلا()) {
        واپس ((CharacterIterator *)(iter->context))->اگلا_Post_Inc();
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
character_Iterator_پچھلا(UCharIterator *iter) {
    اگر(((CharacterIterator *)(iter->context))->has_پچھلا()) {
        واپس ((CharacterIterator *)(iter->context))->پچھلا_2();
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن uint32_t U_CALLCONV
characterIteratorGetState(ثابت UCharIterator *iter) {
    واپس ((CharacterIterator *)(iter->context))->get_اشاریہ();
}

ساکن خالی U_CALLCONV
character_Iterator_سیٹ_State(UCharIterator *iter, uint32_t state, UError_رمز *p_Error_رمز) {
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        /* do nothing */
    } ورنہ اگر(iter==خالی_اشاریہ || iter->context==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    } ورنہ اگر((int32_t)state<((CharacterIterator *)(iter->context))->آغاز_اشاریہ() || ((CharacterIterator *)(iter->context))->انتہا_اشاریہ()<(int32_t)state) {
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
    } ورنہ {
        ((CharacterIterator *)(iter->context))->سیٹ_اشاریہ((int32_t)state);
    }
}

ساکن ثابت UCharIterator characterIteratorWrapper={
    خالی_اشاریہ, 0, 0, 0, 0, 0,
    character_Iterator_Get_اشاریہ,
    characterIteratorMove,
    character_Iterator_Has_اگلا,
    character_Iterator_Has_پچھلا,
    characterIteratorCurrent,
    character_Iterator_اگلا,
    character_Iterator_پچھلا,
    خالی_اشاریہ,
    characterIteratorGetState,
    character_Iterator_سیٹ_State
};

U_CAPI خالی U_EXPORT2
uiter_سیٹ_Character_Iterator(UCharIterator *iter, CharacterIterator *charIter) {
    اگر (iter != خالی_اشاریہ) {
        اگر (charIter != خالی_اشاریہ) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } ورنہ {
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

ساکن UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    اگر(iter->اشاریہ<iter->حد_بندی) {
        واپس ((Replaceable *)(iter->context))->charAt(iter->اشاریہ);
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
replaceable_Iterator_اگلا(UCharIterator *iter) {
    اگر(iter->اشاریہ<iter->حد_بندی) {
        واپس ((Replaceable *)(iter->context))->charAt(iter->اشاریہ++);
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
replaceable_Iterator_پچھلا(UCharIterator *iter) {
    اگر(iter->اشاریہ>iter->آغاز) {
        واپس ((Replaceable *)(iter->context))->charAt(--iter->اشاریہ);
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن ثابت UCharIterator replaceableIterator={
    خالی_اشاریہ, 0, 0, 0, 0, 0,
    string_Iterator_Get_اشاریہ,
    stringIteratorMove,
    string_Iterator_Has_اگلا,
    string_Iterator_Has_پچھلا,
    replaceableIteratorCurrent,
    replaceable_Iterator_اگلا,
    replaceable_Iterator_پچھلا,
    خالی_اشاریہ,
    stringIteratorGetState,
    string_Iterator_سیٹ_State
};

U_CAPI خالی U_EXPORT2
uiter_سیٹ_Replaceable(UCharIterator *iter, ثابت Replaceable *rep) {
    اگر (iter != خالی_اشاریہ) {
        اگر (rep != خالی_اشاریہ) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->حد_بندی=iter->length=rep->length();
        } ورنہ {
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

#تعریف_کرو UITER_CNV_استعداد 16

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

ساکن int32_t U_CALLCONV
utf8_Iterator_Get_اشاریہ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    انتخاب(origin) {
    حالت UITER_ZERO:
    حالت UITER_آغاز:
        واپس 0;
    حالت UITER_CURRENT:
        اگر(iter->اشاریہ<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ثابت uint8_t *s;
            UChar32 c;
            int32_t i, حد_بندی, اشاریہ;

            s=(ثابت uint8_t *)iter->context;
            i=اشاریہ=0;
            حد_بندی=iter->آغاز; /* count up to the UTF-8 index */
            جب(i<حد_بندی) {
                U8_اگلا_OR_FFFD(s, i, حد_بندی, c);
                اشاریہ+=U16_لمبائی(c);
            }

            iter->آغاز=i; /* just in case setState() did not get us to a code point boundary */
            اگر(i==iter->حد_بندی) {
                iter->length=اشاریہ; /* in case it was <0 or wrong */
            }
            اگر(iter->reserved_میدان!=0) {
                --اشاریہ; /* we are in the middle of a supplementary code point */
            }
            iter->اشاریہ=اشاریہ;
        }
        واپس iter->اشاریہ;
    حالت UITER_حد_بندی:
    حالت UITER_لمبائی:
        اگر(iter->length<0) {
            ثابت uint8_t *s;
            UChar32 c;
            int32_t i, حد_بندی, length;

            s=(ثابت uint8_t *)iter->context;
            اگر(iter->اشاریہ<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                حد_بندی=iter->آغاز;

                /* count from the beginning to the current index */
                جب(i<حد_بندی) {
                    U8_اگلا_OR_FFFD(s, i, حد_بندی, c);
                    length+=U16_لمبائی(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->آغاز=i; /* just in case setState() did not get us to a code point boundary */
                iter->اشاریہ= iter->reserved_میدان!=0 ? length-1 : length;
            } ورنہ {
                i=iter->آغاز;
                length=iter->اشاریہ;
                اگر(iter->reserved_میدان!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            حد_بندی=iter->حد_بندی;
            جب(i<حد_بندی) {
                U8_اگلا_OR_FFFD(s, i, حد_بندی, c);
                length+=U16_لمبائی(c);
            }
            iter->length=length;
        }
        واپس iter->length;
    طےشدہ:
        /* not a valid origin */
        /* Should never get here! */
        واپس -1;
    }
}

ساکن int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ثابت uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    انتخاب(origin) {
    حالت UITER_ZERO:
    حالت UITER_آغاز:
        pos=delta;
        havePos=سچ;
        /* iter->index<0 (unknown) is possible */
        توڑو;
    حالت UITER_CURRENT:
        اگر(iter->اشاریہ>=0) {
            pos=iter->اشاریہ+delta;
            havePos=سچ;
        } ورنہ {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=جھوٹ;
        }
        توڑو;
    حالت UITER_حد_بندی:
    حالت UITER_لمبائی:
        اگر(iter->length>=0) {
            pos=iter->length+delta;
            havePos=سچ;
        } ورنہ {
            /* pin to the end, avoid counting the length */
            iter->اشاریہ=-1;
            iter->آغاز=iter->حد_بندی;
            iter->reserved_میدان=0;
            اگر(delta>=0) {
                واپس UITER_UNKNOWN_اشاریہ;
            } ورنہ {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=جھوٹ;
            }
        }
        توڑو;
    طےشدہ:
        واپس -1;  /* Error */
    }

    اگر(havePos) {
        /* shortcuts: pinning to the edges of the string */
        اگر(pos<=0) {
            iter->اشاریہ=iter->آغاز=iter->reserved_میدان=0;
            واپس 0;
        } ورنہ اگر(iter->length>=0 && pos>=iter->length) {
            iter->اشاریہ=iter->length;
            iter->آغاز=iter->حد_بندی;
            iter->reserved_میدان=0;
            واپس iter->اشاریہ;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        اگر(iter->اشاریہ<0 || pos<iter->اشاریہ/2) {
            /* go forward from the start instead of backward from the current index */
            iter->اشاریہ=iter->آغاز=iter->reserved_میدان=0;
        } ورنہ اگر(iter->length>=0 && (iter->length-pos)<(pos-iter->اشاریہ)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->اشاریہ=iter->length;
            iter->آغاز=iter->حد_بندی;
            iter->reserved_میدان=0;
        }

        delta=pos-iter->اشاریہ;
        اگر(delta==0) {
            واپس iter->اشاریہ; /* nothing to do */
        }
    } ورنہ {
        /* move relative to unknown UTF-16 index */
        اگر(delta==0) {
            واپس UITER_UNKNOWN_اشاریہ; /* nothing to do */
        } ورنہ اگر(-delta>=iter->آغاز) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->اشاریہ=iter->آغاز=iter->reserved_میدان=0;
            واپس 0;
        } ورنہ اگر(delta>=(iter->حد_بندی-iter->آغاز)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->اشاریہ=iter->length; /* may or may not be <0 (unknown) */
            iter->آغاز=iter->حد_بندی;
            iter->reserved_میدان=0;
            واپس iter->اشاریہ>=0 ? iter->اشاریہ : (int32_t)UITER_UNKNOWN_اشاریہ;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ثابت uint8_t *)iter->context;
    pos=iter->اشاریہ; /* could be <0 (unknown) */
    i=iter->آغاز;
    اگر(delta>0) {
        /* go forward */
        int32_t حد_بندی=iter->حد_بندی;
        اگر(iter->reserved_میدان!=0) {
            iter->reserved_میدان=0;
            ++pos;
            --delta;
        }
        جب(delta>0 && i<حد_بندی) {
            U8_اگلا_OR_FFFD(s, i, حد_بندی, c);
            اگر(c<=0xffff) {
                ++pos;
                --delta;
            } ورنہ اگر(delta>=2) {
                pos+=2;
                delta-=2;
            } ورنہ /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reserved_میدان=c;
                ++pos;
                توڑو; /* delta=0; */
            }
        }
        اگر(i==حد_بندی) {
            اگر(iter->length<0 && iter->اشاریہ>=0) {
                iter->length= iter->reserved_میدان==0 ? pos : pos+1;
            } ورنہ اگر(iter->اشاریہ<0 && iter->length>=0) {
                iter->اشاریہ= iter->reserved_میدان==0 ? iter->length : iter->length-1;
            }
        }
    } ورنہ /* delta<0 */ {
        /* go backward */
        اگر(iter->reserved_میدان!=0) {
            iter->reserved_میدان=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        جب(delta<0 && i>0) {
            U8_پچھلا_OR_FFFD(s, 0, i, c);
            اگر(c<=0xffff) {
                --pos;
                ++delta;
            } ورنہ اگر(delta<=-2) {
                pos-=2;
                delta+=2;
            } ورنہ /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reserved_میدان=c;
                --pos;
                توڑو; /* delta=0; */
            }
        }
    }

    iter->آغاز=i;
    اگر(iter->اشاریہ>=0) {
        واپس iter->اشاریہ=pos;
    } ورنہ {
        /* we started with index<0 (unknown) so pos is bogus */
        اگر(i<=1) {
            واپس iter->اشاریہ=i; /* reached the beginning */
        } ورنہ {
            /* we still don't know the UTF-16 index */
            واپس UITER_UNKNOWN_اشاریہ;
        }
    }
}

ساکن UBool U_CALLCONV
utf8_Iterator_Has_اگلا(UCharIterator *iter) {
    واپس iter->آغاز<iter->حد_بندی || iter->reserved_میدان!=0;
}

ساکن UBool U_CALLCONV
utf8_Iterator_Has_پچھلا(UCharIterator *iter) {
    واپس iter->آغاز>0;
}

ساکن UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    اگر(iter->reserved_میدان!=0) {
        واپس U16_TRAIL(iter->reserved_میدان);
    } ورنہ اگر(iter->آغاز<iter->حد_بندی) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->آغاز;

        U8_اگلا_OR_FFFD(s, i, iter->حد_بندی, c);
        اگر(c<=0xffff) {
            واپس c;
        } ورنہ {
            واپس U16_LEAD(c);
        }
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
utf8_Iterator_اگلا(UCharIterator *iter) {
    int32_t اشاریہ;

    اگر(iter->reserved_میدان!=0) {
        char16_t trail=U16_TRAIL(iter->reserved_میدان);
        iter->reserved_میدان=0;
        اگر((اشاریہ=iter->اشاریہ)>=0) {
            iter->اشاریہ=اشاریہ+1;
        }
        واپس trail;
    } ورنہ اگر(iter->آغاز<iter->حد_بندی) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;

        U8_اگلا_OR_FFFD(s, iter->آغاز, iter->حد_بندی, c);
        اگر((اشاریہ=iter->اشاریہ)>=0) {
            iter->اشاریہ=++اشاریہ;
            اگر(iter->length<0 && iter->آغاز==iter->حد_بندی) {
                iter->length= c<=0xffff ? اشاریہ : اشاریہ+1;
            }
        } ورنہ اگر(iter->آغاز==iter->حد_بندی && iter->length>=0) {
            iter->اشاریہ= c<=0xffff ? iter->length : iter->length-1;
        }
        اگر(c<=0xffff) {
            واپس c;
        } ورنہ {
            iter->reserved_میدان=c;
            واپس U16_LEAD(c);
        }
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن UChar32 U_CALLCONV
utf8_Iterator_پچھلا(UCharIterator *iter) {
    int32_t اشاریہ;

    اگر(iter->reserved_میدان!=0) {
        char16_t lead=U16_LEAD(iter->reserved_میدان);
        iter->reserved_میدان=0;
        iter->آغاز-=4; /* we stayed behind the supplementary code point; go before it now */
        اگر((اشاریہ=iter->اشاریہ)>0) {
            iter->اشاریہ=اشاریہ-1;
        }
        واپس lead;
    } ورنہ اگر(iter->آغاز>0) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;

        U8_پچھلا_OR_FFFD(s, 0, iter->آغاز, c);
        اگر((اشاریہ=iter->اشاریہ)>0) {
            iter->اشاریہ=اشاریہ-1;
        } ورنہ اگر(iter->آغاز<=1) {
            iter->اشاریہ= c<=0xffff ? iter->آغاز : iter->آغاز+1;
        }
        اگر(c<=0xffff) {
            واپس c;
        } ورنہ {
            iter->آغاز+=4; /* back to behind this supplementary code point for consistent state */
            iter->reserved_میدان=c;
            واپس U16_TRAIL(c);
        }
    } ورنہ {
        واپس U_SENTINEL;
    }
}

ساکن uint32_t U_CALLCONV
utf8IteratorGetState(ثابت UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->آغاز<<1);
    اگر(iter->reserved_میدان!=0) {
        state|=1;
    }
    واپس state;
}

ساکن خالی U_CALLCONV
utf8_Iterator_سیٹ_State(UCharIterator *iter,
                     uint32_t state,
                     UError_رمز *p_Error_رمز)
{
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        /* do nothing */
    } ورنہ اگر(iter==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    } ورنہ اگر(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } ورنہ {
        int32_t اشاریہ=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        اگر((state==0 ? اشاریہ<0 : اشاریہ<4) || iter->حد_بندی<اشاریہ) {
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        } ورنہ {
            iter->آغاز=اشاریہ; /* restore UTF-8 byte index */
            اگر(اشاریہ<=1) {
                iter->اشاریہ=اشاریہ;
            } ورنہ {
                iter->اشاریہ=-1; /* unknown UTF-16 index */
            }
            اگر(state==0) {
                iter->reserved_میدان=0;
            } ورنہ {
                /* verified index>=4 above */
                UChar32 c;
                U8_پچھلا_OR_FFFD((ثابت uint8_t *)iter->context, 0, اشاریہ, c);
                اگر(c<=0xffff) {
                    *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                } ورنہ {
                    iter->reserved_میدان=c;
                }
            }
        }
    }
}

ساکن ثابت UCharIterator utf8Iterator={
    خالی_اشاریہ, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_اشاریہ,
    utf8IteratorMove,
    utf8_Iterator_Has_اگلا,
    utf8_Iterator_Has_پچھلا,
    utf8IteratorCurrent,
    utf8_Iterator_اگلا,
    utf8_Iterator_پچھلا,
    خالی_اشاریہ,
    utf8IteratorGetState,
    utf8_Iterator_سیٹ_State
};

U_CAPI خالی U_EXPORT2
uiter_سیٹ_UTF8(UCharIterator *iter, ثابت حرف *s, int32_t length) {
    اگر (iter != خالی_اشاریہ) {
        اگر (s != خالی_اشاریہ && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            اگر(length>=0) {
                iter->حد_بندی=length;
            } ورنہ {
                iter->حد_بندی=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->حد_بندی<=1 ? iter->حد_بندی : -1;
        } ورنہ {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    اگر(U16_IS_SURROGATE(c)) {
        اگر(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            اگر(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } ورنہ {
            اگر(U16_IS_LEAD(c2=iter->پچھلا_2(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            اگر(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    واپس c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    اگر(U16_IS_LEAD(c)) {
        اگر(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } ورنہ اگر(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    واپس c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->پچھلا_2(iter);
    اگر(U16_IS_TRAIL(c)) {
        اگر(U16_IS_LEAD(c2=iter->پچھلا_2(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } ورنہ اگر(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    واپس c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ثابت UCharIterator *iter) {
    اگر(iter==خالی_اشاریہ || iter->getState==خالی_اشاریہ) {
        واپس UITER_NO_STATE;
    } ورنہ {
        واپس iter->getState(iter);
    }
}

U_CAPI خالی U_EXPORT2
uiter_سیٹ_State(UCharIterator *iter, uint32_t state, UError_رمز *p_Error_رمز) {
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        /* do nothing */
    } ورنہ اگر(iter==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
    } ورنہ اگر(iter->سیٹ_State==خالی_اشاریہ) {
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
    } ورنہ {
        iter->سیٹ_State(iter, state, p_Error_رمز);
    }
}

U_CDECL_انتہا
