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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/chariter.h"
#అన్తర్భావ "unicode/rep.h"
#అన్తర్భావ "unicode/uiter.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cstring.h"

U_NAMESPACE_USE

#నిర్వచించు IS_EVEN(n) (((n)&1)==0)
#నిర్వచించు IS_సూచిక_EVEN(p) IS_EVEN((size_t)p)

U_CDECL_BEGIN

/* No-Op UCharIterator implementation for illegal input --------------------- */

స్థిర int32_t U_CALLCONV
noop_Get_సూచీ(UCharIterator * /*iter*/, UCharIteratorOrigin /*origin*/) {
    తిరిగి 0;
}

స్థిర int32_t U_CALLCONV
noopMove(UCharIterator * /*iter*/, int32_t /*delta*/, UCharIteratorOrigin /*origin*/) {
    తిరిగి 0;
}

స్థిర UBool U_CALLCONV
noop_Has_తదుపరి(UCharIterator * /*iter*/) {
    తిరిగి అబద్ధం;
}

స్థిర UChar32 U_CALLCONV
noopCurrent(UCharIterator * /*iter*/) {
    తిరిగి U_SENTINEL;
}

స్థిర uint32_t U_CALLCONV
noopGetState(స్థిరరాశి UCharIterator * /*iter*/) {
    తిరిగి UITER_NO_STATE;
}

స్థిర శూన్య U_CALLCONV
noop_సమితి_State(UCharIterator * /*iter*/, uint32_t /*state*/, UError_సంకేతం *p_Error_సంకేతం) {
    *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
}

స్థిర స్థిరరాశి UCharIterator noopIterator={
    శూన్యసూచిక, 0, 0, 0, 0, 0,
    noop_Get_సూచీ,
    noopMove,
    noop_Has_తదుపరి,
    noop_Has_తదుపరి,
    noopCurrent,
    noopCurrent,
    noopCurrent,
    శూన్యసూచిక,
    noopGetState,
    noop_సమితి_State
};

/* UCharIterator implementation for simple strings -------------------------- */

/*
 * This is an implementation of a code unit (char16_t) iterator
 * for char16_t * strings.
 *
 * The UCharIterator.context field holds a pointer to the string.
 */

స్థిర int32_t U_CALLCONV
string_Iterator_Get_సూచీ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ఎంపిక(origin) {
    స్థితి UITER_ZERO:
        తిరిగి 0;
    స్థితి UITER_ప్రారంభం:
        తిరిగి iter->ప్రారంభం;
    స్థితి UITER_CURRENT:
        తిరిగి iter->సూచీ;
    స్థితి UITER_హద్దు:
        తిరిగి iter->హద్దు;
    స్థితి UITER_నిడివి:
        తిరిగి iter->length;
    అప్రమేయ:
        /* not a valid origin */
        /* Should never get here! */
        తిరిగి -1;
    }
}

స్థిర int32_t U_CALLCONV
stringIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    int32_t pos;

    ఎంపిక(origin) {
    స్థితి UITER_ZERO:
        pos=delta;
        ఆపు;
    స్థితి UITER_ప్రారంభం:
        pos=iter->ప్రారంభం+delta;
        ఆపు;
    స్థితి UITER_CURRENT:
        pos=iter->సూచీ+delta;
        ఆపు;
    స్థితి UITER_హద్దు:
        pos=iter->హద్దు+delta;
        ఆపు;
    స్థితి UITER_నిడివి:
        pos=iter->length+delta;
        ఆపు;
    అప్రమేయ:
        తిరిగి -1;  /* Error */
    }

    ఒకవేళ(pos<iter->ప్రారంభం) {
        pos=iter->ప్రారంభం;
    } లేకపోతే ఒకవేళ(pos>iter->హద్దు) {
        pos=iter->హద్దు;
    }

    తిరిగి iter->సూచీ=pos;
}

స్థిర UBool U_CALLCONV
string_Iterator_Has_తదుపరి(UCharIterator *iter) {
    తిరిగి iter->సూచీ<iter->హద్దు;
}

స్థిర UBool U_CALLCONV
string_Iterator_Has_మునుపటి(UCharIterator *iter) {
    తిరిగి iter->సూచీ>iter->ప్రారంభం;
}

