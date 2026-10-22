// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ubidiimp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999aug06
*   created by: Markus W. Scherer, updated by Matitiahu Allouche
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UBIDIIMP_H
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDIIMP_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubidi.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "ubidi_props.h"

/* miscellaneous definitions ---------------------------------------------- */
/*
 * Internal option bit for ubidi_writeReordered() and doWriteReverse():
 * Used exclusively by setParaRunsOnly() during Scheme 9 (LOGICAL, RTL => LOGICAL, LTR)
 * BidiTransform transformations. When mirroring changes code point length (e.g., U+1DB10
 * contracting back to U+221D), setParaRunsOnly() requires that the temporary visualText
 * buffer maintain 1-to-1 code unit synchronization with pBiDi->length (4) to prevent
 * array desynchronization in the subsequent ubidi_setPara call.
 * 
 * When this flag is set, doWriteReverse() steps source indices by destination code unit
 * length (j += k) instead of original base character length (j += origBaseLen), preserving
 * legacy ICU behavior where the trailing surrogate half (which has Bidi class LTR) is copied
 * into visualText, allowing ubidi_setPara to correctly split the run for reordering.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_ಆಂತರಿಕ_RUNS_ONLY 64

// ICU-20853=ICU-20935 Solaris #defines CS and ES in sys/regset.h
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CS
#   undef CS
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ES
#   undef ES
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint8_t DirProp;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t Flags;

/*  Comparing the description of the BiDi algorithm with this implementation
    is easier with the same names for the BiDi types in the code as there.
    See UCharDirection in uchar.h .
*/
ಎಣಿಕೆ {
    L=  U_ಎಡ_ಗಮ್ಯ_ಬಲ,                /*  0 */
    R=  U_ಬಲ_ಗಮ್ಯ_ಎಡ,                /*  1 */
    EN= U_EUROPEAN_ಸಂಖ್ಯೆ,              /*  2 */
    ES= U_EUROPEAN_ಸಂಖ್ಯೆ_SEPARATOR,    /*  3 */
    ET= U_EUROPEAN_ಸಂಖ್ಯೆ_TERMINATOR,   /*  4 */
    AN= U_ARABIC_ಸಂಖ್ಯೆ,                /*  5 */
    CS= U_COMMON_ಸಂಖ್ಯೆ_SEPARATOR,      /*  6 */
    B=  U_ಖಂಡ_SEPARATOR,              /*  7 */
    S=  U_SEGMENT_SEPARATOR,            /*  8 */
    WS= U_WHITE_SPACE_NEUTRAL,          /*  9 */
    ON= U_OTHER_NEUTRAL,                /* 10 */
    LRE=U_ಎಡ_ಗಮ್ಯ_ಬಲ_EMBEDDING,      /* 11 */
    LRO=U_ಎಡ_ಗಮ್ಯ_ಬಲ_OVERRIDE,       /* 12 */
    AL= U_ಬಲ_ಗಮ್ಯ_ಎಡ_ARABIC,         /* 13 */
    RLE=U_ಬಲ_ಗಮ್ಯ_ಎಡ_EMBEDDING,      /* 14 */
    RLO=U_ಬಲ_ಗಮ್ಯ_ಎಡ_OVERRIDE,       /* 15 */
    PDF=U_ತೆಗೆ_DIRECTIONAL_FORMAT,       /* 16 */
    NSM=U_DIR_NON_SPACING_MARK,         /* 17 */
    BN= U_BOUNDARY_NEUTRAL,             /* 18 */
    FSI=U_ಮೊದಲ_STRONG_ISOLATE,         /* 19 */
    LRI=U_ಎಡ_ಗಮ್ಯ_ಬಲ_ISOLATE,        /* 20 */
    RLI=U_ಬಲ_ಗಮ್ಯ_ಎಡ_ISOLATE,        /* 21 */
    PDI=U_ತೆಗೆ_DIRECTIONAL_ISOLATE,      /* 22 */
    ENL,    /* EN after W7 */           /* 23 */
    ENR,    /* EN not subject to W7 */  /* 24 */
    dir_Prop_ಎಣಿಕೆ
};

/*  Sometimes, bit values are more appropriate
    to deal with directionality properties.
    Abbreviations in these macro names refer to names
    used in the BiDi algorithm.
*/
#ವ್ಯಾಖ್ಯಾನಿಸು DIRPROP_FLAG(dir) (1UL<<(dir))
#ವ್ಯಾಖ್ಯಾನಿಸು ಶುದ್ಧ_DIRPROP(prop)  ((prop)&~0xE0)    ?????????????????????????

