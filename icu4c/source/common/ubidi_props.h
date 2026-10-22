// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2004-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ubidi_props.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004dec30
*   created by: Markus W. Scherer
*
*   Low-level Unicode bidi/shaping properties access.
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UBIDI_PROPS_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UBIDI_PROPS_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uset_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC ਖਾਲੀ
ubidi_ਜੋੜੋ_Property_Starts(ਅਚਲ USetAdder *sa, UError_ਕੋਡ *p_Error_ਕੋਡ);

/* property access functions */

U_CFUNC int32_t
ubidi_get_Max_ਮੁੱਲ(UProperty which);

U_CAPI UCharDirection
ubidi_getClass(UChar32 c);

U_CFUNC UBool
ubidi_isMirrored(UChar32 c);

U_CFUNC UChar32
ubidi_getMirror(UChar32 c);

U_CFUNC UBool
ubidi_isBidiControl(UChar32 c);

U_CFUNC UBool
ubidi_isJoinControl(UChar32 c);

U_CFUNC UJoiningType
ubidi_getJoiningType(UChar32 c);

U_CFUNC UJoining_ਸਮੂਹ
ubidi_get_Joining_ਸਮੂਹ(UChar32 c);

U_CFUNC UBidiPairedBracketType
ubidi_getPairedBracketType(UChar32 c);

U_CFUNC UChar32
ubidi_getPairedBracket(UChar32 c);

/* file definitions --------------------------------------------------------- */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_ਅੰਕੜੇ_NAME "ubidi"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_ਅੰਕੜੇ_TYPE "icu"

/* format "BiDi" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_FMT_0 0x42
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_FMT_1 0x69
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_FMT_2 0x44
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_FMT_3 0x69

/* indexes into indexes[] */
ਗਿਣਤੀ {
    UBIDI_IX_ਸੂਚਕਾਂਕ_ਸਿਖਰ,
    UBIDI_IX_ਲੰਮਾਈ,
    UBIDI_IX_TRIE_ਮਾਪ,
    UBIDI_IX_MIRROR_ਲੰਮਾਈ,

    UBIDI_IX_JG_ਸ਼ੁਰੂ,
    UBIDI_IX_JG_ਹੱਦਬੰਦੀ,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_ਮੁੱਲਾਂ_ਸੂਚਕਾਂਕ=15,
    UBIDI_IX_ਸਿਖਰ=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

ਗਿਣਤੀ {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_ਖਿਸਕਾਓ=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_ਖਿਸਕਾਓ=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_ਖਿਸਕਾਓ=10,
    UBIDI_BIDI_CONTROL_ਖਿਸਕਾਓ=11,

    UBIDI_IS_MIRRORED_ਖਿਸਕਾਓ=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_ਖਿਸਕਾਓ=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_ਖਿਸਕਾਓ=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_CLASS_ਨਕਾਬ        0x0000001f
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_JT_ਨਕਾਬ           0x000000e0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_BPT_ਨਕਾਬ          0x00000300

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_MAX_JG_ਨਕਾਬ       0x00ff0000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_ਨਕਾਬ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_GET_FLAG(props, ਖਿਸਕਾਓ) (((props)>>(ਖਿਸਕਾਓ))&1)

#ਜੇ U_SIGNED_ਸੱਜਾ_ਖਿਸਕਾਓ_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_ਖਿਸਕਾਓ)
#ਨਹੀਂਤਾਂ
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_ਖਿਸਕਾਓ)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_ਖਿਸਕਾਓ))
#ਜੇ_ਅੰਤ

ਗਿਣਤੀ {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

ਗਿਣਤੀ {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ=21,
    UBIDI_MAX_MIRROR_ਸੂਚਕਾਂਕ=0x7ff
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_GET_MIRROR_ਕੋਡ_ਬਿੰਦੂ(m) (UChar32)((m)&0x1fffff)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_GET_MIRROR_ਸੂਚਕਾਂਕ(m) ((m)>>UBIDI_MIRROR_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ)

U_CDECL_ਸਿਰਾ

#ਜੇ_ਅੰਤ
