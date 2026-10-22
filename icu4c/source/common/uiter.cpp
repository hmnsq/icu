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

#تعریف_کرو IS_EVEN(n) (((n)&1)==0)
#تعریف_کرو IS_اشارہ_گر_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

ساكن int32_t U_CALLCONV
noop_Get_اشاریہ(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    أرجع 0;
}

ساكن int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    أرجع 0;
}

ساكن UBool U_CALLCONV
noop_Has_اگلا(UCharIterator * /*iter*/) {
    أرجع خطأ;
}

ساكن UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    أرجع U_SENTINEL;
}

ساكن uint32_t U_CALLCONV
noopGetState(ثابت UCharIterator * /*iter*/) {
    أرجع UITER_NO_STATE;
}

ساكن فراغ U_CALLCONV
noop_سیٹ_State(UCharIterator * /*iter*/, uint32_t /*state*/, UError_کوڈ *p_Error_کوڈ) {
    *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
}

ساكن ثابت UCharIterator noopIterator={
    خالی_سوچک, 0, 0, 0, 0, 0,
    noop_Get_اشاریہ,
    noopMove,
    noop_Has_اگلا,
    noop_Has_اگلا,
    noopCurrent,
    noopCurrent,
    noopCurrent,
    خالی_سوچک,
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

ساكن int32_t U_CALLCONV
string_Iterator_Get_اشاریہ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
        أرجع 0;
    حالة UITER_شروع:
        أرجع iter->شروع;
    حالة UITER_CURRENT:
        أرجع iter->اشاریہ;
    حالة UITER_حد_بندی:
        أرجع iter->حد_بندی;
    حالة UITER_لمائی:
        أرجع iter->length;
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        أرجع -1;
    }
}

ساكن int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    بدل(origin) {
    حالة UITER_ZERO:
        pos=delta;
        اكسر;
    حالة UITER_شروع:
        pos=iter->شروع+delta;
        اكسر;
    حالة UITER_CURRENT:
        pos=iter->اشاریہ+delta;
        اكسر;
    حالة UITER_حد_بندی:
        pos=iter->حد_بندی+delta;
        اكسر;
    حالة UITER_لمائی:
        pos=iter->length+delta;
        اكسر;
    افتراضي:
        أرجع -1;  /* Error */
    }

    إذا(pos<iter->شروع) {
        pos=iter->شروع;
    } وإلا إذا(pos>iter->حد_بندی) {
        pos=iter->حد_بندی;
    }

    أرجع iter->اشاریہ=pos;
}

ساكن UBool U_CALLCONV
string_Iterator_Has_اگلا(UCharIterator *iter) {
    أرجع iter->اشاریہ<iter->حد_بندی;
}

ساكن UBool U_CALLCONV
string_Iterator_Has_پچھلا(UCharIterator *iter) {
    أرجع iter->اشاریہ>iter->شروع;
}

