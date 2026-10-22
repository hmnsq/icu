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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UBIDIIMP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDIIMP_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubidi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubidi_props.h"

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_ਅੰਦਰੂਨੀ_RUNS_ONLY 64

// ICU-20853=ICU-20935 Solaris #defines CS and ES in sys/regset.h
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CS
#   undef CS
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ES
#   undef ES
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint8_t DirProp;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t Flags;

/*  Comparing the description of the BiDi algorithm with this implementation
    is easier with the same names for the BiDi types in the code as there.
    See UCharDirection in uchar.h .
*/
ਗਿਣਤੀ {
    L=  U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ,                /*  0 */
    R=  U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ,                /*  1 */
    EN= U_EUROPEAN_ਸੰਖਿਆ,              /*  2 */
    ES= U_EUROPEAN_ਸੰਖਿਆ_SEPARATOR,    /*  3 */
    ET= U_EUROPEAN_ਸੰਖਿਆ_TERMINATOR,   /*  4 */
    AN= U_ARABIC_ਸੰਖਿਆ,                /*  5 */
    CS= U_COMMON_ਸੰਖਿਆ_SEPARATOR,      /*  6 */
    B=  U_ਖੰਡ_SEPARATOR,              /*  7 */
    S=  U_SEGMENT_SEPARATOR,            /*  8 */
    WS= U_WHITE_SPACE_NEUTRAL,          /*  9 */
    ON= U_OTHER_NEUTRAL,                /* 10 */
    LRE=U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ_EMBEDDING,      /* 11 */
    LRO=U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ_OVERRIDE,       /* 12 */
    AL= U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ARABIC,         /* 13 */
    RLE=U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_EMBEDDING,      /* 14 */
    RLO=U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_OVERRIDE,       /* 15 */
    PDF=U_ਕੱਢੋ_DIRECTIONAL_FORMAT,       /* 16 */
    NSM=U_DIR_NON_SPACING_MARK,         /* 17 */
    BN= U_BOUNDARY_NEUTRAL,             /* 18 */
    FSI=U_ਪਹਿਲਾ_STRONG_ISOLATE,         /* 19 */
    LRI=U_ਖੱਬਾ_ਮੰਜ਼ਿਲ_ਸੱਜਾ_ISOLATE,        /* 20 */
    RLI=U_ਸੱਜਾ_ਮੰਜ਼ਿਲ_ਖੱਬਾ_ISOLATE,        /* 21 */
    PDI=U_ਕੱਢੋ_DIRECTIONAL_ISOLATE,      /* 22 */
    ENL,    /* EN after W7 */           /* 23 */
    ENR,    /* EN not subject to W7 */  /* 24 */
    dir_Prop_ਗਿਣਤੀ
};

/*  Sometimes, bit values are more appropriate
    to deal with directionality properties.
    Abbreviations in these macro names refer to names
    used in the BiDi algorithm.
*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRPROP_FLAG(dir) (1UL<<(dir))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸ਼ੁੱਧ_DIRPROP(prop)  ((prop)&~0xE0)    ?????????????????????????

/* special flag for multiple runs from explicit embedding codes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRPROP_FLAG_MULTI_RUNS (1UL<<31)

/* are there any characters that are LTR or RTL? */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_LTR (DIRPROP_FLAG(L)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(ENL)|DIRPROP_FLAG(ENR)|DIRPROP_FLAG(AN)|DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(LRI))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_RTL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(RLI))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_R_AL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_STRONG_EN_AN (DIRPROP_FLAG(L)|DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(AN))

/* explicit embedding codes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_EXPLICIT (DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(PDF))

/* explicit isolate codes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_ISO (DIRPROP_FLAG(LRI)|DIRPROP_FLAG(RLI)|DIRPROP_FLAG(FSI)|DIRPROP_FLAG(PDI))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_BN_EXPLICIT (DIRPROP_FLAG(BN)|ਨਕਾਬ_EXPLICIT)

/* paragraph and segment separators */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_B_S (DIRPROP_FLAG(B)|DIRPROP_FLAG(S))

/* all types that are counted as White Space or Neutral in some steps */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_WS (ਨਕਾਬ_B_S|DIRPROP_FLAG(WS)|ਨਕਾਬ_BN_EXPLICIT|ਨਕਾਬ_ISO)

