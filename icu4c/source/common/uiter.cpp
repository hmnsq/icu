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

#تعریف_کړه IS_EVEN(n) (((n)&1)==0)
#تعریف_کړه IS_اشاره_ګر_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

ساكن int32_t U_CALLCONV
noop_Get_شاخص(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    أرجع 0;
}

ساكن int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    أرجع 0;
}

ساكن UBool U_CALLCONV
noop_Has_بل(UCharIterator * /*iter*/) {
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
noop_سېټ_State(UCharIterator * /*iter*/, uint32_t /*state*/, UError_کوډ *p_Error_کوډ) {
    *p_Error_کوډ=U_UNSUPPORTED_ERROR;
}

ساكن ثابت UCharIterator noopIterator={
    تش_اشاره, 0, 0, 0, 0, 0,
    noop_Get_شاخص,
    noopMove,
    noop_Has_بل,
    noop_Has_بل,
    noopCurrent,
    noopCurrent,
    noopCurrent,
    تش_اشاره,
    noopGetState,
    noop_سېټ_State
};

/* UCharIterator implementation for simple strings -------------------------- */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * for char16_t * strings.
 *
 * The UCharIterator.context field holds a pointer to the string.
 */

ساكن int32_t U_CALLCONV
string_Iterator_Get_شاخص(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
        أرجع 0;
    حالة UITER_پیل:
        أرجع iter->پیل;
    حالة UITER_CURRENT:
        أرجع iter->شاخص;
    حالة UITER_حد:
        أرجع iter->حد;
    حالة UITER_اوږدتیا:
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
    حالة UITER_پیل:
        pos=iter->پیل+delta;
        اكسر;
    حالة UITER_CURRENT:
        pos=iter->شاخص+delta;
        اكسر;
    حالة UITER_حد:
        pos=iter->حد+delta;
        اكسر;
    حالة UITER_اوږدتیا:
        pos=iter->length+delta;
        اكسر;
    افتراضي:
        أرجع -1;  /* Error */
    }

    إذا(pos<iter->پیل) {
        pos=iter->پیل;
    } وإلا إذا(pos>iter->حد) {
        pos=iter->حد;
    }

    أرجع iter->شاخص=pos;
}

ساكن UBool U_CALLCONV
string_Iterator_Has_بل(UCharIterator *iter) {
    أرجع iter->شاخص<iter->حد;
}

ساكن UBool U_CALLCONV
string_Iterator_Has_پخوانی(UCharIterator *iter) {
    أرجع iter->شاخص>iter->پیل;
}

