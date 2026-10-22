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

#अगर_अघोषित __UBIDI_PROPS_H__
#घोषणा __UBIDI_PROPS_H__

#समावेश "unicode/utypes.h"
#समावेश "unicode/uset.h"
#समावेश "putilimp.h"
#समावेश "uset_imp.h"
#समावेश "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC व्योम
ubidi_addPropertyStarts(स्थिर USetAdder *sa, UErrorCode *pErrorCode);

/* property access functions */

U_CFUNC int32_t
ubidi_get_Max_मूल्य(UProperty which);

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

U_CFUNC UJoiningGroup
ubidi_getJoiningGroup(UChar32 c);

U_CFUNC UBidiPairedBracketType
ubidi_getPairedBracketType(UChar32 c);

U_CFUNC UChar32
ubidi_getPairedBracket(UChar32 c);

/* file definitions --------------------------------------------------------- */

#घोषणा UBIDI_तथ्य_NAME "ubidi"
#घोषणा UBIDI_तथ्य_TYPE "icu"

/* format "BiDi" */
#घोषणा UBIDI_FMT_0 0x42
#घोषणा UBIDI_FMT_1 0x69
#घोषणा UBIDI_FMT_2 0x44
#घोषणा UBIDI_FMT_3 0x69

/* indexes into indexes[] */
क्रमागत {
    UBIDI_IX_सूचकांक_शीर्ष,
    UBIDI_IX_लंबाई,
    UBIDI_IX_TRIE_आकार,
    UBIDI_IX_MIRROR_लंबाई,

    UBIDI_IX_JG_START,
    UBIDI_IX_JG_LIMIT,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_मूल्यावली_सूचकांक=15,
    UBIDI_IX_शीर्ष=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

क्रमागत {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_SHIFT=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_SHIFT=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_SHIFT=10,
    UBIDI_BIDI_CONTROL_SHIFT=11,

    UBIDI_IS_MIRRORED_SHIFT=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_SHIFT=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_SHIFT=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#घोषणा UBIDI_CLASS_MASK        0x0000001f
#घोषणा UBIDI_JT_MASK           0x000000e0
#घोषणा UBIDI_BPT_MASK          0x00000300

#घोषणा UBIDI_MAX_JG_MASK       0x00ff0000

#घोषणा UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_MASK)
#घोषणा UBIDI_GET_FLAG(props, shift) (((props)>>(shift))&1)

#अगर U_SIGNED_दायाँ_SHIFT_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_SHIFT)
#अन्यथा
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_SHIFT)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_SHIFT))
#पूर्ण_अगर

क्रमागत {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

क्रमागत {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_सूचकांक_SHIFT=21,
    UBIDI_MAX_MIRROR_सूचकांक=0x7ff
};

#घोषणा UBIDI_GET_MIRROR_CODE_POINT(m) (UChar32)((m)&0x1fffff)

#घोषणा UBIDI_GET_MIRROR_सूचकांक(m) ((m)>>UBIDI_MIRROR_सूचकांक_SHIFT)

U_CDECL_END

#पूर्ण_अगर
