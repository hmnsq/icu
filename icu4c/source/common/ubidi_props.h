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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UBIDI_PROPS_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UBIDI_PROPS_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC ಶೂನ್ಯ
ubidi_ಕೂಡಿಸು_Property_Starts(ಸ್ಥಿರಾಂಕ USetAdder *sa, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/* property access functions */

U_CFUNC int32_t
ubidi_get_Max_ಮೌಲ್ಯ(UProperty which);

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

U_CFUNC UJoining_ಗುಂಪು
ubidi_get_Joining_ಗುಂಪು(UChar32 c);

U_CFUNC UBidiPairedBracketType
ubidi_getPairedBracketType(UChar32 c);

U_CFUNC UChar32
ubidi_getPairedBracket(UChar32 c);

/* file definitions --------------------------------------------------------- */

#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_ದತ್ತಾಂಶ_NAME "ubidi"
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_ದತ್ತಾಂಶ_TYPE "icu"

/* format "BiDi" */
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_FMT_0 0x42
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_FMT_1 0x69
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_FMT_2 0x44
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_FMT_3 0x69

/* indexes into indexes[] */
ಎಣಿಕೆ {
    UBIDI_IX_ಸೂಚ್ಯಂಕ_ತುದಿ,
    UBIDI_IX_ಉದ್ದಳತೆ,
    UBIDI_IX_TRIE_ಗಾತ್ರ,
    UBIDI_IX_MIRROR_ಉದ್ದಳತೆ,

    UBIDI_IX_JG_ಆರಂಭ,
    UBIDI_IX_JG_ಎಲ್ಲೆ,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_ಮೌಲ್ಯಗಳು_ಸೂಚ್ಯಂಕ=15,
    UBIDI_IX_ತುದಿ=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

ಎಣಿಕೆ {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_ಸರಿಸು=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_ಸರಿಸು=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_ಸರಿಸು=10,
    UBIDI_BIDI_CONTROL_ಸರಿಸು=11,

    UBIDI_IS_MIRRORED_ಸರಿಸು=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_ಸರಿಸು=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_ಸರಿಸು=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_CLASS_ಮುಖವಾಡ        0x0000001f
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_JT_ಮುಖವಾಡ           0x000000e0
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_BPT_ಮುಖವಾಡ          0x00000300

#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_MAX_JG_ಮುಖವಾಡ       0x00ff0000

#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_ಮುಖವಾಡ)
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_GET_FLAG(props, ಸರಿಸು) (((props)>>(ಸರಿಸು))&1)

#ಆದರೆ U_SIGNED_ಬಲ_ಸರಿಸು_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_ಸರಿಸು)
#ಇಲ್ಲದಿದ್ದರೆ
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_ಸರಿಸು)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_ಸರಿಸು))
#ಆದರೆ_ಅಂತ್ಯ

ಎಣಿಕೆ {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

ಎಣಿಕೆ {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_ಸೂಚ್ಯಂಕ_ಸರಿಸು=21,
    UBIDI_MAX_MIRROR_ಸೂಚ್ಯಂಕ=0x7ff
};

#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_GET_MIRROR_ಸಂಕೇತ_ಬಿಂದು(m) (UChar32)((m)&0x1fffff)

#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_GET_MIRROR_ಸೂಚ್ಯಂಕ(m) ((m)>>UBIDI_MIRROR_ಸೂಚ್ಯಂಕ_ಸರಿಸು)

U_CDECL_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