/* types that are neutrals or could becomes neutrals in (Wn) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_POSSIBLE_N (DIRPROP_FLAG(ON)|DIRPROP_FLAG(CS)|DIRPROP_FLAG(ES)|DIRPROP_FLAG(ET)|ਨਕਾਬ_WS)

/*
 *  These types may be changed to "e",
 *  the embedding type (L or R) of the run,
 *  in the BiDi algorithm (N2)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਕਾਬ_EMBEDDING (DIRPROP_FLAG(NSM)|ਨਕਾਬ_POSSIBLE_N)

/* the dirProp's L and R are defined to 0 and 1 values in UCharDirection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_LR_FROM_LEVEL(level) ((DirProp)((level)&1))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_DEFAULT_LEVEL(level) ((level)>=0xfe)

/*
 *  The following bit is used for the directional isolate status.
 *  Stack entries corresponding to isolate sequences are greater than ISOLATE.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISOLATE  0x0100

U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_ਸੂਚਕਾਂਕ(ਅਚਲ UBiDi *pBiDi, int32_t ਸੂਚਕਾਂਕ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_PARALEVEL(ubidi, ਸੂਚਕਾਂਕ) \
            ((UBiDiLevel)(!(ubidi)->defaultParaLevel || (ਸੂਚਕਾਂਕ)<(ubidi)->paras[0].ਹੱਦਬੰਦੀ ? \
                         (ubidi)->paraLevel : ubidi_get_Para_Level_At_ਸੂਚਕਾਂਕ((ubidi), (ਸੂਚਕਾਂਕ))))

/* number of paras entries allocated initially without malloc */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIMPLE_PARAS_ਗਿਣਤੀ      10
/* number of isolate entries allocated initially without malloc */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIMPLE_ISOLATES_ਗਿਣਤੀ   5
/* number of isolate run entries for paired brackets allocated initially without malloc */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIMPLE_OPENINGS_ਗਿਣਤੀ   20

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CR  0x000D
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LF  0x000A