/* special flag for multiple runs from explicit embedding codes */
#ವ್ಯಾಖ್ಯಾನಿಸು DIRPROP_FLAG_MULTI_RUNS (1UL<<31)

/* are there any characters that are LTR or RTL? */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_LTR (DIRPROP_FLAG(L)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(ENL)|DIRPROP_FLAG(ENR)|DIRPROP_FLAG(AN)|DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(LRI))
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_RTL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(RLI))
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_R_AL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL))
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_STRONG_EN_AN (DIRPROP_FLAG(L)|DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(AN))

/* explicit embedding codes */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_EXPLICIT (DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(PDF))

/* explicit isolate codes */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_ISO (DIRPROP_FLAG(LRI)|DIRPROP_FLAG(RLI)|DIRPROP_FLAG(FSI)|DIRPROP_FLAG(PDI))

#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_BN_EXPLICIT (DIRPROP_FLAG(BN)|ಮುಖವಾಡ_EXPLICIT)

/* paragraph and segment separators */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_B_S (DIRPROP_FLAG(B)|DIRPROP_FLAG(S))

/* all types that are counted as White Space or Neutral in some steps */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_WS (ಮುಖವಾಡ_B_S|DIRPROP_FLAG(WS)|ಮುಖವಾಡ_BN_EXPLICIT|ಮುಖವಾಡ_ISO)

/* types that are neutrals or could becomes neutrals in (Wn) */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_POSSIBLE_N (DIRPROP_FLAG(ON)|DIRPROP_FLAG(CS)|DIRPROP_FLAG(ES)|DIRPROP_FLAG(ET)|ಮುಖವಾಡ_WS)

/*
 *  These types may be changed to "e",
 *  the embedding type (L or R) of the run,
 *  in the BiDi algorithm (N2)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಖವಾಡ_EMBEDDING (DIRPROP_FLAG(NSM)|ಮುಖವಾಡ_POSSIBLE_N)

/* the dirProp's L and R are defined to 0 and 1 values in UCharDirection */
#ವ್ಯಾಖ್ಯಾನಿಸು GET_LR_FROM_LEVEL(level) ((DirProp)((level)&1))

#ವ್ಯಾಖ್ಯಾನಿಸು IS_DEFAULT_LEVEL(level) ((level)>=0xfe)

/*
 *  The following bit is used for the directional isolate status.
 *  Stack entries corresponding to isolate sequences are greater than ISOLATE.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ISOLATE  0x0100

U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UBiDi *pBiDi, int32_t ಸೂಚ್ಯಂಕ);

#ವ್ಯಾಖ್ಯಾನಿಸು GET_PARALEVEL(ubidi, ಸೂಚ್ಯಂಕ) \
            ((UBiDiLevel)(!(ubidi)->defaultParaLevel || (ಸೂಚ್ಯಂಕ)<(ubidi)->paras[0].ಎಲ್ಲೆ ? \
                         (ubidi)->paraLevel : ubidi_get_Para_Level_At_ಸೂಚ್ಯಂಕ((ubidi), (ಸೂಚ್ಯಂಕ))))

/* number of paras entries allocated initially without malloc */
#ವ್ಯಾಖ್ಯಾನಿಸು SIMPLE_PARAS_ಎಣಿಕೆ      10
/* number of isolate entries allocated initially without malloc */
#ವ್ಯಾಖ್ಯಾನಿಸು SIMPLE_ISOLATES_ಎಣಿಕೆ   5
/* number of isolate run entries for paired brackets allocated initially without malloc */
#ವ್ಯಾಖ್ಯಾನಿಸು SIMPLE_OPENINGS_ಎಣಿಕೆ   20

#ವ್ಯಾಖ್ಯಾನಿಸು CR  0x000D
#ವ್ಯಾಖ್ಯಾನಿಸು LF  0x000A