ساكن UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    إذا(iter->شاخص<iter->حد) {
        أرجع ((ثابت char16_t *)(iter->context))[iter->شاخص];
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
string_Iterator_بل(UCharIterator *iter) {
    إذا(iter->شاخص<iter->حد) {
        أرجع ((ثابت char16_t *)(iter->context))[iter->شاخص++];
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
string_Iterator_پخوانی(UCharIterator *iter) {
    إذا(iter->شاخص>iter->پیل) {
        أرجع ((ثابت char16_t *)(iter->context))[--iter->شاخص];
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
stringIteratorGetState(ثابت UCharIterator *iter) {
    أرجع (uint32_t)iter->شاخص;
}

ساكن فراغ U_CALLCONV
string_Iterator_سېټ_State(UCharIterator *iter, uint32_t state, UError_کوډ *p_Error_کوډ) {
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        /* do nothing */
    } وإلا إذا(iter==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
    } وإلا إذا((int32_t)state<iter->پیل || iter->حد<(int32_t)state) {
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
    } وإلا {
        iter->شاخص=(int32_t)state;
    }
}

ساكن ثابت UCharIterator stringIterator={
    تش_اشاره, 0, 0, 0, 0, 0,
    string_Iterator_Get_شاخص,
    stringIteratorMove,
    string_Iterator_Has_بل,
    string_Iterator_Has_پخوانی,
    stringIteratorCurrent,
    string_Iterator_بل,
    string_Iterator_پخوانی,
    تش_اشاره,
    stringIteratorGetState,
    string_Iterator_سېټ_State
};

U_CAPI فراغ U_EXPORT2
uiter_سېټ_String(UCharIterator *iter, ثابت char16_t *s, int32_t length) {
    إذا (iter != تش_اشاره) {
        إذا (s != تش_اشاره && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            إذا(length>=0) {
                iter->length=length;
            } وإلا {
                iter->length=u_strlen(s);
            }
            iter->حد=iter->length;
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
utf16BEIteratorGet(UCharIterator *iter, int32_t شاخص) {
    ثابت uint8_t *p=(ثابت uint8_t *)iter->context;
    أرجع ((char16_t)p[2*شاخص]<<8)|(char16_t)p[2*شاخص+1];
}

ساكن UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t شاخص;

    إذا((شاخص=iter->شاخص)<iter->حد) {
        أرجع utf16BEIteratorGet(iter, شاخص);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf16_BEIterator_بل(UCharIterator *iter) {
    int32_t شاخص;

    إذا((شاخص=iter->شاخص)<iter->حد) {
        iter->شاخص=شاخص+1;
        أرجع utf16BEIteratorGet(iter, شاخص);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf16_BEIterator_پخوانی(UCharIterator *iter) {
    int32_t شاخص;

    إذا((شاخص=iter->شاخص)>iter->پیل) {
        iter->شاخص=--شاخص;
        أرجع utf16BEIteratorGet(iter, شاخص);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن ثابت UCharIterator utf16BEIterator={
    تش_اشاره, 0, 0, 0, 0, 0,
    string_Iterator_Get_شاخص,
    stringIteratorMove,
    string_Iterator_Has_بل,
    string_Iterator_Has_پخوانی,
    utf16BEIteratorCurrent,
    utf16_BEIterator_بل,
    utf16_BEIterator_پخوانی,
    تش_اشاره,
    stringIteratorGetState,
    string_Iterator_سېټ_State
};

/*
 * Count the number of UChars in a UTF-16BE string before a terminating char16_t NUL,
 * i.e., before a pair of 0 bytes where the first 0 byte is at an even
 * offset from s.
 */
ساكن int32_t
utf16BE_strlen(ثابت حرف *s) {
    إذا(IS_اشاره_ګر_EVEN(s)) {
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
uiter_سېټ_UTF16_BE(UCharIterator *iter, ثابت حرف *s, int32_t length) {
    إذا(iter!=تش_اشاره) {
        /* allow only even-length strings (the input length counts bytes) */
        إذا(s!=تش_اشاره && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            إذا(U_IS_BIG_ENDIAN && IS_اشاره_ګر_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_سېټ_String(iter, (ثابت char16_t *)s, length);
                أرجع;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            إذا(length>=0) {
                iter->length=length;
            } وإلا {
                iter->length=utf16BE_strlen(s);
            }
            iter->حد=iter->length;
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
character_Iterator_Get_شاخص(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
        أرجع 0;
    حالة UITER_پیل:
        أرجع ((CharacterIterator *)(iter->context))->پیل_شاخص();
    حالة UITER_CURRENT:
        أرجع ((CharacterIterator *)(iter->context))->get_شاخص();
    حالة UITER_حد:
        أرجع ((CharacterIterator *)(iter->context))->پای_ټکی_شاخص();
    حالة UITER_اوږدتیا:
        أرجع ((CharacterIterator *)(iter->context))->get_اوږدتیا();
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
        ((CharacterIterator *)(iter->context))->سېټ_شاخص(delta);
        أرجع ((CharacterIterator *)(iter->context))->get_شاخص();
    حالة UITER_پیل:
    حالة UITER_CURRENT:
    حالة UITER_حد:
        أرجع ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    حالة UITER_اوږدتیا:
        ((CharacterIterator *)(iter->context))->سېټ_شاخص(((CharacterIterator *)(iter->context))->get_اوږدتیا()+delta);
        أرجع ((CharacterIterator *)(iter->context))->get_شاخص();
    افتراضي:
        /* not a valid origin */
        /* Should never get here! */
        أرجع -1;
    }
}

ساكن UBool U_CALLCONV
character_Iterator_Has_بل(UCharIterator *iter) {
    أرجع ((CharacterIterator *)(iter->context))->has_بل();
}

ساكن UBool U_CALLCONV
character_Iterator_Has_پخوانی(UCharIterator *iter) {
    أرجع ((CharacterIterator *)(iter->context))->has_پخوانی();
}

ساكن UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    إذا(c!=0xffff || ((CharacterIterator *)(iter->context))->has_بل()) {
        أرجع c;
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
character_Iterator_بل(UCharIterator *iter) {
    إذا(((CharacterIterator *)(iter->context))->has_بل()) {
        أرجع ((CharacterIterator *)(iter->context))->بل_Post_Inc();
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
character_Iterator_پخوانی(UCharIterator *iter) {
    إذا(((CharacterIterator *)(iter->context))->has_پخوانی()) {
        أرجع ((CharacterIterator *)(iter->context))->پخوانی();
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
characterIteratorGetState(ثابت UCharIterator *iter) {
    أرجع ((CharacterIterator *)(iter->context))->get_شاخص();
}

ساكن فراغ U_CALLCONV
character_Iterator_سېټ_State(UCharIterator *iter, uint32_t state, UError_کوډ *p_Error_کوډ) {
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        /* do nothing */
    } وإلا إذا(iter==تش_اشاره || iter->context==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
    } وإلا إذا((int32_t)state<((CharacterIterator *)(iter->context))->پیل_شاخص() || ((CharacterIterator *)(iter->context))->پای_ټکی_شاخص()<(int32_t)state) {
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
    } وإلا {
        ((CharacterIterator *)(iter->context))->سېټ_شاخص((int32_t)state);
    }
}

ساكن ثابت UCharIterator characterIteratorWrapper={
    تش_اشاره, 0, 0, 0, 0, 0,
    character_Iterator_Get_شاخص,
    characterIteratorMove,
    character_Iterator_Has_بل,
    character_Iterator_Has_پخوانی,
    characterIteratorCurrent,
    character_Iterator_بل,
    character_Iterator_پخوانی,
    تش_اشاره,
    characterIteratorGetState,
    character_Iterator_سېټ_State
};

U_CAPI فراغ U_EXPORT2
uiter_سېټ_Character_Iterator(UCharIterator *iter, CharacterIterator *charIter) {
    إذا (iter != تش_اشاره) {
        إذا (charIter != تش_اشاره) {
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
    إذا(iter->شاخص<iter->حد) {
        أرجع ((Replaceable *)(iter->context))->charAt(iter->شاخص);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
replaceable_Iterator_بل(UCharIterator *iter) {
    إذا(iter->شاخص<iter->حد) {
        أرجع ((Replaceable *)(iter->context))->charAt(iter->شاخص++);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
replaceable_Iterator_پخوانی(UCharIterator *iter) {
    إذا(iter->شاخص>iter->پیل) {
        أرجع ((Replaceable *)(iter->context))->charAt(--iter->شاخص);
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن ثابت UCharIterator replaceableIterator={
    تش_اشاره, 0, 0, 0, 0, 0,
    string_Iterator_Get_شاخص,
    stringIteratorMove,
    string_Iterator_Has_بل,
    string_Iterator_Has_پخوانی,
    replaceableIteratorCurrent,
    replaceable_Iterator_بل,
    replaceable_Iterator_پخوانی,
    تش_اشاره,
    stringIteratorGetState,
    string_Iterator_سېټ_State
};

U_CAPI فراغ U_EXPORT2
uiter_سېټ_Replaceable(UCharIterator *iter, ثابت Replaceable *rep) {
    إذا (iter != تش_اشاره) {
        إذا (rep != تش_اشاره) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->حد=iter->length=rep->length();
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

#تعریف_کړه UITER_CNV_ظرفیت 16

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
utf8_Iterator_Get_شاخص(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    بدل(origin) {
    حالة UITER_ZERO:
    حالة UITER_پیل:
        أرجع 0;
    حالة UITER_CURRENT:
        إذا(iter->شاخص<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            ثابت uint8_t *s;
            UChar32 c;
            int32_t i, حد, شاخص;

            s=(ثابت uint8_t *)iter->context;
            i=شاخص=0;
            حد=iter->پیل; /* count up to the UTF-8 index */
            طالما(i<حد) {
                U8_بل_OR_FFFD(s, i, حد, c);
                شاخص+=U16_اوږدتیا(c);
            }

            iter->پیل=i; /* just in case setState() did not get us to a code point boundary */
            إذا(i==iter->حد) {
                iter->length=شاخص; /* in case it was <0 or wrong */
            }
            إذا(iter->reserved_ډګر!=0) {
                --شاخص; /* we are in the middle of a supplementary code point */
            }
            iter->شاخص=شاخص;
        }
        أرجع iter->شاخص;
    حالة UITER_حد:
    حالة UITER_اوږدتیا:
        إذا(iter->length<0) {
            ثابت uint8_t *s;
            UChar32 c;
            int32_t i, حد, length;

            s=(ثابت uint8_t *)iter->context;
            إذا(iter->شاخص<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                حد=iter->پیل;

                /* count from the beginning to the current index */
                طالما(i<حد) {
                    U8_بل_OR_FFFD(s, i, حد, c);
                    length+=U16_اوږدتیا(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->پیل=i; /* just in case setState() did not get us to a code point boundary */
                iter->شاخص= iter->reserved_ډګر!=0 ? length-1 : length;
            } وإلا {
                i=iter->پیل;
                length=iter->شاخص;
                إذا(iter->reserved_ډګر!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            حد=iter->حد;
            طالما(i<حد) {
                U8_بل_OR_FFFD(s, i, حد, c);
                length+=U16_اوږدتیا(c);
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
    حالة UITER_پیل:
        pos=delta;
        havePos=ریښتیا;
        /* iter->index<0 (unknown) is possible */
        اكسر;
    حالة UITER_CURRENT:
        إذا(iter->شاخص>=0) {
            pos=iter->شاخص+delta;
            havePos=ریښتیا;
        } وإلا {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=خطأ;
        }
        اكسر;
    حالة UITER_حد:
    حالة UITER_اوږدتیا:
        إذا(iter->length>=0) {
            pos=iter->length+delta;
            havePos=ریښتیا;
        } وإلا {
            /* pin to the end, avoid counting the length */
            iter->شاخص=-1;
            iter->پیل=iter->حد;
            iter->reserved_ډګر=0;
            إذا(delta>=0) {
                أرجع UITER_UNKNOWN_شاخص;
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
            iter->شاخص=iter->پیل=iter->reserved_ډګر=0;
            أرجع 0;
        } وإلا إذا(iter->length>=0 && pos>=iter->length) {
            iter->شاخص=iter->length;
            iter->پیل=iter->حد;
            iter->reserved_ډګر=0;
            أرجع iter->شاخص;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        إذا(iter->شاخص<0 || pos<iter->شاخص/2) {
            /* go forward from the start instead of backward from the current index */
            iter->شاخص=iter->پیل=iter->reserved_ډګر=0;
        } وإلا إذا(iter->length>=0 && (iter->length-pos)<(pos-iter->شاخص)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->شاخص=iter->length;
            iter->پیل=iter->حد;
            iter->reserved_ډګر=0;
        }

        delta=pos-iter->شاخص;
        إذا(delta==0) {
            أرجع iter->شاخص; /* nothing to do */
        }
    } وإلا {
        /* move relative to unknown UTF-16 index */
        إذا(delta==0) {
            أرجع UITER_UNKNOWN_شاخص; /* nothing to do */
        } وإلا إذا(-delta>=iter->پیل) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->شاخص=iter->پیل=iter->reserved_ډګر=0;
            أرجع 0;
        } وإلا إذا(delta>=(iter->حد-iter->پیل)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->شاخص=iter->length; /* may or may not be <0 (unknown) */
            iter->پیل=iter->حد;
            iter->reserved_ډګر=0;
            أرجع iter->شاخص>=0 ? iter->شاخص : (int32_t)UITER_UNKNOWN_شاخص;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(ثابت uint8_t *)iter->context;
    pos=iter->شاخص; /* could be <0 (unknown) */
    i=iter->پیل;
    إذا(delta>0) {
        /* go forward */
        int32_t حد=iter->حد;
        إذا(iter->reserved_ډګر!=0) {
            iter->reserved_ډګر=0;
            ++pos;
            --delta;
        }
        طالما(delta>0 && i<حد) {
            U8_بل_OR_FFFD(s, i, حد, c);
            إذا(c<=0xffff) {
                ++pos;
                --delta;
            } وإلا إذا(delta>=2) {
                pos+=2;
                delta-=2;
            } وإلا /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reserved_ډګر=c;
                ++pos;
                اكسر; /* delta=0; */
            }
        }
        إذا(i==حد) {
            إذا(iter->length<0 && iter->شاخص>=0) {
                iter->length= iter->reserved_ډګر==0 ? pos : pos+1;
            } وإلا إذا(iter->شاخص<0 && iter->length>=0) {
                iter->شاخص= iter->reserved_ډګر==0 ? iter->length : iter->length-1;
            }
        }
    } وإلا /* delta<0 */ {
        /* go backward */
        إذا(iter->reserved_ډګر!=0) {
            iter->reserved_ډګر=0;
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
                iter->reserved_ډګر=c;
                --pos;
                اكسر; /* delta=0; */
            }
        }
    }

    iter->پیل=i;
    إذا(iter->شاخص>=0) {
        أرجع iter->شاخص=pos;
    } وإلا {
        /* we started with index<0 (unknown) so pos is bogus */
        إذا(i<=1) {
            أرجع iter->شاخص=i; /* reached the beginning */
        } وإلا {
            /* we still don't know the UTF-16 index */
            أرجع UITER_UNKNOWN_شاخص;
        }
    }
}

ساكن UBool U_CALLCONV
utf8_Iterator_Has_بل(UCharIterator *iter) {
    أرجع iter->پیل<iter->حد || iter->reserved_ډګر!=0;
}

ساكن UBool U_CALLCONV
utf8_Iterator_Has_پخوانی(UCharIterator *iter) {
    أرجع iter->پیل>0;
}

ساكن UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    إذا(iter->reserved_ډګر!=0) {
        أرجع U16_TRAIL(iter->reserved_ډګر);
    } وإلا إذا(iter->پیل<iter->حد) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->پیل;

        U8_بل_OR_FFFD(s, i, iter->حد, c);
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
utf8_Iterator_بل(UCharIterator *iter) {
    int32_t شاخص;

    إذا(iter->reserved_ډګر!=0) {
        char16_t trail=U16_TRAIL(iter->reserved_ډګر);
        iter->reserved_ډګر=0;
        إذا((شاخص=iter->شاخص)>=0) {
            iter->شاخص=شاخص+1;
        }
        أرجع trail;
    } وإلا إذا(iter->پیل<iter->حد) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;

        U8_بل_OR_FFFD(s, iter->پیل, iter->حد, c);
        إذا((شاخص=iter->شاخص)>=0) {
            iter->شاخص=++شاخص;
            إذا(iter->length<0 && iter->پیل==iter->حد) {
                iter->length= c<=0xffff ? شاخص : شاخص+1;
            }
        } وإلا إذا(iter->پیل==iter->حد && iter->length>=0) {
            iter->شاخص= c<=0xffff ? iter->length : iter->length-1;
        }
        إذا(c<=0xffff) {
            أرجع c;
        } وإلا {
            iter->reserved_ډګر=c;
            أرجع U16_LEAD(c);
        }
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن UChar32 U_CALLCONV
utf8_Iterator_پخوانی(UCharIterator *iter) {
    int32_t شاخص;

    إذا(iter->reserved_ډګر!=0) {
        char16_t lead=U16_LEAD(iter->reserved_ډګر);
        iter->reserved_ډګر=0;
        iter->پیل-=4; /* we stayed behind the supplementary code point; go before it now */
        إذا((شاخص=iter->شاخص)>0) {
            iter->شاخص=شاخص-1;
        }
        أرجع lead;
    } وإلا إذا(iter->پیل>0) {
        ثابت uint8_t *s=(ثابت uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->پیل, c);
        إذا((شاخص=iter->شاخص)>0) {
            iter->شاخص=شاخص-1;
        } وإلا إذا(iter->پیل<=1) {
            iter->شاخص= c<=0xffff ? iter->پیل : iter->پیل+1;
        }
        إذا(c<=0xffff) {
            أرجع c;
        } وإلا {
            iter->پیل+=4; /* back to behind this supplementary code point for consistent state */
            iter->reserved_ډګر=c;
            أرجع U16_TRAIL(c);
        }
    } وإلا {
        أرجع U_SENTINEL;
    }
}

ساكن uint32_t U_CALLCONV
utf8IteratorGetState(ثابت UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->پیل<<1);
    إذا(iter->reserved_ډګر!=0) {
        state|=1;
    }
    أرجع state;
}

ساكن فراغ U_CALLCONV
utf8_Iterator_سېټ_State(UCharIterator *iter,
                     uint32_t state,
                     UError_کوډ *p_Error_کوډ)
{
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        /* do nothing */
    } وإلا إذا(iter==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
    } وإلا إذا(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } وإلا {
        int32_t شاخص=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        إذا((state==0 ? شاخص<0 : شاخص<4) || iter->حد<شاخص) {
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        } وإلا {
            iter->پیل=شاخص; /* restore UTF-8 byte index */
            إذا(شاخص<=1) {
                iter->شاخص=شاخص;
            } وإلا {
                iter->شاخص=-1; /* unknown UTF-16 index */
            }
            إذا(state==0) {
                iter->reserved_ډګر=0;
            } وإلا {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((ثابت uint8_t *)iter->context, 0, شاخص, c);
                إذا(c<=0xffff) {
                    *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                } وإلا {
                    iter->reserved_ډګر=c;
                }
            }
        }
    }
}

ساكن ثابت UCharIterator utf8Iterator={
    تش_اشاره, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_شاخص,
    utf8IteratorMove,
    utf8_Iterator_Has_بل,
    utf8_Iterator_Has_پخوانی,
    utf8IteratorCurrent,
    utf8_Iterator_بل,
    utf8_Iterator_پخوانی,
    تش_اشاره,
    utf8IteratorGetState,
    utf8_Iterator_سېټ_State
};

U_CAPI فراغ U_EXPORT2
uiter_سېټ_UTF8(UCharIterator *iter, ثابت حرف *s, int32_t length) {
    إذا (iter != تش_اشاره) {
        إذا (s != تش_اشاره && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            إذا(length>=0) {
                iter->حد=length;
            } وإلا {
                iter->حد=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->حد<=1 ? iter->حد : -1;
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
            إذا(U16_IS_LEAD(c2=iter->پخوانی(iter))) {
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

    c=iter->پخوانی(iter);
    إذا(U16_IS_TRAIL(c)) {
        إذا(U16_IS_LEAD(c2=iter->پخوانی(iter))) {
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
    إذا(iter==تش_اشاره || iter->getState==تش_اشاره) {
        أرجع UITER_NO_STATE;
    } وإلا {
        أرجع iter->getState(iter);
    }
}

U_CAPI فراغ U_EXPORT2
uiter_سېټ_State(UCharIterator *iter, uint32_t state, UError_کوډ *p_Error_کوډ) {
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        /* do nothing */
    } وإلا إذا(iter==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
    } وإلا إذا(iter->سېټ_State==تش_اشاره) {
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
    } وإلا {
        iter->سېټ_State(iter, state, p_Error_کوډ);
    }
}

U_CDECL_پای_ټکی
