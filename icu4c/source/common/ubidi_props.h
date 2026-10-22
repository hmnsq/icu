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

#اگر_تعریف_نشده __UBIDI_PROPS_H__
#تعریف_کن __UBIDI_PROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"
#ضمن "putilimp.h"
#ضمن "uset_imp.h"
#ضمن "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC فراغ
ubidi_جمع_کن_Property_Starts(ثابت USetAdder *sa, UError_کد *p_Error_کد);

/* property access functions */

U_CFUNC int32_t
ubidi_get_Max_مقدار(UProperty which);

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

U_CFUNC UJoining_گروه
ubidi_get_Joining_گروه(UChar32 c);

U_CFUNC UBidiPairedBracketType
ubidi_getPairedBracketType(UChar32 c);

U_CFUNC UChar32
ubidi_getPairedBracket(UChar32 c);

/* file definitions --------------------------------------------------------- */

#تعریف_کن UBIDI_داده_NAME "ubidi"
#تعریف_کن UBIDI_داده_TYPE "icu"

/* format "BiDi" */
#تعریف_کن UBIDI_FMT_0 0x42
#تعریف_کن UBIDI_FMT_1 0x69
#تعریف_کن UBIDI_FMT_2 0x44
#تعریف_کن UBIDI_FMT_3 0x69

/* indexes into indexes[] */
تعداد {
    UBIDI_IX_شاخص_بالاترین,
    UBIDI_IX_درازی,
    UBIDI_IX_TRIE_اندازه,
    UBIDI_IX_MIRROR_درازی,

    UBIDI_IX_JG_شروع,
    UBIDI_IX_JG_حد,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_مقادیر_شاخص=15,
    UBIDI_IX_بالاترین=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

تعداد {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_لغزش=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_لغزش=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_لغزش=10,
    UBIDI_BIDI_CONTROL_لغزش=11,

    UBIDI_IS_MIRRORED_لغزش=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_لغزش=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_لغزش=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#تعریف_کن UBIDI_CLASS_ماسک        0x0000001f
#تعریف_کن UBIDI_JT_ماسک           0x000000e0
#تعریف_کن UBIDI_BPT_ماسک          0x00000300

#تعریف_کن UBIDI_MAX_JG_ماسک       0x00ff0000

#تعریف_کن UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_ماسک)
#تعریف_کن UBIDI_GET_FLAG(props, لغزش) (((props)>>(لغزش))&1)

#إذا U_SIGNED_راست_لغزش_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_لغزش)
#وإلا
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_لغزش)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_لغزش))
#پایان_اگر

تعداد {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

تعداد {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_شاخص_لغزش=21,
    UBIDI_MAX_MIRROR_شاخص=0x7ff
};

#تعریف_کن UBIDI_GET_MIRROR_کد_نقطه(m) (UChar32)((m)&0x1fffff)

#تعریف_کن UBIDI_GET_MIRROR_شاخص(m) ((m)>>UBIDI_MIRROR_شاخص_لغزش)

U_CDECL_انتها

#پایان_اگر
