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

#جيڪڏهن_وصف_نه_ٿيل __UBIDI_PROPS_H__
#وصف_ڪريو __UBIDI_PROPS_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"
#ضمن "putilimp.h"
#ضمن "uset_imp.h"
#ضمن "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC فراغ
ubidi_جوڙيو_Property_Starts(ثابت USetAdder *sa, UError_ڪوڊ *p_Error_ڪوڊ);

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

U_CFUNC UJoining_گروهه
ubidi_get_Joining_گروهه(UChar32 c);

U_CFUNC UBidiPairedBracketType
ubidi_getPairedBracketType(UChar32 c);

U_CFUNC UChar32
ubidi_getPairedBracket(UChar32 c);

/* file definitions --------------------------------------------------------- */

#وصف_ڪريو UBIDI_معلومات_NAME "ubidi"
#وصف_ڪريو UBIDI_معلومات_TYPE "icu"

/* format "BiDi" */
#وصف_ڪريو UBIDI_FMT_0 0x42
#وصف_ڪريو UBIDI_FMT_1 0x69
#وصف_ڪريو UBIDI_FMT_2 0x44
#وصف_ڪريو UBIDI_FMT_3 0x69

/* indexes into indexes[] */
تعداد {
    UBIDI_IX_اشاريو_چوٽي,
    UBIDI_IX_ڊيگهائي,
    UBIDI_IX_TRIE_سائيز,
    UBIDI_IX_MIRROR_ڊيگهائي,

    UBIDI_IX_JG_شروعات,
    UBIDI_IX_JG_حد_بندي,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_قدرون_اشاريو=15,
    UBIDI_IX_چوٽي=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

تعداد {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_سرڪايو=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_سرڪايو=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_سرڪايو=10,
    UBIDI_BIDI_CONTROL_سرڪايو=11,

    UBIDI_IS_MIRRORED_سرڪايو=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_سرڪايو=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_سرڪايو=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#وصف_ڪريو UBIDI_CLASS_نقاب        0x0000001f
#وصف_ڪريو UBIDI_JT_نقاب           0x000000e0
#وصف_ڪريو UBIDI_BPT_نقاب          0x00000300

#وصف_ڪريو UBIDI_MAX_JG_نقاب       0x00ff0000

#وصف_ڪريو UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_نقاب)
#وصف_ڪريو UBIDI_GET_FLAG(props, سرڪايو) (((props)>>(سرڪايو))&1)

#إذا U_SIGNED_ساڄو_سرڪايو_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_سرڪايو)
#وإلا
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_سرڪايو)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_سرڪايو))
#جيڪڏهن_پورو

تعداد {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

تعداد {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_اشاريو_سرڪايو=21,
    UBIDI_MAX_MIRROR_اشاريو=0x7ff
};

#وصف_ڪريو UBIDI_GET_MIRROR_ڪوڊ_نقطو(m) (UChar32)((m)&0x1fffff)

#وصف_ڪريو UBIDI_GET_MIRROR_اشاريو(m) ((m)>>UBIDI_MIRROR_اشاريو_سرڪايو)

U_CDECL_پڇاڙي

#جيڪڏهن_پورو
