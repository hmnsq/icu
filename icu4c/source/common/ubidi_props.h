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

#यदि_अपरिभाषितम् __UBIDI_PROPS_H__
#परिभाषय __UBIDI_PROPS_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uset_imp.h"
#अन्तर्भाव "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC शून्य
ubidi_सङ्कलय_Property_Starts(ध्रुव USetAdder *sa, UError_संहिता *p_Error_संहिता);

/* property access functions */

U_CFUNC int32_t
ubidi_get_Max_मानम्(UProperty which);

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

U_CFUNC UJoining_गणः
ubidi_get_Joining_गणः(UChar32 c);

U_CFUNC UBidiPairedBracketType
ubidi_getPairedBracketType(UChar32 c);

U_CFUNC UChar32
ubidi_getPairedBracket(UChar32 c);

/* file definitions --------------------------------------------------------- */

#परिभाषय UBIDI_दत्त_NAME "ubidi"
#परिभाषय UBIDI_दत्त_TYPE "icu"

/* format "BiDi" */
#परिभाषय UBIDI_FMT_0 0x42
#परिभाषय UBIDI_FMT_1 0x69
#परिभाषय UBIDI_FMT_2 0x44
#परिभाषय UBIDI_FMT_3 0x69

/* indexes into indexes[] */
गणना {
    UBIDI_IX_सूचकांक_शिखरम्,
    UBIDI_IX_दीर्घता,
    UBIDI_IX_TRIE_आकार,
    UBIDI_IX_MIRROR_दीर्घता,

    UBIDI_IX_JG_START,
    UBIDI_IX_JG_सीमा,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_मानानि_सूचकांक=15,
    UBIDI_IX_शिखरम्=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

गणना {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_सरणम्=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_सरणम्=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_सरणम्=10,
    UBIDI_BIDI_CONTROL_सरणम्=11,

    UBIDI_IS_MIRRORED_सरणम्=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_सरणम्=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_सरणम्=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#परिभाषय UBIDI_CLASS_आवरणम्        0x0000001f
#परिभाषय UBIDI_JT_आवरणम्           0x000000e0
#परिभाषय UBIDI_BPT_आवरणम्          0x00000300

#परिभाषय UBIDI_MAX_JG_आवरणम्       0x00ff0000

#परिभाषय UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_आवरणम्)
#परिभाषय UBIDI_GET_FLAG(props, सरणम्) (((props)>>(सरणम्))&1)

#यदि U_SIGNED_दक्षिणः_सरणम्_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_सरणम्)
#अन्यथा
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_सरणम्)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_सरणम्))
#यदि_अन्तः

गणना {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

गणना {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_सूचकांक_सरणम्=21,
    UBIDI_MAX_MIRROR_सूचकांक=0x7ff
};

#परिभाषय UBIDI_GET_MIRROR_संहिता_POINT(m) (UChar32)((m)&0x1fffff)

#परिभाषय UBIDI_GET_MIRROR_सूचकांक(m) ((m)>>UBIDI_MIRROR_सूचकांक_सरणम्)

U_CDECL_अन्तः

#यदि_अन्तः