/* Run structure for reordering --------------------------------------------- */
ಎಣಿಕೆ {
    LRM_BEFORE=1,
    LRM_AFTER=2,
    RLM_BEFORE=4,
    RLM_AFTER=8
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Para {
    int32_t ಎಲ್ಲೆ;
    int32_t level;
} Para;

ಎಣಿಕೆ {                                  /* flags for Opening.flags */
    ಸಿಕ್ಕಿತು_L=DIRPROP_FLAG(L),
    ಸಿಕ್ಕಿತು_R=DIRPROP_FLAG(R)
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Opening {
    int32_t position;                   /* position of opening bracket */
    int32_t ಹೊಂದಿಕೆ;                      /* matching char or -position of closing bracket */
    int32_t contextPos;                 /* position of last strong char found before opening */
    uint16_t flags;                     /* bits for L or R/AL found within the pair */
    UBiDiDirection contextDir;          /* L or R according to last strong char before opening */
    uint8_t filler;                     /* to complete a nice multiple of 4 chars */
} Opening;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Iso_ಸತತ {
    int32_t  contextPos;                /* position of char determining context */
    uint16_t ಆರಂಭ;                     /* index of first opening entry for this run */
    uint16_t ಎಲ್ಲೆ;                     /* index after last opening entry for this run */
    UBiDiLevel level;                   /* level of this run */
    DirProp ಕೊನೆಯ_Strong;                 /* bidi class of last strong char found in this run */
    DirProp ಕೊನೆಯ_ತಳ;                   /* bidi class of last base char found in this run */
    UBiDiDirection contextDir;          /* L or R to use as context for following openings */
} Iso_ಸತತ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Bracket_ದತ್ತಾಂಶ {
    UBiDi   *pBiDi;
    /* array of opening entries which should be enough in most cases; no malloc() */
    Opening simpleOpenings[SIMPLE_OPENINGS_ಎಣಿಕೆ];
    Opening *openings;                  /* pointer to current array of entries */
    int32_t openings_ಎಣಿಕೆ;              /* number of allocated entries */
    int32_t iso_ಸತತ_ಕೊನೆಯ;                 /* index of last used entry */
    /* array of nested isolated sequence entries; can never excess UBIDI_MAX_EXPLICIT_LEVEL
       + 1 for index 0, + 1 for before the first isolated sequence */
    Iso_ಸತತ  isoRuns[UBIDI_MAX_EXPLICIT_LEVEL+2];
    UBool isNumbersSpecial;             /* reordering mode for NUMBERS_SPECIAL */
} Bracket_ದತ್ತಾಂಶ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Isolate {
    int32_t ಆರಂಭ_ON;
    int32_t start1;
    int32_t state;
    int16_t stateImp;
} Isolate;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಸತತ {
    int32_t logical_ಆರಂಭ,   /* first character of the run; b31 indicates even/odd level */
            visual_ಎಲ್ಲೆ,    /* last visual position of the run +1 */
            ಸೇರಿಸು_Remove;   /* if >0, flags for inserting LRM/RLM before/after run,
                               if <0, count of bidi controls within run            */
} ಸತತ;

/* in a Run, logicalStart will get this bit set if the run level is odd */
#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚ್ಯಂಕ_ODD_ಬಿಟ್ (1UL<<31)

#ವ್ಯಾಖ್ಯಾನಿಸು MAKE_ಸೂಚ್ಯಂಕ_ODD_PAIR(ಸೂಚ್ಯಂಕ, level) ((ಸೂಚ್ಯಂಕ)|((int32_t)((level)&1)<<31))
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೂಡಿಸು_ODD_ಬಿಟ್_FROM_LEVEL(x, level)  ((x)|=((int32_t)((level)&1)<<31))
#ವ್ಯಾಖ್ಯಾನಿಸು REMOVE_ODD_ಬಿಟ್(x)                 ((x)&=~ಸೂಚ್ಯಂಕ_ODD_ಬಿಟ್)

#ವ್ಯಾಖ್ಯಾನಿಸು GET_ಸೂಚ್ಯಂಕ(x)   ((x)&~ಸೂಚ್ಯಂಕ_ODD_ಬಿಟ್)
#ವ್ಯಾಖ್ಯಾನಿಸು GET_ODD_ಬಿಟ್(x) ((uint32_t)(x)>>31)
#ವ್ಯಾಖ್ಯಾನಿಸು IS_ODD_ಸತತ(x)  ((UBool)(((x)&ಸೂಚ್ಯಂಕ_ODD_ಬಿಟ್)!=0))
#ವ್ಯಾಖ್ಯಾನಿಸು IS_EVEN_ಸತತ(x) ((UBool)(((x)&ಸೂಚ್ಯಂಕ_ODD_ಬಿಟ್)==0))

U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ);

/** BiDi control code points */
ಎಣಿಕೆ {
    ZWNJ_CHAR=0x200c,
    ZWJ_CHAR,
    LRM_CHAR,
    RLM_CHAR,
    LRE_CHAR=0x202a,
    RLE_CHAR,
    PDF_CHAR,
    LRO_CHAR,
    RLO_CHAR,
    LRI_CHAR=0x2066,
    RLI_CHAR,
    FSI_CHAR,
    PDI_CHAR
};

#ವ್ಯಾಖ್ಯಾನಿಸು IS_BIDI_CONTROL_CHAR(c) (((uint32_t)(c)&0xfffffffc)==ZWNJ_CHAR || (uint32_t)((c)-LRE_CHAR)<5 || (uint32_t)((c)-LRI_CHAR)<4)

/* InsertPoints structure for noting where to put BiDi marks ---------------- */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಬಿಂದು {
    int32_t pos;            /* position in text */
    int32_t flag;           /* flag for LRM/RLM, before/after */
} ಬಿಂದು;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಸೇರಿಸು_Points_2 {
    int32_t ಸಾಮರ್ಥ್ಯ;       /* number of points allocated */
    int32_t ಗಾತ್ರ_3;           /* number of points used */
    int32_t confirmed;      /* number of points confirmed */
    UError_ಸಂಕೇತ error_ಸಂಕೇತ;   /* for eventual memory shortage */
    ಬಿಂದು *points;          /* pointer to array of points */
} ಸೇರಿಸು_Points_2;


/* UBiDi structure ----------------------------------------------------------- */

ರಚನೆ UBiDi {
    /* pointer to parent paragraph object (pointer to self if this object is
     * a paragraph object); set to NULL in a newly opened object; set to a
     * real value after a successful execution of ubidi_setPara or ubidi_setLine
     */
    ಸ್ಥಿರಾಂಕ UBiDi * pParaBiDi;

    /* alias pointer to the current text */
    ಸ್ಥಿರಾಂಕ UChar *ಪಠ್ಯ;

    /* length of the current text */
    int32_t original_ಉದ್ದಳತೆ;

    /* if the UBIDI_OPTION_STREAMING option is set, this is the length
     * of text actually processed by ubidi_setPara, which may be shorter than
     * the original length.
     * Otherwise, it is identical to the original length.
     */
    int32_t ಉದ್ದಳತೆ;

    /* if the UBIDI_OPTION_REMOVE_CONTROLS option is set, and/or
     * marks are allowed to be inserted in one of the reordering mode, the
     * length of the result string may be different from the processed length.
     */
    int32_t ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ;

    /* memory sizes in bytes */
    int32_t dir_Props_ಗಾತ್ರ, ಮಟ್ಟಗಳು_ಗಾತ್ರ, openings_ಗಾತ್ರ, paras_ಗಾತ್ರ, runs_ಗಾತ್ರ, isolates_ಗಾತ್ರ;

    /* allocated memory */
    DirProp *dir_Props_ಸ್ಮರಣೆ;
    UBiDiLevel *ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ;
    Opening *openings_ಸ್ಮರಣೆ;
    Para *paras_ಸ್ಮರಣೆ;
    ಸತತ *runs_ಸ್ಮರಣೆ;
    Isolate *isolates_ಸ್ಮರಣೆ;

    /* indicators for whether memory may be allocated after ubidi_open() */
    UBool may_Allocate_ಪಠ್ಯ, mayAllocateRuns;

    /* arrays with one value per text-character */
    DirProp *dirProps;
    UBiDiLevel *ಮಟ್ಟಗಳು;

    /* are we performing an approximation of the "inverse BiDi" algorithm? */
    UBool isInverse;

    /* are we using the basic algorithm or its variation? */
    UBi_Di_Reordering_ಬಹುಲಕ reordering_ಬಹುಲಕ;

    /* UBIDI_REORDER_xxx values must be ordered so that all the regular
     * logical to visual modes come first, and all inverse BiDi modes
     * come last.
     */
    #ವ್ಯಾಖ್ಯಾನಿಸು UBIDI_REORDER_ಕೊನೆಯ_LOGICAL_ಗಮ್ಯ_VISUAL    UBIDI_REORDER_NUMBERS_SPECIAL

    /* bitmask for reordering options */
    uint32_t reorderingOptions;

    /* must block separators receive level 0? */
    UBool ಕ್ರಮ_Paragraphs_LTR;

    /* the paragraph level */
    UBiDiLevel paraLevel;
    /* original paraLevel when contextual */
    /* must be one of UBIDI_DEFAULT_xxx or 0 if not contextual */
    UBiDiLevel defaultParaLevel;

    /* context data */
    ಸ್ಥಿರಾಂಕ UChar *prologue;
    int32_t pro_ಉದ್ದಳತೆ;
    ಸ್ಥಿರಾಂಕ UChar *epilogue;
    int32_t epi_ಉದ್ದಳತೆ;

    /* the following is set in ubidi_setPara, used in processPropertySeq */
    ಸ್ಥಿರಾಂಕ ರಚನೆ ImpTabPair * pImpTabPair;  /* pointer to levels state table pair */

    /* the overall paragraph or line directionality - see UBiDiDirection */
    UBiDiDirection direction;

    /* flags is a bit set for which directional properties are in the text */
    Flags flags;

    /* lastArabicPos is index to the last AL in the text, -1 if none */
    int32_t ಕೊನೆಯ_Arabic_Pos;

    /* characters after trailingWSStart are WS and are */
    /* implicitly at the paraLevel (rule (L1)) - levels may not reflect that */
    int32_t trailingWSStart;

    /* fields for paragraph handling */
    int32_t para_ಎಣಿಕೆ;                  /* set in getDirProps() */
    /* filled in getDirProps() */
    Para *paras;

    /* for relatively short text, we only need a tiny array of paras (no malloc()) */
    Para simpleParas[SIMPLE_PARAS_ಎಣಿಕೆ];

    /* fields for line reordering */
    int32_t ಸತತ_ಎಣಿಕೆ;     /* ==-1: runs not set up yet */
    ಸತತ *runs;

    /* for non-mixed text, we only need a tiny array of runs (no malloc()) */
    ಸತತ simpleRuns[1];

    /* maximum or current nesting depth of isolate sequences */
    /* Within resolveExplicitLevels() and checkExplicitLevels(), this is the maximal
       nesting encountered.
       Within resolveImplicitLevels(), this is the index of the current isolates
       stack entry. */
    int32_t isolate_ಎಣಿಕೆ;
    Isolate *isolates;

    /* for simple text, have a small stack (no malloc()) */
    Isolate simpleIsolates[SIMPLE_ISOLATES_ಎಣಿಕೆ];

    /* for inverse Bidi with insertion of directional marks */
    ಸೇರಿಸು_Points_2 ಸೇರಿಸು_Points;

    /* for option UBIDI_OPTION_REMOVE_CONTROLS */
    int32_t control_ಎಣಿಕೆ;

    /* for Bidi class callback */
    UBiDiClassCallback *fnClassCallback;    /* action pointer */
    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *coClassCallback;            /* context pointer */
};

#ವ್ಯಾಖ್ಯಾನಿಸು IS_VALID_PARA(x) ((x) && ((x)->pParaBiDi==(x)))
#ವ್ಯಾಖ್ಯಾನಿಸು IS_VALID_PARA_OR_ಸಾಲು(x) ((x) && ((x)->pParaBiDi==(x) || (((x)->pParaBiDi) && (x)->pParaBiDi->pParaBiDi==(x)->pParaBiDi)))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
    DirProp *dir_Props_ಸ್ಮರಣೆ;
    UBiDiLevel *ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ;
    Opening *openings_ಸ್ಮರಣೆ;
    Para *paras_ಸ್ಮರಣೆ;
    ಸತತ *runs_ಸ್ಮರಣೆ;
    Isolate *isolates_ಸ್ಮರಣೆ;
} Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ;