ساكن UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    إذا(iter->اشاریہ<iter->حد_بندی) {
        أرجع ((ثابت char16_t *)(iter->context))[iter->اشاریہ];
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
string_Iterator_اگلا(UCharIterator *iter) {
    إذا(iter->اشاریہ<iter->حد_بندی) {
        أرجع ((ثابت char16_t *)(iter->context))[iter->اشاریہ++];
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
string_Iterator_پچھلا(UCharIterator *iter) {
    إذا(iter->اشاریہ>iter->شروع) {
        أرجع ((ثابت char16_t *)(iter->context))[--iter->اشاریہ];
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
stringIteratorGetState(ثابت UCharIterator *iter) {
    أرجع (uint32_t)iter->اشاریہ;
}

ساكن فراغ U_CALLCONV
string_Iterator_سیٹ_State(UCharIterator *iter, uint32_t state, UError_کوڈ *p_Error_کوڈ) {
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        /* do nothing */
    } وإلا إذا(iter==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
    } وإلا إذا((int32_t)state<iter->شروع || iter->حد_بندی<(int32_t)state) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
    } وإلا {
        iter->اشاریہ=(int32_t)state;
    }
}

ساكن ثابت UCharIterator stringIterator={
    خالی_سوچک, 0, 0, 0, 0, 0,
    string_Iterator_Get_اشاریہ,
    stringIteratorMove,
    string_Iterator_Has_اگلا,
    string_Iterator_Has_پچھلا,
    stringIteratorCurrent,
    string_Iterator_اگلا,
    string_Iterator_پچھلا,
    خالی_سوچک,
    stringIteratorGetState,
    string_Iterator_سیٹ_State
};

U_CAPI فراغ U_EXPORT2
uiter_سیٹ_String(UCharIterator *iter, ثابت char16_t *s, int32_t length) {
    إذا (iter != خالی_سوچک) {
        إذا (s != خالی_سوچک && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            إذا(length>=0) {
                iter->length=length;
            } وإلا {
                iter->length=u_strlen(s);
            }
            iter->حد_بندی=iter->length;
        } وإلا {
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
utf16BEIteratorGet(UCharIterator *iter, int32_t اشاریہ) {
    ثابت uint8_t *p=(ثابت uint8_t *)iter->context;
    أرجع ((char16_t)p[2*اشاریہ]<<8)|(char16_t)p[2*اشاریہ+1];
}

ساكن UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t اشاریہ;

    إذا((اشاریہ=iter->اشاریہ)<iter->حد_بندی) {
        أرجع utf16BEIteratorGet(iter, اشاریہ);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf16_BEIterator_اگلا(UCharIterator *iter) {
    int32_t اشاریہ;

    إذا((اشاریہ=iter->اشاریہ)<iter->حد_بندی) {
        iter->اشاریہ=اشاریہ+1;
        أرجع utf16BEIteratorGet(iter, اشاریہ);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf16_BEIterator_پچھلا(UCharIterator *iter) {
    int32_t اشاریہ;

    إذا((اشاریہ=iter->اشاریہ)>iter->شروع) {
        iter->اشاریہ=--اشاریہ;
        أرجع utf16BEIteratorGet(iter, اشاریہ);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن ثابت UCharIterator utf16BEIterator={
    خالی_سوچک, 0, 0, 0, 0, 0,
    string_Iterator_Get_اشاریہ,
    stringIteratorMove,
    string_Iterator_Has_اگلا,
    string_Iterator_Has_پچھلا,
    utf16BEIteratorCurrent,
    utf16_BEIterator_اگلا,
    utf16_BEIterator_پچھلا,
    خالی_سوچک,
    stringIteratorGetState,
    string_Iterator_سیٹ_State
};

/*
 * Count the number of UChars in a UTF-16BE string before a terminating char16_t NUL,
 * i.e., before a pair of 0 bytes where the first 0 byte is at an even
 * offset from s.
 */
ساكن int32_t
utf16BE_strlen(ثابت حرف *s) {
    إذا(IS_اشارہ_گر_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        أرجع u_strlen((ثابت char16_t *)s);
    } وإلا {
        /* odd-aligned, search for pair of 0 bytes */
        ثابت حرف *p=s;

        طالما(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        أرجع (int32_t)((p-s)/2);
    }
}

U_CAPI فراغ U_EXPORT2
uiter_سیٹ_UTF16_BE(UCharIterator *iter, ثابت حرف *s, int32_t length) {
    إذا(iter!=خالی_سوچک) {
        /* allow only even-length strings (the input length counts bytes) */
        إذا(s!=خالی_سوچک && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            إذا(U_IS_BIG_ENDIAN && IS_اشارہ_گر_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_سیٹ_String(iter, (ثابت char16_t *)s, length);
                أرجع;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            إذا(length>=0) {
                iter->length=length;
            } وإلا {
                iter->length=utf16BE_strlen(s);
            }
            iter->حد_بندی=iter->length;
        } وإلا {
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
character_Iterator_Get_اشاریہ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
        أرجع 0;
    حالة UITER_شروع:
        أرجع ((CharacterIterator *)(iter->context))->شروع_اشاریہ();
    حالة UITER_CURRENT:
        أرجع ((CharacterIterator *)(iter->context))->get_اشاریہ();
    حالة UITER_حد_بندی:
        أرجع ((CharacterIterator *)(iter->context))->سرا_اشاریہ();
    حالة UITER_لمائی:
        أرجع ((CharacterIterator *)(iter->context))->get_لمائی();
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        أرجع -1;
    }
}

ساكن int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
        ((CharacterIterator *)(iter->context))->سیٹ_اشاریہ(delta);
        أرجع ((CharacterIterator *)(iter->context))->get_اشاریہ();
    حالة UITER_شروع:
    حالة UITER_CURRENT:
    حالة UITER_حد_بندی:
        أرجع ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    حالة UITER_لمائی:
        ((CharacterIterator *)(iter->context))->سیٹ_اشاریہ(((CharacterIterator *)(iter->context))->get_لمائی()+delta);
        أرجع ((CharacterIterator *)(iter->context))->get_اشاریہ();
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        أرجع -1;
    }
}

ساكن UBool U_CALLCONV
character_Iterator_Has_اگلا(UCharIterator *iter) {
    أرجع ((CharacterIterator *)(iter->context))->has_اگلا();
}

ساكن UBool U_CALLCONV
character_Iterator_Has_پچھلا(UCharIterator *iter) {
    أرجع ((CharacterIterator *)(iter->context))->has_پچھلا();
}

ساكن UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    إذا(c!=0xffff || ((CharacterIterator *)(iter->context))->has_اگلا()) {
        أرجع c;
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
character_Iterator_اگلا(UCharIterator *iter) {
    إذا(((CharacterIterator *)(iter->context))->has_اگلا()) {
        أرجع ((CharacterIterator *)(iter->context))->اگلا_Post_Inc();
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
character_Iterator_پچھلا(UCharIterator *iter) {
    إذا(((CharacterIterator *)(iter->context))->has_پچھلا()) {
        أرجع ((CharacterIterator *)(iter->context))->پچھلا();
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
characterIteratorGetState(ثابت UCharIterator *iter) {
    أرجع ((CharacterIterator *)(iter->context))->get_اشاریہ();
}

ساكن فراغ U_CALLCONV
character_Iterator_سیٹ_State(UCharIterator *iter, uint32_t state, UError_کوڈ *p_Error_کوڈ) {
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        /* do nothing */
    } وإلا إذا(iter==خالی_سوچک || iter->context==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
    } وإلا إذا((int32_t)state<((CharacterIterator *)(iter->context))->شروع_اشاریہ() || ((CharacterIterator *)(iter->context))->سرا_اشاریہ()<(int32_t)state) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
    } وإلا {
        ((CharacterIterator *)(iter->context))->سیٹ_اشاریہ((int32_t)state);
    }
}

ساكن ثابت UCharIterator characterIteratorWrapper={
    خالی_سوچک, 0, 0, 0, 0, 0,
    character_Iterator_Get_اشاریہ,
    characterIteratorMove,
    character_Iterator_Has_اگلا,
    character_Iterator_Has_پچھلا,
    characterIteratorCurrent,
    character_Iterator_اگلا,
    character_Iterator_پچھلا,
    خالی_سوچک,
    characterIteratorGetState,
    character_Iterator_سیٹ_State
};

U_CAPI فراغ U_EXPORT2
uiter_سیٹ_Character_Iterator(UCharIterator *iter, CharacterIterator *charIter) {
    إذا (iter != خالی_سوچک) {
        إذا (charIter != خالی_سوچک) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } وإلا {
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
    إذا(iter->اشاریہ<iter->حد_بندی) {
        أرجع ((Replaceable *)(iter->context))->charAt(iter->اشاریہ);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
replaceable_Iterator_اگلا(UCharIterator *iter) {
    إذا(iter->اشاریہ<iter->حد_بندی) {
        أرجع ((Replaceable *)(iter->context))->charAt(iter->اشاریہ++);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
replaceable_Iterator_پچھلا(UCharIterator *iter) {
    إذا(iter->اشاریہ>iter->شروع) {
        أرجع ((Replaceable *)(iter->context))->charAt(--iter->اشاریہ);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن ثابت UCharIterator replaceableIterator={
    خالی_سوچک, 0, 0, 0, 0, 0,
    string_Iterator_Get_اشاریہ,
    stringIteratorMove,
    string_Iterator_Has_اگلا,
    string_Iterator_Has_پچھلا,
    replaceableIteratorCurrent,
    replaceable_Iterator_اگلا,
    replaceable_Iterator_پچھلا,
    خالی_سوچک,
    stringIteratorGetState,
    string_Iterator_سیٹ_State
};

U_CAPI فراغ U_EXPORT2
uiter_سیٹ_Replaceable(UCharIterator *iter, ثابت Replaceable *rep) {
    إذا (iter != خالی_سوچک) {
        إذا (rep != خالی_سوچک) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->حد_بندی=iter->length=rep->length();
        } وإلا {
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

#تعریف_کرو UITER_CNV_سمرتھا 16

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
utf8_Iterator_Get_اشاریہ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
    حالة UITER_شروع:
        أرجع 0;
    حالة UITER_CURRENT:
        إذا(iter->اشاریہ<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ثابت uint8_t *s;
            UChar32 c;
            int32_t i, حد_بندی, اشاریہ;

            s=(ثابت uint8_t *)iter->context;
            i=اشاریہ=0;
            حد_بندی=iter->شروع; /* count up to the UTF-8 index */
            طالما(i<حد_بندی) {
                U8_اگلا_OR_FFFD(s, i, حد_بندی, c);
                اشاریہ+=U16_لمائی(c);
            }

            iter->شروع=i; /* just in case setState() did not get us to a code point boundary */
            إذا(i==iter->حد_بندی) {
                iter->length=اشاریہ; /* in case it was <0 or wrong */
            }
            إذا(iter->reserved_میدان!=0) {
                --اشاریہ; /* we are in the middle of a supplementary code point */
            }
            iter->اشاریہ=اشاریہ;
        }
        أرجع iter->اشاریہ;
    حالة UITER_حد_بندی:
    حالة UITER_لمائی:
        إذا(iter->length<0) {
            ثابت uint8_t *s;
            UChar32 c;
            int32_t i, حد_بندی, length;

            s=(ثابت uint8_t *)iter->context;
            إذا(iter->اشاریہ<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                حد_بندی=iter->شروع;

                /* count from the beginning to the current index */
                طالما(i<حد_بندی) {
                    U8_اگلا_OR_FFFD(s, i, حد_بندی, c);
                    length+=U16_لمائی(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->شروع=i; /* just in case setState() did not get us to a code point boundary */
                iter->اشاریہ= iter->reserved_میدان!=0 ? length-1 : length;
            } وإلا {
                i=iter->شروع;
                length=iter->اشاریہ;
                إذا(iter->reserved_میدان!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            حد_بندی=iter->حد_بندی;
            طالما(i<حد_بندی) {
                U8_اگلا_OR_FFFD(s, i, حد_بندی, c);
                length+=U16_لمائی(c);
            }
            iter->length=length;
        }
        أرجع iter->length;
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        أرجع -1;
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
    حالة UITER_شروع:
        pos=delta;
        havePos=سچ;
        /* iter->index<0 (unknown) is possible */
        اكسر;
    حالة UITER_CURRENT:
        إذا(iter->اشاریہ>=0) {
            pos=iter->اشاریہ+delta;
            havePos=سچ;
        } وإلا {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=خطأ;
        }
        اكسر;
    حالة UITER_حد_بندی:
    حالة UITER_لمائی:
        إذا(iter->length>=0) {
            pos=iter->length+delta;
            havePos=سچ;
        } وإلا {
            /* pin to the end, avoid counting the length */
            iter->اشاریہ=-1;
            iter->شروع=iter->حد_بندی;
            iter->reserved_میدان=0;
            إذا(delta>=0) {
                أرجع UITER_UNKNOWN_اشاریہ;
            } وإلا {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=خطأ;
            }
        }
        اكسر;
    افتراضي:
        أرجع -1;  /* Error */
    }

    إذا(havePos) {
        /* shortcuts: pinning to the edges of the string */
        إذا(pos<=0) {
            iter->اشاریہ=iter->شروع=iter->reserved_میدان=0;
            أرجع 0;
        } وإلا إذا(iter->length>=0 && pos>=iter->length) {
            iter->اشاریہ=iter->length;
            iter->شروع=iter->حد_بندی;
            iter->reserved_میدان=0;
            أرجع iter->اشاریہ;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        إذا(iter->اشاریہ<0 || pos<iter->اشاریہ/2) {
            /* go forward from the start instead of backward from the current index */
            iter->اشاریہ=iter->شروع=iter->reserved_میدان=0;
        } وإلا إذا(iter->length>=0 && (iter->length-pos)<(pos-iter->اشاریہ)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->اشاریہ=iter->length;
            iter->شروع=iter->حد_بندی;
            iter->reserved_میدان=0;
        }

        delta=pos-iter->اشاریہ;
        إذا(delta==0) {
            أرجع iter->اشاریہ; /* nothing to do */
        }
    } وإلا {
        /* move relative to unknown UTF-16 index */
        إذا(delta==0) {
            أرجع UITER_UNKNOWN_اشاریہ; /* nothing to do */
        } وإلا إذا(-delta>=iter->شروع) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->اشاریہ=iter->شروع=iter->reserved_میدان=0;
            أرجع 0;
        } وإلا إذا(delta>=(iter->حد_بندی-iter->شروع)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->اشاریہ=iter->length; /* may or may not be <0 (unknown) */
            iter->شروع=iter->حد_بندی;
            iter->reserved_میدان=0;
            أرجع iter->اشاریہ>=0 ? iter->اشاریہ : (int32_t)UITER_UNKNOWN_اشاریہ;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ثابت uint8_t *)iter->context;
    pos=iter->اشاریہ; /* could be <0 (unknown) */
    i=iter->شروع;
    إذا(delta>0) {
        /* go forward */
        int32_t حد_بندی=iter->حد_بندی;
        إذا(iter->reserved_میدان!=0) {
            iter->reserved_میدان=0;
            ++pos;
            --delta;
        }
        طالما(delta>0 && i<حد_بندی) {
            U8_اگلا_OR_FFFD(s, i, حد_بندی, c);
            إذا(c<=0xffff) {
                ++pos;
                --delta;
            } وإلا إذا(delta>=2) {
                pos+=2;
                delta-=2;
            } وإلا /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reserved_میدان=c;
                ++pos;
                اكسر; /* delta=0; */
            }
        }
        إذا(i==حد_بندی) {
            إذا(iter->length<0 && iter->اشاریہ>=0) {
                iter->length= iter->reserved_میدان==0 ? pos : pos+1;
            } وإلا إذا(iter->اشاریہ<0 && iter->length>=0) {
                iter->اشاریہ= iter->reserved_میدان==0 ? iter->length : iter->length-1;
            }
        }
    } وإلا /* delta<0 */ {
        /* go backward */
        إذا(iter->reserved_میدان!=0) {
            iter->reserved_میدان=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        طالما(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            إذا(c<=0xffff) {
                --pos;
                ++delta;
            } وإلا إذا(delta<=-2) {
                pos-=2;
                delta+=2;
            } وإلا /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reserved_میدان=c;
                --pos;
                اكسر; /* delta=0; */
            }
        }
    }

    iter->شروع=i;
    إذا(iter->اشاریہ>=0) {
        أرجع iter->اشاریہ=pos;
    } وإلا {
        /* we started with index<0 (unknown) so pos is bogus */
        إذا(i<=1) {
            أرجع iter->اشاریہ=i; /* reached the beginning */
        } وإلا {
            /* we still don't know the UTF-16 index */
            أرجع UITER_UNKNOWN_اشاریہ;
        }
    }
}

ساكن UBool U_CALLCONV
utf8_Iterator_Has_اگلا(UCharIterator *iter) {
    أرجع iter->شروع<iter->حد_بندی || iter->reserved_میدان!=0;
}

ساكن UBool U_CALLCONV
utf8_Iterator_Has_پچھلا(UCharIterator *iter) {
    أرجع iter->شروع>0;
}

ساكن UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    إذا(iter->reserved_میدان!=0) {
        أرجع U16_TRAIL(iter->reserved_میدان);
    } وإلا إذا(iter->شروع<iter->حد_بندی) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->شروع;

        U8_اگلا_OR_FFFD(s, i, iter->حد_بندی, c);
        إذا(c<=0xffff) {
            أرجع c;
        } وإلا {
            أرجع U16_LEAD(c);
        }
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf8_Iterator_اگلا(UCharIterator *iter) {
    int32_t اشاریہ;

    إذا(iter->reserved_میدان!=0) {
        char16_t trail=U16_TRAIL(iter->reserved_میدان);
        iter->reserved_میدان=0;
        إذا((اشاریہ=iter->اشاریہ)>=0) {
            iter->اشاریہ=اشاریہ+1;
        }
        أرجع trail;
    } وإلا إذا(iter->شروع<iter->حد_بندی) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;

        U8_اگلا_OR_FFFD(s, iter->شروع, iter->حد_بندی, c);
        إذا((اشاریہ=iter->اشاریہ)>=0) {
            iter->اشاریہ=++اشاریہ;
            إذا(iter->length<0 && iter->شروع==iter->حد_بندی) {
                iter->length= c<=0xffff ? اشاریہ : اشاریہ+1;
            }
        } وإلا إذا(iter->شروع==iter->حد_بندی && iter->length>=0) {
            iter->اشاریہ= c<=0xffff ? iter->length : iter->length-1;
        }
        إذا(c<=0xffff) {
            أرجع c;
        } وإلا {
            iter->reserved_میدان=c;
            أرجع U16_LEAD(c);
        }
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf8_Iterator_پچھلا(UCharIterator *iter) {
    int32_t اشاریہ;

    إذا(iter->reserved_میدان!=0) {
        char16_t lead=U16_LEAD(iter->reserved_میدان);
        iter->reserved_میدان=0;
        iter->شروع-=4; /* we stayed behind the supplementary code point; go before it now */
        إذا((اشاریہ=iter->اشاریہ)>0) {
            iter->اشاریہ=اشاریہ-1;
        }
        أرجع lead;
    } وإلا إذا(iter->شروع>0) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->شروع, c);
        إذا((اشاریہ=iter->اشاریہ)>0) {
            iter->اشاریہ=اشاریہ-1;
        } وإلا إذا(iter->شروع<=1) {
            iter->اشاریہ= c<=0xffff ? iter->شروع : iter->شروع+1;
        }
        إذا(c<=0xffff) {
            أرجع c;
        } وإلا {
            iter->شروع+=4; /* back to behind this supplementary code point for consistent state */
            iter->reserved_میدان=c;
            أرجع U16_TRAIL(c);
        }
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
utf8IteratorGetState(ثابت UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->شروع<<1);
    إذا(iter->reserved_میدان!=0) {
        state|=1;
    }
    أرجع state;
}

ساكن فراغ U_CALLCONV
utf8_Iterator_سیٹ_State(UCharIterator *iter,
                     uint32_t state,
                     UError_کوڈ *p_Error_کوڈ)
{
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        /* do nothing */
    } وإلا إذا(iter==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
    } وإلا إذا(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } وإلا {
        int32_t اشاریہ=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        إذا((state==0 ? اشاریہ<0 : اشاریہ<4) || iter->حد_بندی<اشاریہ) {
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        } وإلا {
            iter->شروع=اشاریہ; /* restore UTF-8 byte index */
            إذا(اشاریہ<=1) {
                iter->اشاریہ=اشاریہ;
            } وإلا {
                iter->اشاریہ=-1; /* unknown UTF-16 index */
            }
            إذا(state==0) {
                iter->reserved_میدان=0;
            } وإلا {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((ثابت uint8_t *)iter->context, 0, اشاریہ, c);
                إذا(c<=0xffff) {
                    *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
                } وإلا {
                    iter->reserved_میدان=c;
                }
            }
        }
    }
}

ساكن ثابت UCharIterator utf8Iterator={
    خالی_سوچک, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_اشاریہ,
    utf8IteratorMove,
    utf8_Iterator_Has_اگلا,
    utf8_Iterator_Has_پچھلا,
    utf8IteratorCurrent,
    utf8_Iterator_اگلا,
    utf8_Iterator_پچھلا,
    خالی_سوچک,
    utf8IteratorGetState,
    utf8_Iterator_سیٹ_State
};

U_CAPI فراغ U_EXPORT2
uiter_سیٹ_UTF8(UCharIterator *iter, ثابت حرف *s, int32_t length) {
    إذا (iter != خالی_سوچک) {
        إذا (s != خالی_سوچک && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            إذا(length>=0) {
                iter->حد_بندی=length;
            } وإلا {
                iter->حد_بندی=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->حد_بندی<=1 ? iter->حد_بندی : -1;
        } وإلا {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    إذا(U16_IS_SURROGATE(c)) {
        إذا(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            إذا(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } وإلا {
            إذا(U16_IS_LEAD(c2=iter->پچھلا(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            إذا(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    أرجع c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    إذا(U16_IS_LEAD(c)) {
        إذا(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } وإلا إذا(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    أرجع c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->پچھلا(iter);
    إذا(U16_IS_TRAIL(c)) {
        إذا(U16_IS_LEAD(c2=iter->پچھلا(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } وإلا إذا(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    أرجع c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(ثابت UCharIterator *iter) {
    إذا(iter==خالی_سوچک || iter->getState==خالی_سوچک) {
        أرجع UITER_NO_STATE;
    } وإلا {
        أرجع iter->getState(iter);
    }
}

U_CAPI فراغ U_EXPORT2
uiter_سیٹ_State(UCharIterator *iter, uint32_t state, UError_کوڈ *p_Error_کوڈ) {
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        /* do nothing */
    } وإلا إذا(iter==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
    } وإلا إذا(iter->سیٹ_State==خالی_سوچک) {
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
    } وإلا {
        iter->سیٹ_State(iter, state, p_Error_کوڈ);
    }
}

U_CDECL_سرا