స్థిర UChar32 U_CALLCONV
stringIteratorCurrent(UCharIterator *iter) {
    ఒకవేళ(iter->సూచీ<iter->హద్దు) {
        తిరిగి ((స్థిరరాశి char16_t *)(iter->context))[iter->సూచీ];
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
string_Iterator_తదుపరి(UCharIterator *iter) {
    ఒకవేళ(iter->సూచీ<iter->హద్దు) {
        తిరిగి ((స్థిరరాశి char16_t *)(iter->context))[iter->సూచీ++];
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
string_Iterator_మునుపటి(UCharIterator *iter) {
    ఒకవేళ(iter->సూచీ>iter->ప్రారంభం) {
        తిరిగి ((స్థిరరాశి char16_t *)(iter->context))[--iter->సూచీ];
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర uint32_t U_CALLCONV
stringIteratorGetState(స్థిరరాశి UCharIterator *iter) {
    తిరిగి (uint32_t)iter->సూచీ;
}

స్థిర శూన్య U_CALLCONV
string_Iterator_సమితి_State(UCharIterator *iter, uint32_t state, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        /* do nothing */
    } లేకపోతే ఒకవేళ(iter==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    } లేకపోతే ఒకవేళ((int32_t)state<iter->ప్రారంభం || iter->హద్దు<(int32_t)state) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
    } లేకపోతే {
        iter->సూచీ=(int32_t)state;
    }
}

స్థిర స్థిరరాశి UCharIterator stringIterator={
    శూన్యసూచిక, 0, 0, 0, 0, 0,
    string_Iterator_Get_సూచీ,
    stringIteratorMove,
    string_Iterator_Has_తదుపరి,
    string_Iterator_Has_మునుపటి,
    stringIteratorCurrent,
    string_Iterator_తదుపరి,
    string_Iterator_మునుపటి,
    శూన్యసూచిక,
    stringIteratorGetState,
    string_Iterator_సమితి_State
};

U_CAPI శూన్య U_EXPORT2
uiter_సమితి_String(UCharIterator *iter, స్థిరరాశి char16_t *s, int32_t length) {
    ఒకవేళ (iter != శూన్యసూచిక) {
        ఒకవేళ (s != శూన్యసూచిక && length >= -1) {
            *iter=stringIterator;
            iter->context=s;
            ఒకవేళ(length>=0) {
                iter->length=length;
            } లేకపోతే {
                iter->length=u_strlen(s);
            }
            iter->హద్దు=iter->length;
        } లేకపోతే {
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
స్థిర అంతఃవరుస UChar32
utf16BEIteratorGet(UCharIterator *iter, int32_t సూచీ) {
    స్థిరరాశి uint8_t *p=(స్థిరరాశి uint8_t *)iter->context;
    తిరిగి ((char16_t)p[2*సూచీ]<<8)|(char16_t)p[2*సూచీ+1];
}

స్థిర UChar32 U_CALLCONV
utf16BEIteratorCurrent(UCharIterator *iter) {
    int32_t సూచీ;

    ఒకవేళ((సూచీ=iter->సూచీ)<iter->హద్దు) {
        తిరిగి utf16BEIteratorGet(iter, సూచీ);
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
utf16_BEIterator_తదుపరి(UCharIterator *iter) {
    int32_t సూచీ;

    ఒకవేళ((సూచీ=iter->సూచీ)<iter->హద్దు) {
        iter->సూచీ=సూచీ+1;
        తిరిగి utf16BEIteratorGet(iter, సూచీ);
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
utf16_BEIterator_మునుపటి(UCharIterator *iter) {
    int32_t సూచీ;

    ఒకవేళ((సూచీ=iter->సూచీ)>iter->ప్రారంభం) {
        iter->సూచీ=--సూచీ;
        తిరిగి utf16BEIteratorGet(iter, సూచీ);
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర స్థిరరాశి UCharIterator utf16BEIterator={
    శూన్యసూచిక, 0, 0, 0, 0, 0,
    string_Iterator_Get_సూచీ,
    stringIteratorMove,
    string_Iterator_Has_తదుపరి,
    string_Iterator_Has_మునుపటి,
    utf16BEIteratorCurrent,
    utf16_BEIterator_తదుపరి,
    utf16_BEIterator_మునుపటి,
    శూన్యసూచిక,
    stringIteratorGetState,
    string_Iterator_సమితి_State
};

/*
 * Count the number of UChars in a UTF-16BE string before a terminating char16_t NUL,
 * i.e., before a pair of 0 bytes where the first 0 byte is at an even
 * offset from s.
 */
స్థిర int32_t
utf16BE_strlen(స్థిరరాశి అక్షరం *s) {
    ఒకవేళ(IS_సూచిక_EVEN(s)) {
        /*
         * even-aligned, call u_strlen(s)
         * we are probably on a little-endian machine, but searching for char16_t NUL
         * does not care about endianness
         */
        తిరిగి u_strlen((స్థిరరాశి char16_t *)s);
    } లేకపోతే {
        /* odd-aligned, search for pair of 0 bytes */
        స్థిరరాశి అక్షరం *p=s;

        అయితే(!(*p==0 && p[1]==0)) {
            p+=2;
        }
        తిరిగి (int32_t)((p-s)/2);
    }
}

U_CAPI శూన్య U_EXPORT2
uiter_సమితి_UTF16_BE(UCharIterator *iter, స్థిరరాశి అక్షరం *s, int32_t length) {
    ఒకవేళ(iter!=శూన్యసూచిక) {
        /* allow only even-length strings (the input length counts bytes) */
        ఒకవేళ(s!=శూన్యసూచిక && (length==-1 || (length>=0 && IS_EVEN(length)))) {
            /* length/=2, except that >>=1 also works for -1 (-1/2==0, -1>>1==-1) */
            length>>=1;

            ఒకవేళ(U_IS_BIG_ENDIAN && IS_సూచిక_EVEN(s)) {
                /* big-endian machine and 2-aligned UTF-16BE string: use normal char16_t iterator */
                uiter_సమితి_String(iter, (స్థిరరాశి char16_t *)s, length);
                తిరిగి;
            }

            *iter=utf16BEIterator;
            iter->context=s;
            ఒకవేళ(length>=0) {
                iter->length=length;
            } లేకపోతే {
                iter->length=utf16BE_strlen(s);
            }
            iter->హద్దు=iter->length;
        } లేకపోతే {
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

స్థిర int32_t U_CALLCONV
character_Iterator_Get_సూచీ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ఎంపిక(origin) {
    స్థితి UITER_ZERO:
        తిరిగి 0;
    స్థితి UITER_ప్రారంభం:
        తిరిగి ((CharacterIterator *)(iter->context))->ప్రారంభం_సూచీ();
    స్థితి UITER_CURRENT:
        తిరిగి ((CharacterIterator *)(iter->context))->get_సూచీ();
    స్థితి UITER_హద్దు:
        తిరిగి ((CharacterIterator *)(iter->context))->చివర_సూచీ();
    స్థితి UITER_నిడివి:
        తిరిగి ((CharacterIterator *)(iter->context))->get_నిడివి();
    అప్రమేయ:
        /* not a valid origin */
        /* Should never get here! */
        తిరిగి -1;
    }
}

స్థిర int32_t U_CALLCONV
characterIteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ఎంపిక(origin) {
    స్థితి UITER_ZERO:
        ((CharacterIterator *)(iter->context))->సమితి_సూచీ(delta);
        తిరిగి ((CharacterIterator *)(iter->context))->get_సూచీ();
    స్థితి UITER_ప్రారంభం:
    స్థితి UITER_CURRENT:
    స్థితి UITER_హద్దు:
        తిరిగి ((CharacterIterator *)(iter->context))->move(delta, (CharacterIterator::EOrigin)origin);
    స్థితి UITER_నిడివి:
        ((CharacterIterator *)(iter->context))->సమితి_సూచీ(((CharacterIterator *)(iter->context))->get_నిడివి()+delta);
        తిరిగి ((CharacterIterator *)(iter->context))->get_సూచీ();
    అప్రమేయ:
        /* not a valid origin */
        /* Should never get here! */
        తిరిగి -1;
    }
}

స్థిర UBool U_CALLCONV
character_Iterator_Has_తదుపరి(UCharIterator *iter) {
    తిరిగి ((CharacterIterator *)(iter->context))->has_తదుపరి();
}

స్థిర UBool U_CALLCONV
character_Iterator_Has_మునుపటి(UCharIterator *iter) {
    తిరిగి ((CharacterIterator *)(iter->context))->has_మునుపటి();
}

స్థిర UChar32 U_CALLCONV
characterIteratorCurrent(UCharIterator *iter) {
    UChar32 c;

    c=((CharacterIterator *)(iter->context))->current();
    ఒకవేళ(c!=0xffff || ((CharacterIterator *)(iter->context))->has_తదుపరి()) {
        తిరిగి c;
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
character_Iterator_తదుపరి(UCharIterator *iter) {
    ఒకవేళ(((CharacterIterator *)(iter->context))->has_తదుపరి()) {
        తిరిగి ((CharacterIterator *)(iter->context))->తదుపరి_Post_Inc();
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
character_Iterator_మునుపటి(UCharIterator *iter) {
    ఒకవేళ(((CharacterIterator *)(iter->context))->has_మునుపటి()) {
        తిరిగి ((CharacterIterator *)(iter->context))->మునుపటి();
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర uint32_t U_CALLCONV
characterIteratorGetState(స్థిరరాశి UCharIterator *iter) {
    తిరిగి ((CharacterIterator *)(iter->context))->get_సూచీ();
}

స్థిర శూన్య U_CALLCONV
character_Iterator_సమితి_State(UCharIterator *iter, uint32_t state, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        /* do nothing */
    } లేకపోతే ఒకవేళ(iter==శూన్యసూచిక || iter->context==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    } లేకపోతే ఒకవేళ((int32_t)state<((CharacterIterator *)(iter->context))->ప్రారంభం_సూచీ() || ((CharacterIterator *)(iter->context))->చివర_సూచీ()<(int32_t)state) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
    } లేకపోతే {
        ((CharacterIterator *)(iter->context))->సమితి_సూచీ((int32_t)state);
    }
}

స్థిర స్థిరరాశి UCharIterator characterIteratorWrapper={
    శూన్యసూచిక, 0, 0, 0, 0, 0,
    character_Iterator_Get_సూచీ,
    characterIteratorMove,
    character_Iterator_Has_తదుపరి,
    character_Iterator_Has_మునుపటి,
    characterIteratorCurrent,
    character_Iterator_తదుపరి,
    character_Iterator_మునుపటి,
    శూన్యసూచిక,
    characterIteratorGetState,
    character_Iterator_సమితి_State
};

U_CAPI శూన్య U_EXPORT2
uiter_సమితి_Character_Iterator(UCharIterator *iter, CharacterIterator *charIter) {
    ఒకవేళ (iter != శూన్యసూచిక) {
        ఒకవేళ (charIter != శూన్యసూచిక) {
            *iter=characterIteratorWrapper;
            iter->context=charIter;
        } లేకపోతే {
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

స్థిర UChar32 U_CALLCONV
replaceableIteratorCurrent(UCharIterator *iter) {
    ఒకవేళ(iter->సూచీ<iter->హద్దు) {
        తిరిగి ((Replaceable *)(iter->context))->charAt(iter->సూచీ);
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
replaceable_Iterator_తదుపరి(UCharIterator *iter) {
    ఒకవేళ(iter->సూచీ<iter->హద్దు) {
        తిరిగి ((Replaceable *)(iter->context))->charAt(iter->సూచీ++);
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
replaceable_Iterator_మునుపటి(UCharIterator *iter) {
    ఒకవేళ(iter->సూచీ>iter->ప్రారంభం) {
        తిరిగి ((Replaceable *)(iter->context))->charAt(--iter->సూచీ);
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర స్థిరరాశి UCharIterator replaceableIterator={
    శూన్యసూచిక, 0, 0, 0, 0, 0,
    string_Iterator_Get_సూచీ,
    stringIteratorMove,
    string_Iterator_Has_తదుపరి,
    string_Iterator_Has_మునుపటి,
    replaceableIteratorCurrent,
    replaceable_Iterator_తదుపరి,
    replaceable_Iterator_మునుపటి,
    శూన్యసూచిక,
    stringIteratorGetState,
    string_Iterator_సమితి_State
};

U_CAPI శూన్య U_EXPORT2
uiter_సమితి_Replaceable(UCharIterator *iter, స్థిరరాశి Replaceable *rep) {
    ఒకవేళ (iter != శూన్యసూచిక) {
        ఒకవేళ (rep != శూన్యసూచిక) {
            *iter=replaceableIterator;
            iter->context=rep;
            iter->హద్దు=iter->length=rep->length();
        } లేకపోతే {
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

#నిర్వచించు UITER_CNV_సామర్థ్యం 16

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

స్థిర int32_t U_CALLCONV
utf8_Iterator_Get_సూచీ(UCharIterator *iter, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    ఎంపిక(origin) {
    స్థితి UITER_ZERO:
    స్థితి UITER_ప్రారంభం:
        తిరిగి 0;
    స్థితి UITER_CURRENT:
        ఒకవేళ(iter->సూచీ<0) {
            /* the current UTF-16 index is unknown after setState(), count from the beginning */
            స్థిరరాశి uint8_t *s;
            UChar32 c;
            int32_t i, హద్దు, సూచీ;

            s=(స్థిరరాశి uint8_t *)iter->context;
            i=సూచీ=0;
            హద్దు=iter->ప్రారంభం; /* count up to the UTF-8 index */
            అయితే(i<హద్దు) {
                U8_తదుపరి_OR_FFFD(s, i, హద్దు, c);
                సూచీ+=U16_నిడివి(c);
            }

            iter->ప్రారంభం=i; /* just in case setState() did not get us to a code point boundary */
            ఒకవేళ(i==iter->హద్దు) {
                iter->length=సూచీ; /* in case it was <0 or wrong */
            }
            ఒకవేళ(iter->reserved_క్షేత్రం!=0) {
                --సూచీ; /* we are in the middle of a supplementary code point */
            }
            iter->సూచీ=సూచీ;
        }
        తిరిగి iter->సూచీ;
    స్థితి UITER_హద్దు:
    స్థితి UITER_నిడివి:
        ఒకవేళ(iter->length<0) {
            స్థిరరాశి uint8_t *s;
            UChar32 c;
            int32_t i, హద్దు, length;

            s=(స్థిరరాశి uint8_t *)iter->context;
            ఒకవేళ(iter->సూచీ<0) {
                /*
                 * the current UTF-16 index is unknown after setState(),
                 * we must first count from the beginning to here
                 */
                i=length=0;
                హద్దు=iter->ప్రారంభం;

                /* count from the beginning to the current index */
                అయితే(i<హద్దు) {
                    U8_తదుపరి_OR_FFFD(s, i, హద్దు, c);
                    length+=U16_నిడివి(c);
                }

                /* assume i==limit==iter->start, set the UTF-16 index */
                iter->ప్రారంభం=i; /* just in case setState() did not get us to a code point boundary */
                iter->సూచీ= iter->reserved_క్షేత్రం!=0 ? length-1 : length;
            } లేకపోతే {
                i=iter->ప్రారంభం;
                length=iter->సూచీ;
                ఒకవేళ(iter->reserved_క్షేత్రం!=0) {
                    ++length;
                }
            }

            /* count from the current index to the end */
            హద్దు=iter->హద్దు;
            అయితే(i<హద్దు) {
                U8_తదుపరి_OR_FFFD(s, i, హద్దు, c);
                length+=U16_నిడివి(c);
            }
            iter->length=length;
        }
        తిరిగి iter->length;
    అప్రమేయ:
        /* not a valid origin */
        /* Should never get here! */
        తిరిగి -1;
    }
}

స్థిర int32_t U_CALLCONV
utf8IteratorMove(UCharIterator *iter, int32_t delta, UCharIteratorOrigin origin) UPRV_NO_SANITIZE_UNDEFINED {
    స్థిరరాశి uint8_t *s;
    UChar32 c;
    int32_t pos; /* requested UTF-16 index */
    int32_t i; /* UTF-8 index */
    UBool havePos;

    /* calculate the requested UTF-16 index */
    ఎంపిక(origin) {
    స్థితి UITER_ZERO:
    స్థితి UITER_ప్రారంభం:
        pos=delta;
        havePos=నిజం;
        /* iter->index<0 (unknown) is possible */
        ఆపు;
    స్థితి UITER_CURRENT:
        ఒకవేళ(iter->సూచీ>=0) {
            pos=iter->సూచీ+delta;
            havePos=నిజం;
        } లేకపోతే {
            /* the current UTF-16 index is unknown after setState(), use only delta */
            pos=0;
            havePos=అబద్ధం;
        }
        ఆపు;
    స్థితి UITER_హద్దు:
    స్థితి UITER_నిడివి:
        ఒకవేళ(iter->length>=0) {
            pos=iter->length+delta;
            havePos=నిజం;
        } లేకపోతే {
            /* pin to the end, avoid counting the length */
            iter->సూచీ=-1;
            iter->ప్రారంభం=iter->హద్దు;
            iter->reserved_క్షేత్రం=0;
            ఒకవేళ(delta>=0) {
                తిరిగి UITER_UNKNOWN_సూచీ;
            } లేకపోతే {
                /* the current UTF-16 index is unknown, use only delta */
                pos=0;
                havePos=అబద్ధం;
            }
        }
        ఆపు;
    అప్రమేయ:
        తిరిగి -1;  /* Error */
    }

    ఒకవేళ(havePos) {
        /* shortcuts: pinning to the edges of the string */
        ఒకవేళ(pos<=0) {
            iter->సూచీ=iter->ప్రారంభం=iter->reserved_క్షేత్రం=0;
            తిరిగి 0;
        } లేకపోతే ఒకవేళ(iter->length>=0 && pos>=iter->length) {
            iter->సూచీ=iter->length;
            iter->ప్రారంభం=iter->హద్దు;
            iter->reserved_క్షేత్రం=0;
            తిరిగి iter->సూచీ;
        }

        /* minimize the number of U8_NEXT/PREV operations */
        ఒకవేళ(iter->సూచీ<0 || pos<iter->సూచీ/2) {
            /* go forward from the start instead of backward from the current index */
            iter->సూచీ=iter->ప్రారంభం=iter->reserved_క్షేత్రం=0;
        } లేకపోతే ఒకవేళ(iter->length>=0 && (iter->length-pos)<(pos-iter->సూచీ)) {
            /*
             * if we have the UTF-16 index and length and the new position is
             * closer to the end than the current index,
             * then go backward from the end instead of forward from the current index
             */
            iter->సూచీ=iter->length;
            iter->ప్రారంభం=iter->హద్దు;
            iter->reserved_క్షేత్రం=0;
        }

        delta=pos-iter->సూచీ;
        ఒకవేళ(delta==0) {
            తిరిగి iter->సూచీ; /* nothing to do */
        }
    } లేకపోతే {
        /* move relative to unknown UTF-16 index */
        ఒకవేళ(delta==0) {
            తిరిగి UITER_UNKNOWN_సూచీ; /* nothing to do */
        } లేకపోతే ఒకవేళ(-delta>=iter->ప్రారంభం) {
            /* moving backwards by more UChars than there are UTF-8 bytes, pin to 0 */
            iter->సూచీ=iter->ప్రారంభం=iter->reserved_క్షేత్రం=0;
            తిరిగి 0;
        } లేకపోతే ఒకవేళ(delta>=(iter->హద్దు-iter->ప్రారంభం)) {
            /* moving forward by more UChars than the remaining UTF-8 bytes, pin to the end */
            iter->సూచీ=iter->length; /* may or may not be <0 (unknown) */
            iter->ప్రారంభం=iter->హద్దు;
            iter->reserved_క్షేత్రం=0;
            తిరిగి iter->సూచీ>=0 ? iter->సూచీ : (int32_t)UITER_UNKNOWN_సూచీ;
        }
    }

    /* delta!=0 */

    /* move towards the requested position, pin to the edges of the string */
    s=(స్థిరరాశి uint8_t *)iter->context;
    pos=iter->సూచీ; /* could be <0 (unknown) */
    i=iter->ప్రారంభం;
    ఒకవేళ(delta>0) {
        /* go forward */
        int32_t హద్దు=iter->హద్దు;
        ఒకవేళ(iter->reserved_క్షేత్రం!=0) {
            iter->reserved_క్షేత్రం=0;
            ++pos;
            --delta;
        }
        అయితే(delta>0 && i<హద్దు) {
            U8_తదుపరి_OR_FFFD(s, i, హద్దు, c);
            ఒకవేళ(c<=0xffff) {
                ++pos;
                --delta;
            } లేకపోతే ఒకవేళ(delta>=2) {
                pos+=2;
                delta-=2;
            } లేకపోతే /* delta==1 */ {
                /* stop in the middle of a supplementary code point */
                iter->reserved_క్షేత్రం=c;
                ++pos;
                ఆపు; /* delta=0; */
            }
        }
        ఒకవేళ(i==హద్దు) {
            ఒకవేళ(iter->length<0 && iter->సూచీ>=0) {
                iter->length= iter->reserved_క్షేత్రం==0 ? pos : pos+1;
            } లేకపోతే ఒకవేళ(iter->సూచీ<0 && iter->length>=0) {
                iter->సూచీ= iter->reserved_క్షేత్రం==0 ? iter->length : iter->length-1;
            }
        }
    } లేకపోతే /* delta<0 */ {
        /* go backward */
        ఒకవేళ(iter->reserved_క్షేత్రం!=0) {
            iter->reserved_క్షేత్రం=0;
            i-=4; /* we stayed behind the supplementary code point; go before it now */
            --pos;
            ++delta;
        }
        అయితే(delta<0 && i>0) {
            U8_PREV_OR_FFFD(s, 0, i, c);
            ఒకవేళ(c<=0xffff) {
                --pos;
                ++delta;
            } లేకపోతే ఒకవేళ(delta<=-2) {
                pos-=2;
                delta+=2;
            } లేకపోతే /* delta==-1 */ {
                /* stop in the middle of a supplementary code point */
                i+=4; /* back to behind this supplementary code point for consistent state */
                iter->reserved_క్షేత్రం=c;
                --pos;
                ఆపు; /* delta=0; */
            }
        }
    }

    iter->ప్రారంభం=i;
    ఒకవేళ(iter->సూచీ>=0) {
        తిరిగి iter->సూచీ=pos;
    } లేకపోతే {
        /* we started with index<0 (unknown) so pos is bogus */
        ఒకవేళ(i<=1) {
            తిరిగి iter->సూచీ=i; /* reached the beginning */
        } లేకపోతే {
            /* we still don't know the UTF-16 index */
            తిరిగి UITER_UNKNOWN_సూచీ;
        }
    }
}

స్థిర UBool U_CALLCONV
utf8_Iterator_Has_తదుపరి(UCharIterator *iter) {
    తిరిగి iter->ప్రారంభం<iter->హద్దు || iter->reserved_క్షేత్రం!=0;
}

స్థిర UBool U_CALLCONV
utf8_Iterator_Has_మునుపటి(UCharIterator *iter) {
    తిరిగి iter->ప్రారంభం>0;
}

స్థిర UChar32 U_CALLCONV
utf8IteratorCurrent(UCharIterator *iter) {
    ఒకవేళ(iter->reserved_క్షేత్రం!=0) {
        తిరిగి U16_TRAIL(iter->reserved_క్షేత్రం);
    } లేకపోతే ఒకవేళ(iter->ప్రారంభం<iter->హద్దు) {
        స్థిరరాశి uint8_t *s=(స్థిరరాశి uint8_t *)iter->context;
        UChar32 c;
        int32_t i=iter->ప్రారంభం;

        U8_తదుపరి_OR_FFFD(s, i, iter->హద్దు, c);
        ఒకవేళ(c<=0xffff) {
            తిరిగి c;
        } లేకపోతే {
            తిరిగి U16_LEAD(c);
        }
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
utf8_Iterator_తదుపరి(UCharIterator *iter) {
    int32_t సూచీ;

    ఒకవేళ(iter->reserved_క్షేత్రం!=0) {
        char16_t trail=U16_TRAIL(iter->reserved_క్షేత్రం);
        iter->reserved_క్షేత్రం=0;
        ఒకవేళ((సూచీ=iter->సూచీ)>=0) {
            iter->సూచీ=సూచీ+1;
        }
        తిరిగి trail;
    } లేకపోతే ఒకవేళ(iter->ప్రారంభం<iter->హద్దు) {
        స్థిరరాశి uint8_t *s=(స్థిరరాశి uint8_t *)iter->context;
        UChar32 c;

        U8_తదుపరి_OR_FFFD(s, iter->ప్రారంభం, iter->హద్దు, c);
        ఒకవేళ((సూచీ=iter->సూచీ)>=0) {
            iter->సూచీ=++సూచీ;
            ఒకవేళ(iter->length<0 && iter->ప్రారంభం==iter->హద్దు) {
                iter->length= c<=0xffff ? సూచీ : సూచీ+1;
            }
        } లేకపోతే ఒకవేళ(iter->ప్రారంభం==iter->హద్దు && iter->length>=0) {
            iter->సూచీ= c<=0xffff ? iter->length : iter->length-1;
        }
        ఒకవేళ(c<=0xffff) {
            తిరిగి c;
        } లేకపోతే {
            iter->reserved_క్షేత్రం=c;
            తిరిగి U16_LEAD(c);
        }
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర UChar32 U_CALLCONV
utf8_Iterator_మునుపటి(UCharIterator *iter) {
    int32_t సూచీ;

    ఒకవేళ(iter->reserved_క్షేత్రం!=0) {
        char16_t lead=U16_LEAD(iter->reserved_క్షేత్రం);
        iter->reserved_క్షేత్రం=0;
        iter->ప్రారంభం-=4; /* we stayed behind the supplementary code point; go before it now */
        ఒకవేళ((సూచీ=iter->సూచీ)>0) {
            iter->సూచీ=సూచీ-1;
        }
        తిరిగి lead;
    } లేకపోతే ఒకవేళ(iter->ప్రారంభం>0) {
        స్థిరరాశి uint8_t *s=(స్థిరరాశి uint8_t *)iter->context;
        UChar32 c;

        U8_PREV_OR_FFFD(s, 0, iter->ప్రారంభం, c);
        ఒకవేళ((సూచీ=iter->సూచీ)>0) {
            iter->సూచీ=సూచీ-1;
        } లేకపోతే ఒకవేళ(iter->ప్రారంభం<=1) {
            iter->సూచీ= c<=0xffff ? iter->ప్రారంభం : iter->ప్రారంభం+1;
        }
        ఒకవేళ(c<=0xffff) {
            తిరిగి c;
        } లేకపోతే {
            iter->ప్రారంభం+=4; /* back to behind this supplementary code point for consistent state */
            iter->reserved_క్షేత్రం=c;
            తిరిగి U16_TRAIL(c);
        }
    } లేకపోతే {
        తిరిగి U_SENTINEL;
    }
}

స్థిర uint32_t U_CALLCONV
utf8IteratorGetState(స్థిరరాశి UCharIterator *iter) {
    uint32_t state=(uint32_t)(iter->ప్రారంభం<<1);
    ఒకవేళ(iter->reserved_క్షేత్రం!=0) {
        state|=1;
    }
    తిరిగి state;
}

స్థిర శూన్య U_CALLCONV
utf8_Iterator_సమితి_State(UCharIterator *iter,
                     uint32_t state,
                     UError_సంకేతం *p_Error_సంకేతం)
{
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        /* do nothing */
    } లేకపోతే ఒకవేళ(iter==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    } లేకపోతే ఒకవేళ(state==utf8IteratorGetState(iter)) {
        /* setting to the current state: no-op */
    } లేకపోతే {
        int32_t సూచీ=(int32_t)(state>>1); /* UTF-8 index */
        state&=1; /* 1 if in surrogate pair, must be index>=4 */

        ఒకవేళ((state==0 ? సూచీ<0 : సూచీ<4) || iter->హద్దు<సూచీ) {
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        } లేకపోతే {
            iter->ప్రారంభం=సూచీ; /* restore UTF-8 byte index */
            ఒకవేళ(సూచీ<=1) {
                iter->సూచీ=సూచీ;
            } లేకపోతే {
                iter->సూచీ=-1; /* unknown UTF-16 index */
            }
            ఒకవేళ(state==0) {
                iter->reserved_క్షేత్రం=0;
            } లేకపోతే {
                /* verified index>=4 above */
                UChar32 c;
                U8_PREV_OR_FFFD((స్థిరరాశి uint8_t *)iter->context, 0, సూచీ, c);
                ఒకవేళ(c<=0xffff) {
                    *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                } లేకపోతే {
                    iter->reserved_క్షేత్రం=c;
                }
            }
        }
    }
}

స్థిర స్థిరరాశి UCharIterator utf8Iterator={
    శూన్యసూచిక, 0, 0, 0, 0, 0,
    utf8_Iterator_Get_సూచీ,
    utf8IteratorMove,
    utf8_Iterator_Has_తదుపరి,
    utf8_Iterator_Has_మునుపటి,
    utf8IteratorCurrent,
    utf8_Iterator_తదుపరి,
    utf8_Iterator_మునుపటి,
    శూన్యసూచిక,
    utf8IteratorGetState,
    utf8_Iterator_సమితి_State
};

U_CAPI శూన్య U_EXPORT2
uiter_సమితి_UTF8(UCharIterator *iter, స్థిరరాశి అక్షరం *s, int32_t length) {
    ఒకవేళ (iter != శూన్యసూచిక) {
        ఒకవేళ (s != శూన్యసూచిక && length >= -1) {
            *iter=utf8Iterator;
            iter->context=s;
            ఒకవేళ(length>=0) {
                iter->హద్దు=length;
            } లేకపోతే {
                iter->హద్దు=(int32_t)uprv_strlen(s);
            }
            iter->length= iter->హద్దు<=1 ? iter->హద్దు : -1;
        } లేకపోతే {
            *iter=noopIterator;
        }
    }
}

/* Helper functions --------------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
uiter_current32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->current(iter);
    ఒకవేళ(U16_IS_SURROGATE(c)) {
        ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
            /*
             * go to the next code unit
             * we know that we are not at the limit because c!=U_SENTINEL
             */
            iter->move(iter, 1, UITER_CURRENT);
            ఒకవేళ(U16_IS_TRAIL(c2=iter->current(iter))) {
                c=U16_GET_SUPPLEMENTARY(c, c2);
            }

            /* undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        } లేకపోతే {
            ఒకవేళ(U16_IS_LEAD(c2=iter->మునుపటి(iter))) {
                c=U16_GET_SUPPLEMENTARY(c2, c);
            }
            ఒకవేళ(c2>=0) {
                /* undo index movement */
                iter->move(iter, 1, UITER_CURRENT);
            }
        }
    }
    తిరిగి c;
}

U_CAPI UChar32 U_EXPORT2
uiter_next32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->next(iter);
    ఒకవేళ(U16_IS_LEAD(c)) {
        ఒకవేళ(U16_IS_TRAIL(c2=iter->next(iter))) {
            c=U16_GET_SUPPLEMENTARY(c, c2);
        } లేకపోతే ఒకవేళ(c2>=0) {
            /* unmatched first surrogate, undo index movement */
            iter->move(iter, -1, UITER_CURRENT);
        }
    }
    తిరిగి c;
}

U_CAPI UChar32 U_EXPORT2
uiter_previous32(UCharIterator *iter) {
    UChar32 c, c2;

    c=iter->మునుపటి(iter);
    ఒకవేళ(U16_IS_TRAIL(c)) {
        ఒకవేళ(U16_IS_LEAD(c2=iter->మునుపటి(iter))) {
            c=U16_GET_SUPPLEMENTARY(c2, c);
        } లేకపోతే ఒకవేళ(c2>=0) {
            /* unmatched second surrogate, undo index movement */
            iter->move(iter, 1, UITER_CURRENT);
        }
    }
    తిరిగి c;
}

U_CAPI uint32_t U_EXPORT2
uiter_getState(స్థిరరాశి UCharIterator *iter) {
    ఒకవేళ(iter==శూన్యసూచిక || iter->getState==శూన్యసూచిక) {
        తిరిగి UITER_NO_STATE;
    } లేకపోతే {
        తిరిగి iter->getState(iter);
    }
}

U_CAPI శూన్య U_EXPORT2
uiter_సమితి_State(UCharIterator *iter, uint32_t state, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        /* do nothing */
    } లేకపోతే ఒకవేళ(iter==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    } లేకపోతే ఒకవేళ(iter->సమితి_State==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
    } లేకపోతే {
        iter->సమితి_State(iter, state, p_Error_సంకేతం);
    }
}

U_CDECL_చివర