/* Macros for initial checks at function entry */
#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrcode, retvalue) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ((pErrcode)==ಖಾಲಿ || U_FAILURE(*pErrcode)) ಹಿಂತಿರುಗಿ retvalue; \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_IF_NOT_VALID_PARA(bidi, errcode, retvalue) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        ಹಿಂತಿರುಗಿ retvalue; \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_IF_NOT_VALID_PARA_OR_ಸಾಲು(bidi, errcode, retvalue) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ(!IS_VALID_PARA_OR_ಸಾಲು(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        ಹಿಂತಿರುಗಿ retvalue; \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_IF_BAD_RANGE(arg, ಆರಂಭ, ಎಲ್ಲೆ, errcode, retvalue) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ((arg)<(ಆರಂಭ) || (arg)>=(ಎಲ್ಲೆ)) { \
        (errcode)=U_ILLEGAL_ವಾದ_ERROR; \
        ಹಿಂತಿರುಗಿ retvalue; \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrcode) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ((pErrcode)==ಖಾಲಿ || U_FAILURE(*pErrcode)) ಹಿಂತಿರುಗಿ; \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_VOID_IF_NOT_VALID_PARA(bidi, errcode) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        ಹಿಂತಿರುಗಿ; \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_VOID_IF_NOT_VALID_PARA_OR_ಸಾಲು(bidi, errcode) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ(!IS_VALID_PARA_OR_ಸಾಲು(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        ಹಿಂತಿರುಗಿ; \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_VOID_IF_BAD_RANGE(arg, ಆರಂಭ, ಎಲ್ಲೆ, errcode) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ((arg)<(ಆರಂಭ) || (arg)>=(ಎಲ್ಲೆ)) { \
        (errcode)=U_ILLEGAL_ವಾದ_ERROR; \
        ಹಿಂತಿರುಗಿ; \
    } \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

/* helper function to (re)allocate memory if allowed */
U_CFUNC UBool
ubidi_get_ಸ್ಮರಣೆ(Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *p_ಸ್ಮರಣೆ, int32_t *p_ಗಾತ್ರ, UBool mayAllocate, size_t ಗಾತ್ರ_Needed);

/* helper macros for each allocated array in UBiDi */
#ವ್ಯಾಖ್ಯಾನಿಸು get_Dir_Props_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->dir_Props_ಸ್ಮರಣೆ, &(pBiDi)->dir_Props_ಗಾತ್ರ, \
                        (pBiDi)->may_Allocate_ಪಠ್ಯ, (ಉದ್ದಳತೆ))

#ವ್ಯಾಖ್ಯಾನಿಸು get_ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ, &(pBiDi)->ಮಟ್ಟಗಳು_ಗಾತ್ರ, \
                        (pBiDi)->may_Allocate_ಪಠ್ಯ, (ಉದ್ದಳತೆ))