/* Run structure for reordering --------------------------------------------- */
ਗਿਣਤੀ {
    LRM_BEFORE=1,
    LRM_AFTER=2,
    RLM_BEFORE=4,
    RLM_AFTER=8
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Para {
    int32_t ਹੱਦਬੰਦੀ;
    int32_t level;
} Para;

ਗਿਣਤੀ {                                  /* flags for Opening.flags */
    ਮਿਲਿਆ_L=DIRPROP_FLAG(L),
    ਮਿਲਿਆ_R=DIRPROP_FLAG(R)
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Opening {
    int32_t position;                   /* position of opening bracket */
    int32_t ਮੇਲ;                      /* matching char or -position of closing bracket */
    int32_t contextPos;                 /* position of last strong char found before opening */
    uint16_t flags;                     /* bits for L or R/AL found within the pair */
    UBiDiDirection contextDir;          /* L or R according to last strong char before opening */
    uint8_t filler;                     /* to complete a nice multiple of 4 chars */
} Opening;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Iso_ਲੜੀਵਾਰ {
    int32_t  contextPos;                /* position of char determining context */
    uint16_t ਸ਼ੁਰੂ;                     /* index of first opening entry for this run */
    uint16_t ਹੱਦਬੰਦੀ;                     /* index after last opening entry for this run */
    UBiDiLevel level;                   /* level of this run */
    DirProp ਆਖਰੀ_Strong;                 /* bidi class of last strong char found in this run */
    DirProp ਆਖਰੀ_ਅਧਾਰ;                   /* bidi class of last base char found in this run */
    UBiDiDirection contextDir;          /* L or R to use as context for following openings */
} Iso_ਲੜੀਵਾਰ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Bracket_ਅੰਕੜੇ {
    UBiDi   *pBiDi;
    /* array of opening entries which should be enough in most cases; no malloc() */
    Opening simpleOpenings[SIMPLE_OPENINGS_ਗਿਣਤੀ];
    Opening *openings;                  /* pointer to current array of entries */
    int32_t openings_ਗਿਣਤੀ;              /* number of allocated entries */
    int32_t iso_ਲੜੀਵਾਰ_ਆਖਰੀ;                 /* index of last used entry */
    /* array of nested isolated sequence entries; can never excess UBIDI_MAX_EXPLICIT_LEVEL
       + 1 for index 0, + 1 for before the first isolated sequence */
    Iso_ਲੜੀਵਾਰ  isoRuns[UBIDI_MAX_EXPLICIT_LEVEL+2];
    UBool isNumbersSpecial;             /* reordering mode for NUMBERS_SPECIAL */
} Bracket_ਅੰਕੜੇ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Isolate {
    int32_t ਸ਼ੁਰੂ_ON;
    int32_t start1;
    int32_t state;
    int16_t stateImp;
} Isolate;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਲੜੀਵਾਰ {
    int32_t logical_ਸ਼ੁਰੂ,   /* first character of the run; b31 indicates even/odd level */
            visual_ਹੱਦਬੰਦੀ,    /* last visual position of the run +1 */
            ਪਾਓ_Remove;   /* if >0, flags for inserting LRM/RLM before/after run,
                               if <0, count of bidi controls within run            */
} ਲੜੀਵਾਰ;

/* in a Run, logicalStart will get this bit set if the run level is odd */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕਾਂਕ_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ (1UL<<31)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAKE_ਸੂਚਕਾਂਕ_ODD_PAIR(ਸੂਚਕਾਂਕ, level) ((ਸੂਚਕਾਂਕ)|((int32_t)((level)&1)<<31))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਜੋੜੋ_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ_FROM_LEVEL(x, level)  ((x)|=((int32_t)((level)&1)<<31))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REMOVE_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(x)                 ((x)&=~ਸੂਚਕਾਂਕ_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ਸੂਚਕਾਂਕ(x)   ((x)&~ਸੂਚਕਾਂਕ_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ(x) ((uint32_t)(x)>>31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_ODD_ਲੜੀਵਾਰ(x)  ((UBool)(((x)&ਸੂਚਕਾਂਕ_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ)!=0))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_EVEN_ਲੜੀਵਾਰ(x) ((UBool)(((x)&ਸੂਚਕਾਂਕ_ODD_ਦੋਆਧਾਰੀ_ਅੰਕ)==0))

U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UError_ਕੋਡ *p_Error_ਕੋਡ);

/** BiDi control code points */
ਗਿਣਤੀ {
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_BIDI_CONTROL_CHAR(c) (((uint32_t)(c)&0xfffffffc)==ZWNJ_CHAR || (uint32_t)((c)-LRE_CHAR)<5 || (uint32_t)((c)-LRI_CHAR)<4)

/* InsertPoints structure for noting where to put BiDi marks ---------------- */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਬਿੰਦੂ {
    int32_t pos;            /* position in text */
    int32_t flag;           /* flag for LRM/RLM, before/after */
} ਬਿੰਦੂ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਪਾਓ_Points_2 {
    int32_t ਸਮਰੱਥਾ;       /* number of points allocated */
    int32_t ਮਾਪ_2;           /* number of points used */
    int32_t confirmed;      /* number of points confirmed */
    UError_ਕੋਡ error_ਕੋਡ;   /* for eventual memory shortage */
    ਬਿੰਦੂ *points;          /* pointer to array of points */
} ਪਾਓ_Points_2;


/* UBiDi structure ----------------------------------------------------------- */

ਬਣਤਰ UBiDi {
    /* pointer to parent paragraph object (pointer to self if this object is
     * a paragraph object); set to NULL in a newly opened object; set to a
     * real value after a successful execution of ubidi_setPara or ubidi_setLine
     */
    ਅਚਲ UBiDi * pParaBiDi;

    /* alias pointer to the current text */
    ਅਚਲ UChar *ਪਾਠ;

    /* length of the current text */
    int32_t original_ਲੰਮਾਈ;

    /* if the UBIDI_OPTION_STREAMING option is set, this is the length
     * of text actually processed by ubidi_setPara, which may be shorter than
     * the original length.
     * Otherwise, it is identical to the original length.
     */
    int32_t ਲੰਮਾਈ;

    /* if the UBIDI_OPTION_REMOVE_CONTROLS option is set, and/or
     * marks are allowed to be inserted in one of the reordering mode, the
     * length of the result string may be different from the processed length.
     */
    int32_t ਨਤੀਜਾ_ਲੰਮਾਈ;

    /* memory sizes in bytes */
    int32_t dir_Props_ਮਾਪ, ਪੱਧਰ_ਮਾਪ, openings_ਮਾਪ, paras_ਮਾਪ, runs_ਮਾਪ, isolates_ਮਾਪ;

    /* allocated memory */
    DirProp *dir_Props_ਸਮ੍ਰਿਤੀ;
    UBiDiLevel *ਪੱਧਰ_ਸਮ੍ਰਿਤੀ;
    Opening *openings_ਸਮ੍ਰਿਤੀ;
    Para *paras_ਸਮ੍ਰਿਤੀ;
    ਲੜੀਵਾਰ *runs_ਸਮ੍ਰਿਤੀ;
    Isolate *isolates_ਸਮ੍ਰਿਤੀ;

    /* indicators for whether memory may be allocated after ubidi_open() */
    UBool may_Allocate_ਪਾਠ, mayAllocateRuns;

    /* arrays with one value per text-character */
    DirProp *dirProps;
    UBiDiLevel *ਪੱਧਰ;

    /* are we performing an approximation of the "inverse BiDi" algorithm? */
    UBool isInverse;

    /* are we using the basic algorithm or its variation? */
    UBi_Di_Reordering_ਬਹੁਲਕ reordering_ਬਹੁਲਕ;

    /* UBIDI_REORDER_xxx values must be ordered so that all the regular
     * logical to visual modes come first, and all inverse BiDi modes
     * come last.
     */
    #ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBIDI_REORDER_ਆਖਰੀ_LOGICAL_ਮੰਜ਼ਿਲ_VISUAL    UBIDI_REORDER_NUMBERS_SPECIAL

    /* bitmask for reordering options */
    uint32_t reorderingOptions;

    /* must block separators receive level 0? */
    UBool ਕ੍ਰਮ_Paragraphs_LTR;

    /* the paragraph level */
    UBiDiLevel paraLevel;
    /* original paraLevel when contextual */
    /* must be one of UBIDI_DEFAULT_xxx or 0 if not contextual */
    UBiDiLevel defaultParaLevel;

    /* context data */
    ਅਚਲ UChar *prologue;
    int32_t pro_ਲੰਮਾਈ;
    ਅਚਲ UChar *epilogue;
    int32_t epi_ਲੰਮਾਈ;

    /* the following is set in ubidi_setPara, used in processPropertySeq */
    ਅਚਲ ਬਣਤਰ ImpTabPair * pImpTabPair;  /* pointer to levels state table pair */

    /* the overall paragraph or line directionality - see UBiDiDirection */
    UBiDiDirection direction;

    /* flags is a bit set for which directional properties are in the text */
    Flags flags;

    /* lastArabicPos is index to the last AL in the text, -1 if none */
    int32_t ਆਖਰੀ_Arabic_Pos;

    /* characters after trailingWSStart are WS and are */
    /* implicitly at the paraLevel (rule (L1)) - levels may not reflect that */
    int32_t trailingWSStart;

    /* fields for paragraph handling */
    int32_t para_ਗਿਣਤੀ;                  /* set in getDirProps() */
    /* filled in getDirProps() */
    Para *paras;

    /* for relatively short text, we only need a tiny array of paras (no malloc()) */
    Para simpleParas[SIMPLE_PARAS_ਗਿਣਤੀ];

    /* fields for line reordering */
    int32_t ਲੜੀਵਾਰ_ਗਿਣਤੀ;     /* ==-1: runs not set up yet */
    ਲੜੀਵਾਰ *runs;

    /* for non-mixed text, we only need a tiny array of runs (no malloc()) */
    ਲੜੀਵਾਰ simpleRuns[1];

    /* maximum or current nesting depth of isolate sequences */
    /* Within resolveExplicitLevels() and checkExplicitLevels(), this is the maximal
       nesting encountered.
       Within resolveImplicitLevels(), this is the index of the current isolates
       stack entry. */
    int32_t isolate_ਗਿਣਤੀ;
    Isolate *isolates;

    /* for simple text, have a small stack (no malloc()) */
    Isolate simpleIsolates[SIMPLE_ISOLATES_ਗਿਣਤੀ];

    /* for inverse Bidi with insertion of directional marks */
    ਪਾਓ_Points_2 ਪਾਓ_Points;

    /* for option UBIDI_OPTION_REMOVE_CONTROLS */
    int32_t control_ਗਿਣਤੀ;

    /* for Bidi class callback */
    UBiDiClassCallback *fnClassCallback;    /* action pointer */
    ਅਚਲ ਖਾਲੀ *coClassCallback;            /* context pointer */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_VALID_PARA(x) ((x) && ((x)->pParaBiDi==(x)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_VALID_PARA_OR_ਸਤਰ(x) ((x) && ((x)->pParaBiDi==(x) || (((x)->pParaBiDi) && (x)->pParaBiDi->pParaBiDi==(x)->pParaBiDi)))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
    DirProp *dir_Props_ਸਮ੍ਰਿਤੀ;
    UBiDiLevel *ਪੱਧਰ_ਸਮ੍ਰਿਤੀ;
    Opening *openings_ਸਮ੍ਰਿਤੀ;
    Para *paras_ਸਮ੍ਰਿਤੀ;
    ਲੜੀਵਾਰ *runs_ਸਮ੍ਰਿਤੀ;
    Isolate *isolates_ਸਮ੍ਰਿਤੀ;
} Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ;

/* Macros for initial checks at function entry */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrcode, retvalue) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ((pErrcode)==ਨਿਰਾ || U_FAILURE(*pErrcode)) ਵਾਪਸ retvalue; \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_IF_NOT_VALID_PARA(bidi, errcode, retvalue) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        ਵਾਪਸ retvalue; \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_IF_NOT_VALID_PARA_OR_ਸਤਰ(bidi, errcode, retvalue) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ(!IS_VALID_PARA_OR_ਸਤਰ(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        ਵਾਪਸ retvalue; \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_IF_BAD_RANGE(arg, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, errcode, retvalue) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ((arg)<(ਸ਼ੁਰੂ) || (arg)>=(ਹੱਦਬੰਦੀ)) { \
        (errcode)=U_ILLEGAL_ਦਲੀਲ_ERROR; \
        ਵਾਪਸ retvalue; \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrcode) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ((pErrcode)==ਨਿਰਾ || U_FAILURE(*pErrcode)) ਵਾਪਸ; \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_VOID_IF_NOT_VALID_PARA(bidi, errcode) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        ਵਾਪਸ; \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_VOID_IF_NOT_VALID_PARA_OR_ਸਤਰ(bidi, errcode) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ(!IS_VALID_PARA_OR_ਸਤਰ(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        ਵਾਪਸ; \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_VOID_IF_BAD_RANGE(arg, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, errcode) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ((arg)<(ਸ਼ੁਰੂ) || (arg)>=(ਹੱਦਬੰਦੀ)) { \
        (errcode)=U_ILLEGAL_ਦਲੀਲ_ERROR; \
        ਵਾਪਸ; \
    } \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

/* helper function to (re)allocate memory if allowed */
U_CFUNC UBool
ubidi_get_ਸਮ੍ਰਿਤੀ(Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *p_ਸਮ੍ਰਿਤੀ, int32_t *p_ਮਾਪ, UBool mayAllocate, size_t ਮਾਪ_Needed);

/* helper macros for each allocated array in UBiDi */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_Dir_Props_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->dir_Props_ਸਮ੍ਰਿਤੀ, &(pBiDi)->dir_Props_ਮਾਪ, \
                        (pBiDi)->may_Allocate_ਪਾਠ, (ਲੰਮਾਈ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_ਪੱਧਰ_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->ਪੱਧਰ_ਸਮ੍ਰਿਤੀ, &(pBiDi)->ਪੱਧਰ_ਮਾਪ, \
                        (pBiDi)->may_Allocate_ਪਾਠ, (ਲੰਮਾਈ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_Runs_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->runs_ਸਮ੍ਰਿਤੀ, &(pBiDi)->runs_ਮਾਪ, \
                        (pBiDi)->mayAllocateRuns, (ਲੰਮਾਈ)*ਆਕਾਰ(ਲੜੀਵਾਰ))

/* additional macros used by ubidi_open() - always allow allocation */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_Initial_Dir_Props_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->dir_Props_ਸਮ੍ਰਿਤੀ, &(pBiDi)->dir_Props_ਮਾਪ, \
                        true, (ਲੰਮਾਈ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_Initial_ਪੱਧਰ_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->ਪੱਧਰ_ਸਮ੍ਰਿਤੀ, &(pBiDi)->ਪੱਧਰ_ਮਾਪ, \
                        true, (ਲੰਮਾਈ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_Initial_Openings_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->openings_ਸਮ੍ਰਿਤੀ, &(pBiDi)->openings_ਮਾਪ, \
                        true, (ਲੰਮਾਈ)*ਆਕਾਰ(Opening))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_Initial_Paras_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->paras_ਸਮ੍ਰਿਤੀ, &(pBiDi)->paras_ਮਾਪ, \
                        true, (ਲੰਮਾਈ)*ਆਕਾਰ(Para))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_Initial_Runs_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->runs_ਸਮ੍ਰਿਤੀ, &(pBiDi)->runs_ਮਾਪ, \
                        true, (ਲੰਮਾਈ)*ਆਕਾਰ(ਲੜੀਵਾਰ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_Initial_Isolates_ਸਮ੍ਰਿਤੀ(pBiDi, ਲੰਮਾਈ) \
        ubidi_get_ਸਮ੍ਰਿਤੀ((Bidi_ਸਮ੍ਰਿਤੀ_For_ਵੰਡ *)&(pBiDi)->isolates_ਸਮ੍ਰਿਤੀ, &(pBiDi)->isolates_ਮਾਪ, \
                        true, (ਲੰਮਾਈ)*ਆਕਾਰ(Isolate))

#ਜੇ_ਅੰਤ
