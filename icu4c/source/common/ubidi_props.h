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

#اگر_غیر_تعریف_شدہ __UBIDI_PROPS_H__
#تعریف_کرو __UBIDI_PROPS_H__

#شامل "unicode/utypes.h"
#شامل "unicode/uset.h"
#شامل "putilimp.h"
#شامل "uset_imp.h"
#شامل "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC خالی
ubidi_جوڑو_Property_Starts(ثابت USetAdder *sa, UError_رمز *p_Error_رمز);

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

#تعریف_کرو UBIDI_ڈیٹا_NAME "ubidi"
#تعریف_کرو UBIDI_ڈیٹا_TYPE "icu"

/* format "BiDi" */
#تعریف_کرو UBIDI_FMT_0 0x42
#تعریف_کرو UBIDI_FMT_1 0x69
#تعریف_کرو UBIDI_FMT_2 0x44
#تعریف_کرو UBIDI_FMT_3 0x69

/* indexes into indexes[] */
شمار {
    UBIDI_IX_اشاریہ_چوٹی,
    UBIDI_IX_لمبائی,
    UBIDI_IX_TRIE_سائز,
    UBIDI_IX_MIRROR_لمبائی,

    UBIDI_IX_JG_آغاز,
    UBIDI_IX_JG_حد_بندی,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_اقدار_اشاریہ=15,
    UBIDI_IX_چوٹی=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

شمار {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_سرکاؤ=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_سرکاؤ=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_سرکاؤ=10,
    UBIDI_BIDI_CONTROL_سرکاؤ=11,

    UBIDI_IS_MIRRORED_سرکاؤ=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_سرکاؤ=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_سرکاؤ=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#تعریف_کرو UBIDI_CLASS_نقاب        0x0000001f
#تعریف_کرو UBIDI_JT_نقاب           0x000000e0
#تعریف_کرو UBIDI_BPT_نقاب          0x00000300

#تعریف_کرو UBIDI_MAX_JG_نقاب       0x00ff0000

#تعریف_کرو UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_نقاب)
#تعریف_کرو UBIDI_GET_FLAG(props, سرکاؤ) (((props)>>(سرکاؤ))&1)

#اگر U_SIGNED_دایاں_سرکاؤ_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_سرکاؤ)
#ورنہ
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_سرکاؤ)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_سرکاؤ))
#اگر_ختم

شمار {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

شمار {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_اشاریہ_سرکاؤ=21,
    UBIDI_MAX_MIRROR_اشاریہ=0x7ff
};

#تعریف_کرو UBIDI_GET_MIRROR_رمز_نقطہ(m) (UChar32)((m)&0x1fffff)

#تعریف_کرو UBIDI_GET_MIRROR_اشاریہ(m) ((m)>>UBIDI_MIRROR_اشاریہ_سرکاؤ)

U_CDECL_انتہا

#اگر_ختم
