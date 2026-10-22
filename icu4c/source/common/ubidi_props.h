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

#నిర్వచితం_కాకపోతే __UBIDI_PROPS_H__
#నిర్వచించు __UBIDI_PROPS_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uset_imp.h"
#అన్తర్భావ "udataswp.h"

U_CDECL_BEGIN

/* library API -------------------------------------------------------------- */

U_CFUNC శూన్య
ubidi_కూడు_Property_Starts(స్థిరరాశి USetAdder *sa, UError_సంకేతం *p_Error_సంకేతం);

/* property access functions */

U_CFUNC int32_t
ubidi_get_Max_విలువ(UProperty which);

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

U_CFUNC UJoining_సమూహం
ubidi_get_Joining_సమూహం(UChar32 c);

U_CFUNC UBidiPairedBracketType
ubidi_getPairedBracketType(UChar32 c);

U_CFUNC UChar32
ubidi_getPairedBracket(UChar32 c);

/* file definitions --------------------------------------------------------- */

#నిర్వచించు UBIDI_దత్తాంశం_NAME "ubidi"
#నిర్వచించు UBIDI_దత్తాంశం_TYPE "icu"

/* format "BiDi" */
#నిర్వచించు UBIDI_FMT_0 0x42
#నిర్వచించు UBIDI_FMT_1 0x69
#నిర్వచించు UBIDI_FMT_2 0x44
#నిర్వచించు UBIDI_FMT_3 0x69

/* indexes into indexes[] */
గణన {
    UBIDI_IX_సూచీ_శిఖరం,
    UBIDI_IX_నిడివి,
    UBIDI_IX_TRIE_కొలత,
    UBIDI_IX_MIRROR_నిడివి,

    UBIDI_IX_JG_ప్రారంభం,
    UBIDI_IX_JG_హద్దు,
    UBIDI_IX_JG_START2,  /* new in format version 2.2, ICU 54 */
    UBIDI_IX_JG_LIMIT2,

    UBIDI_MAX_విలువలు_సూచీ=15,
    UBIDI_IX_శిఖరం=16
};

/* definitions for 16-bit bidi/shaping properties word ---------------------- */

గణన {
 /* UBIDI_CLASS_SHIFT=0, */     /* bidi class: 5 bits (4..0) */
    UBIDI_JT_జరుపు=5,           /* joining type: 3 bits (7..5) */

    UBIDI_BPT_జరుపు=8,          /* Bidi_Paired_Bracket_Type(bpt): 2 bits (9..8) */

    UBIDI_JOIN_CONTROL_జరుపు=10,
    UBIDI_BIDI_CONTROL_జరుపు=11,

    UBIDI_IS_MIRRORED_జరుపు=12,         /* 'is mirrored' */
    UBIDI_MIRROR_DELTA_జరుపు=13,        /* bidi mirroring delta: 3 bits (15..13) */

    UBIDI_MAX_JG_జరుపు=16               /* max JG value in indexes[UBIDI_MAX_VALUES_INDEX] bits 23..16 */
};

#నిర్వచించు UBIDI_CLASS_ముసుగు        0x0000001f
#నిర్వచించు UBIDI_JT_ముసుగు           0x000000e0
#నిర్వచించు UBIDI_BPT_ముసుగు          0x00000300

#నిర్వచించు UBIDI_MAX_JG_ముసుగు       0x00ff0000

#నిర్వచించు UBIDI_GET_CLASS(props) ((props)&UBIDI_CLASS_ముసుగు)
#నిర్వచించు UBIDI_GET_FLAG(props, జరుపు) (((props)>>(జరుపు))&1)

#ఒకవేళ U_SIGNED_కుడి_జరుపు_IS_ARITHMETIC
#   define UBIDI_GET_MIRROR_DELTA(props) ((int16_t)(props)>>UBIDI_MIRROR_DELTA_జరుపు)
#లేకపోతే
#   define UBIDI_GET_MIRROR_DELTA(props) (int16_t)(((props)&0x8000) ? (((props)>>UBIDI_MIRROR_DELTA_జరుపు)|0xe000) : ((props)>>UBIDI_MIRROR_DELTA_జరుపు))
#అయితే_ముగింపు

గణన {
    UBIDI_ESC_MIRROR_DELTA=-4,
    UBIDI_MIN_MIRROR_DELTA=-3,
    UBIDI_MAX_MIRROR_DELTA=3
};

/* definitions for 32-bit mirror table entry -------------------------------- */

గణన {
    /* the source Unicode code point takes 21 bits (20..0) */
    UBIDI_MIRROR_సూచీ_జరుపు=21,
    UBIDI_MAX_MIRROR_సూచీ=0x7ff
};

#నిర్వచించు UBIDI_GET_MIRROR_సంకేతం_బిందువు(m) (UChar32)((m)&0x1fffff)

#నిర్వచించు UBIDI_GET_MIRROR_సూచీ(m) ((m)>>UBIDI_MIRROR_సూచీ_జరుపు)

U_CDECL_చివర

#అయితే_ముగింపు