#ವ್ಯಾಖ್ಯಾನಿಸು get_Runs_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->runs_ಸ್ಮರಣೆ, &(pBiDi)->runs_ಗಾತ್ರ, \
                        (pBiDi)->mayAllocateRuns, (ಉದ್ದಳತೆ)*ಗಾತ್ರ(ಸತತ))

/* additional macros used by ubidi_open() - always allow allocation */
#ವ್ಯಾಖ್ಯಾನಿಸು get_Initial_Dir_Props_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->dir_Props_ಸ್ಮರಣೆ, &(pBiDi)->dir_Props_ಗಾತ್ರ, \
                        true, (ಉದ್ದಳತೆ))

#ವ್ಯಾಖ್ಯಾನಿಸು get_Initial_ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->ಮಟ್ಟಗಳು_ಸ್ಮರಣೆ, &(pBiDi)->ಮಟ್ಟಗಳು_ಗಾತ್ರ, \
                        true, (ಉದ್ದಳತೆ))

#ವ್ಯಾಖ್ಯಾನಿಸು get_Initial_Openings_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->openings_ಸ್ಮರಣೆ, &(pBiDi)->openings_ಗಾತ್ರ, \
                        true, (ಉದ್ದಳತೆ)*ಗಾತ್ರ(Opening))

#ವ್ಯಾಖ್ಯಾನಿಸು get_Initial_Paras_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->paras_ಸ್ಮರಣೆ, &(pBiDi)->paras_ಗಾತ್ರ, \
                        true, (ಉದ್ದಳತೆ)*ಗಾತ್ರ(Para))

#ವ್ಯಾಖ್ಯಾನಿಸು get_Initial_Runs_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->runs_ಸ್ಮರಣೆ, &(pBiDi)->runs_ಗಾತ್ರ, \
                        true, (ಉದ್ದಳತೆ)*ಗಾತ್ರ(ಸತತ))

#ವ್ಯಾಖ್ಯಾನಿಸು get_Initial_Isolates_ಸ್ಮರಣೆ(pBiDi, ಉದ್ದಳತೆ) \
        ubidi_get_ಸ್ಮರಣೆ((Bidi_ಸ್ಮರಣೆ_For_ಹಂಚಿಕೆ *)&(pBiDi)->isolates_ಸ್ಮರಣೆ, &(pBiDi)->isolates_ಗಾತ್ರ, \
                        true, (ಉದ್ದಳತೆ)*ಗಾತ್ರ(Isolate))

#ಆದರೆ_ಅಂತ್ಯ
