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

#جے_بنا_تعریف __UBIDI_PROPS_H__
#تعریف_کرو __UBIDI_PROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"
#ضمن "putilimp.h"
#ضمن "uset_imp.h"
#ضمن "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC فراغ
ubidi_جوڑو_Property_Starts(ثابت USetAdder *sa, UError_کوڈ *p_Error_کوڈ);

/* property access functions */

U_CFUNC int32_t
ubidi_get_Max_قدر(UProperty which);

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

U_CFUNC UJoining_گروہ
ubidi_get_Joining_گروہ(UChar32 c);

U_CFUNC UBidiPairedBracketType
ubidi_getPairedBracketType(UChar32 c);

U_CFUNC UChar32
ubidi_getPairedBracket(UChar32 c);

/* file definitions --------------------------------------------------------- */

#تعریف_کرو UBIDI_انکڑے_NAME "ubidi"
#تعریف_کرو UBIDI_انکڑے_TYPE "icu"

/* format "BiDi" */
#تعریف_کرو UBIDI_FMT_0 0x42
#تعریف_کرو UBIDI_FMT_1 0x69
#تعریف_کرو UBIDI_FMT_2 0x44
#تعریف_کرو UBIDI_FMT_3 0x69

/* indexes into indexes[] */
تعداد {
    UBIDI_IX_اشاریہ_چوٹی,
    UBIDI_IX_لمائی,
    UBIDI_IX_TRIE_ناپ,
    UBIDI_IX_MIRROR_لمائی,

    UBIDI_IX_JG_شروع,
    UBIDI_IX_JG_حد_بندی,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_اقدار_اشاریہ=15,
    UBIDI_IX_چوٹی=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

تعداد {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_کھسکاؤ=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_کھسکاؤ=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_کھسکاؤ=10,
    UBIDI_BIDI_CONTROL_کھسکاؤ=11,

    UBIDI_IS_MIRRORED_کھسکاؤ=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_کھسکاؤ=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_کھسکاؤ=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#تعریف_کرو UBIDI_CLASS_نقاب        0x0000001f
#تعریف_کرو UBIDI_JT_نقاب           0x000000e0
#تعریف_کرو UBIDI_BPT_نقاب          0x00000300

#تعریف_کرو UBIDI_MAX_JG_نقاب       0x00ff0000

#تعریف_کرو UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_نقاب)
#تعریف_کرو UBIDI_GET_FLAG(props, کھسکاؤ) (((props)>>(کھسکاؤ))&1)

#إذا U_SIGNED_دایاں_کھسکاؤ_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_کھسکاؤ)
#وإلا
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_کھسکاؤ)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_کھسکاؤ))
#جے_انت

تعداد {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

تعداد {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_اشاریہ_کھسکاؤ=21,
    UBIDI_MAX_MIRROR_اشاریہ=0x7ff
};

#تعریف_کرو UBIDI_GET_MIRROR_کوڈ_نقطہ(m) (UChar32)((m)&0x1fffff)

#تعریف_کرو UBIDI_GET_MIRROR_اشاریہ(m) ((m)>>UBIDI_MIRROR_اشاریہ_کھسکاؤ)

U_CDECL_سرا

#جے_انت
