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

#నిర్వచితం_కాకపోతే UBIDIIMP_H
#నిర్వచించు UBIDIIMP_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ubidi.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "ubidi_props.h"

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
#నిర్వచించు UBIDI_అంతర్గత_RUNS_ONLY 64

// ICU-20853=ICU-20935 Solaris #defines CS and ES in sys/regset.h
#నిర్వచితమైతే CS
#   undef CS
#అయితే_ముగింపు
#నిర్వచితమైతే ES
#   undef ES
#అయితే_ముగింపు

రకనిర్వచనం uint8_t DirProp;
రకనిర్వచనం uint32_t Flags;

/*  Comparing the description of the BiDi algorithm with this implementation
    is easier with the same names for the BiDi types in the code as there.
    See UCharDirection in uchar.h .
*/
గణన {
    L=  U_ఎడమ_గమ్యం_కుడి,                /*  0 */
    R=  U_కుడి_గమ్యం_ఎడమ,                /*  1 */
    EN= U_EUROPEAN_సంఖ్య,              /*  2 */
    ES= U_EUROPEAN_సంఖ్య_SEPARATOR,    /*  3 */
    ET= U_EUROPEAN_సంఖ్య_TERMINATOR,   /*  4 */
    AN= U_ARABIC_సంఖ్య,                /*  5 */
    CS= U_COMMON_సంఖ్య_SEPARATOR,      /*  6 */
    B=  U_ఖండం_SEPARATOR,              /*  7 */
    S=  U_SEGMENT_SEPARATOR,            /*  8 */
    WS= U_WHITE_SPACE_NEUTRAL,          /*  9 */
    ON= U_OTHER_NEUTRAL,                /* 10 */
    LRE=U_ఎడమ_గమ్యం_కుడి_EMBEDDING,      /* 11 */
    LRO=U_ఎడమ_గమ్యం_కుడి_OVERRIDE,       /* 12 */
    AL= U_కుడి_గమ్యం_ఎడమ_ARABIC,         /* 13 */
    RLE=U_కుడి_గమ్యం_ఎడమ_EMBEDDING,      /* 14 */
    RLO=U_కుడి_గమ్యం_ఎడమ_OVERRIDE,       /* 15 */
    PDF=U_తీయి_DIRECTIONAL_FORMAT,       /* 16 */
    NSM=U_DIR_NON_SPACING_MARK,         /* 17 */
    BN= U_BOUNDARY_NEUTRAL,             /* 18 */
    FSI=U_మొదటి_STRONG_ISOLATE,         /* 19 */
    LRI=U_ఎడమ_గమ్యం_కుడి_ISOLATE,        /* 20 */
    RLI=U_కుడి_గమ్యం_ఎడమ_ISOLATE,        /* 21 */
    PDI=U_తీయి_DIRECTIONAL_ISOLATE,      /* 22 */
    ENL,    /* EN after W7 */           /* 23 */
    ENR,    /* EN not subject to W7 */  /* 24 */
    dir_Prop_లెక్క
};

/*  Sometimes, bit values are more appropriate
    to deal with directionality properties.
    Abbreviations in these macro names refer to names
    used in the BiDi algorithm.
*/
#నిర్వచించు DIRPROP_FLAG(dir) (1UL<<(dir))
#నిర్వచించు శుద్ధ_DIRPROP(prop)  ((prop)&~0xE0)    ?????????????????????????

/* special flag for multiple runs from explicit embedding codes */
#నిర్వచించు DIRPROP_FLAG_MULTI_RUNS (1UL<<31)

/* are there any characters that are LTR or RTL? */
#నిర్వచించు ముసుగు_LTR (DIRPROP_FLAG(L)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(ENL)|DIRPROP_FLAG(ENR)|DIRPROP_FLAG(AN)|DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(LRI))
#నిర్వచించు ముసుగు_RTL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(RLI))
#నిర్వచించు ముసుగు_R_AL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL))
#నిర్వచించు ముసుగు_STRONG_EN_AN (DIRPROP_FLAG(L)|DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(AN))

/* explicit embedding codes */
#నిర్వచించు ముసుగు_EXPLICIT (DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(PDF))

/* explicit isolate codes */
#నిర్వచించు ముసుగు_ISO (DIRPROP_FLAG(LRI)|DIRPROP_FLAG(RLI)|DIRPROP_FLAG(FSI)|DIRPROP_FLAG(PDI))

#నిర్వచించు ముసుగు_BN_EXPLICIT (DIRPROP_FLAG(BN)|ముసుగు_EXPLICIT)

/* paragraph and segment separators */
#నిర్వచించు ముసుగు_B_S (DIRPROP_FLAG(B)|DIRPROP_FLAG(S))

/* all types that are counted as White Space or Neutral in some steps */
#నిర్వచించు ముసుగు_WS (ముసుగు_B_S|DIRPROP_FLAG(WS)|ముసుగు_BN_EXPLICIT|ముసుగు_ISO)

/* types that are neutrals or could becomes neutrals in (Wn) */
#నిర్వచించు ముసుగు_POSSIBLE_N (DIRPROP_FLAG(ON)|DIRPROP_FLAG(CS)|DIRPROP_FLAG(ES)|DIRPROP_FLAG(ET)|ముసుగు_WS)

/*
 *  These types may be changed to "e",
 *  the embedding type (L or R) of the run,
 *  in the BiDi algorithm (N2)
 */
#నిర్వచించు ముసుగు_EMBEDDING (DIRPROP_FLAG(NSM)|ముసుగు_POSSIBLE_N)

/* the dirProp's L and R are defined to 0 and 1 values in UCharDirection */
#నిర్వచించు GET_LR_FROM_LEVEL(level) ((DirProp)((level)&1))

#నిర్వచించు IS_DEFAULT_LEVEL(level) ((level)>=0xfe)

/*
 *  The following bit is used for the directional isolate status.
 *  Stack entries corresponding to isolate sequences are greater than ISOLATE.
 */
#నిర్వచించు ISOLATE  0x0100

U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_సూచీ(స్థిరరాశి UBiDi *pBiDi, int32_t సూచీ);

#నిర్వచించు GET_PARALEVEL(ubidi, సూచీ) \
            ((UBiDiLevel)(!(ubidi)->defaultParaLevel || (సూచీ)<(ubidi)->paras[0].హద్దు ? \
                         (ubidi)->paraLevel : ubidi_get_Para_Level_At_సూచీ((ubidi), (సూచీ))))

/* number of paras entries allocated initially without malloc */
#నిర్వచించు SIMPLE_PARAS_లెక్క      10
/* number of isolate entries allocated initially without malloc */
#నిర్వచించు SIMPLE_ISOLATES_లెక్క   5
/* number of isolate run entries for paired brackets allocated initially without malloc */
#నిర్వచించు SIMPLE_OPENINGS_లెక్క   20

#నిర్వచించు CR  0x000D
#నిర్వచించు LF  0x000A

/* Run structure for reordering --------------------------------------------- */
గణన {
    LRM_BEFORE=1,
    LRM_AFTER=2,
    RLM_BEFORE=4,
    RLM_AFTER=8
};

రకనిర్వచనం నిర్మాణం Para {
    int32_t హద్దు;
    int32_t level;
} Para;

గణన {                                  /* flags for Opening.flags */
    దొరికింది_L=DIRPROP_FLAG(L),
    దొరికింది_R=DIRPROP_FLAG(R)
};

రకనిర్వచనం నిర్మాణం Opening {
    int32_t position;                   /* position of opening bracket */
    int32_t సరిపోలిక;                      /* matching char or -position of closing bracket */
    int32_t contextPos;                 /* position of last strong char found before opening */
    uint16_t flags;                     /* bits for L or R/AL found within the pair */
    UBiDiDirection contextDir;          /* L or R according to last strong char before opening */
    uint8_t filler;                     /* to complete a nice multiple of 4 chars */
} Opening;

రకనిర్వచనం నిర్మాణం Iso_నిరంతరం {
    int32_t  contextPos;                /* position of char determining context */
    uint16_t ప్రారంభం;                     /* index of first opening entry for this run */
    uint16_t హద్దు;                     /* index after last opening entry for this run */
    UBiDiLevel level;                   /* level of this run */
    DirProp చివరి_Strong;                 /* bidi class of last strong char found in this run */
    DirProp చివరి_పునాది;                   /* bidi class of last base char found in this run */
    UBiDiDirection contextDir;          /* L or R to use as context for following openings */
} Iso_నిరంతరం;

రకనిర్వచనం నిర్మాణం Bracket_దత్తాంశం {
    UBiDi   *pBiDi;
    /* array of opening entries which should be enough in most cases; no malloc() */
    Opening simpleOpenings[SIMPLE_OPENINGS_లెక్క];
    Opening *openings;                  /* pointer to current array of entries */
    int32_t openings_లెక్క;              /* number of allocated entries */
    int32_t iso_నిరంతరం_చివరి;                 /* index of last used entry */
    /* array of nested isolated sequence entries; can never excess UBIDI_MAX_EXPLICIT_LEVEL
       + 1 for index 0, + 1 for before the first isolated sequence */
    Iso_నిరంతరం  isoRuns[UBIDI_MAX_EXPLICIT_LEVEL+2];
    UBool isNumbersSpecial;             /* reordering mode for NUMBERS_SPECIAL */
} Bracket_దత్తాంశం;

రకనిర్వచనం నిర్మాణం Isolate {
    int32_t ప్రారంభం_ON;
    int32_t start1;
    int32_t state;
    int16_t stateImp;
} Isolate;

రకనిర్వచనం నిర్మాణం నిరంతరం {
    int32_t logical_ప్రారంభం,   /* first character of the run; b31 indicates even/odd level */
            visual_హద్దు,    /* last visual position of the run +1 */
            చొప్పించు_Remove;   /* if >0, flags for inserting LRM/RLM before/after run,
                               if <0, count of bidi controls within run            */
} నిరంతరం;

/* in a Run, logicalStart will get this bit set if the run level is odd */
#నిర్వచించు సూచీ_ODD_బిట్టు (1UL<<31)

#నిర్వచించు MAKE_సూచీ_ODD_PAIR(సూచీ, level) ((సూచీ)|((int32_t)((level)&1)<<31))
#నిర్వచించు కూడు_ODD_బిట్టు_FROM_LEVEL(x, level)  ((x)|=((int32_t)((level)&1)<<31))
#నిర్వచించు REMOVE_ODD_బిట్టు(x)                 ((x)&=~సూచీ_ODD_బిట్టు)

#నిర్వచించు GET_సూచీ(x)   ((x)&~సూచీ_ODD_బిట్టు)
#నిర్వచించు GET_ODD_బిట్టు(x) ((uint32_t)(x)>>31)
#నిర్వచించు IS_ODD_నిరంతరం(x)  ((UBool)(((x)&సూచీ_ODD_బిట్టు)!=0))
#నిర్వచించు IS_EVEN_నిరంతరం(x) ((UBool)(((x)&సూచీ_ODD_బిట్టు)==0))

U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UError_సంకేతం *p_Error_సంకేతం);

/** BiDi control code points */
గణన {
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

#నిర్వచించు IS_BIDI_CONTROL_CHAR(c) (((uint32_t)(c)&0xfffffffc)==ZWNJ_CHAR || (uint32_t)((c)-LRE_CHAR)<5 || (uint32_t)((c)-LRI_CHAR)<4)

/* InsertPoints structure for noting where to put BiDi marks ---------------- */

రకనిర్వచనం నిర్మాణం బిందువు {
    int32_t pos;            /* position in text */
    int32_t flag;           /* flag for LRM/RLM, before/after */
} బిందువు;

రకనిర్వచనం నిర్మాణం చొప్పించు_Points_2 {
    int32_t సామర్థ్యం;       /* number of points allocated */
    int32_t కొలత_2;           /* number of points used */
    int32_t confirmed;      /* number of points confirmed */
    UError_సంకేతం error_సంకేతం;   /* for eventual memory shortage */
    బిందువు *points;          /* pointer to array of points */
} చొప్పించు_Points_2;


/* UBiDi structure ----------------------------------------------------------- */

నిర్మాణం UBiDi {
    /* pointer to parent paragraph object (pointer to self if this object is
     * a paragraph object); set to NULL in a newly opened object; set to a
     * real value after a successful execution of ubidi_setPara or ubidi_setLine
     */
    స్థిరరాశి UBiDi * pParaBiDi;

    /* alias pointer to the current text */
    స్థిరరాశి UChar *పాఠ్యం;

    /* length of the current text */
    int32_t original_నిడివి;

    /* if the UBIDI_OPTION_STREAMING option is set, this is the length
     * of text actually processed by ubidi_setPara, which may be shorter than
     * the original length.
     * Otherwise, it is identical to the original length.
     */
    int32_t నిడివి;

    /* if the UBIDI_OPTION_REMOVE_CONTROLS option is set, and/or
     * marks are allowed to be inserted in one of the reordering mode, the
     * length of the result string may be different from the processed length.
     */
    int32_t ఫలితం_నిడివి;

    /* memory sizes in bytes */
    int32_t dir_Props_కొలత, స్థాయిలు_కొలత, openings_కొలత, paras_కొలత, runs_కొలత, isolates_కొలత;

    /* allocated memory */
    DirProp *dir_Props_జ్ఞాపకం;
    UBiDiLevel *స్థాయిలు_జ్ఞాపకం;
    Opening *openings_జ్ఞాపకం;
    Para *paras_జ్ఞాపకం;
    నిరంతరం *runs_జ్ఞాపకం;
    Isolate *isolates_జ్ఞాపకం;

    /* indicators for whether memory may be allocated after ubidi_open() */
    UBool may_Allocate_పాఠ్యం, mayAllocateRuns;

    /* arrays with one value per text-character */
    DirProp *dirProps;
    UBiDiLevel *స్థాయిలు;

    /* are we performing an approximation of the "inverse BiDi" algorithm? */
    UBool isInverse;

    /* are we using the basic algorithm or its variation? */
    UBi_Di_Reordering_బహుళకం reordering_బహుళకం;

    /* UBIDI_REORDER_xxx values must be ordered so that all the regular
     * logical to visual modes come first, and all inverse BiDi modes
     * come last.
     */
    #నిర్వచించు UBIDI_REORDER_చివరి_LOGICAL_గమ్యం_VISUAL    UBIDI_REORDER_NUMBERS_SPECIAL

    /* bitmask for reordering options */
    uint32_t reorderingOptions;

    /* must block separators receive level 0? */
    UBool క్రమం_Paragraphs_LTR;

    /* the paragraph level */
    UBiDiLevel paraLevel;
    /* original paraLevel when contextual */
    /* must be one of UBIDI_DEFAULT_xxx or 0 if not contextual */
    UBiDiLevel defaultParaLevel;

    /* context data */
    స్థిరరాశి UChar *prologue;
    int32_t pro_నిడివి;
    స్థిరరాశి UChar *epilogue;
    int32_t epi_నిడివి;

    /* the following is set in ubidi_setPara, used in processPropertySeq */
    స్థిరరాశి నిర్మాణం ImpTabPair * pImpTabPair;  /* pointer to levels state table pair */

    /* the overall paragraph or line directionality - see UBiDiDirection */
    UBiDiDirection direction;

    /* flags is a bit set for which directional properties are in the text */
    Flags flags;

    /* lastArabicPos is index to the last AL in the text, -1 if none */
    int32_t చివరి_Arabic_Pos;

    /* characters after trailingWSStart are WS and are */
    /* implicitly at the paraLevel (rule (L1)) - levels may not reflect that */
    int32_t trailingWSStart;

    /* fields for paragraph handling */
    int32_t para_లెక్క;                  /* set in getDirProps() */
    /* filled in getDirProps() */
    Para *paras;

    /* for relatively short text, we only need a tiny array of paras (no malloc()) */
    Para simpleParas[SIMPLE_PARAS_లెక్క];

    /* fields for line reordering */
    int32_t నిరంతరం_లెక్క;     /* ==-1: runs not set up yet */
    నిరంతరం *runs;

    /* for non-mixed text, we only need a tiny array of runs (no malloc()) */
    నిరంతరం simpleRuns[1];

    /* maximum or current nesting depth of isolate sequences */
    /* Within resolveExplicitLevels() and checkExplicitLevels(), this is the maximal
       nesting encountered.
       Within resolveImplicitLevels(), this is the index of the current isolates
       stack entry. */
    int32_t isolate_లెక్క;
    Isolate *isolates;

    /* for simple text, have a small stack (no malloc()) */
    Isolate simpleIsolates[SIMPLE_ISOLATES_లెక్క];

    /* for inverse Bidi with insertion of directional marks */
    చొప్పించు_Points_2 చొప్పించు_Points;

    /* for option UBIDI_OPTION_REMOVE_CONTROLS */
    int32_t control_లెక్క;

    /* for Bidi class callback */
    UBiDiClassCallback *fnClassCallback;    /* action pointer */
    స్థిరరాశి శూన్య *coClassCallback;            /* context pointer */
};

#నిర్వచించు IS_VALID_PARA(x) ((x) && ((x)->pParaBiDi==(x)))
#నిర్వచించు IS_VALID_PARA_OR_పంక్తి(x) ((x) && ((x)->pParaBiDi==(x) || (((x)->pParaBiDi) && (x)->pParaBiDi->pParaBiDi==(x)->pParaBiDi)))

రకనిర్వచనం సంఘం {
    DirProp *dir_Props_జ్ఞాపకం;
    UBiDiLevel *స్థాయిలు_జ్ఞాపకం;
    Opening *openings_జ్ఞాపకం;
    Para *paras_జ్ఞాపకం;
    నిరంతరం *runs_జ్ఞాపకం;
    Isolate *isolates_జ్ఞాపకం;
} Bidi_జ్ఞాపకం_For_కేటాయింపు;

/* Macros for initial checks at function entry */
#నిర్వచించు RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrcode, retvalue) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ((pErrcode)==ఖాళీ || U_FAILURE(*pErrcode)) తిరిగి retvalue; \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు RETURN_IF_NOT_VALID_PARA(bidi, errcode, retvalue) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        తిరిగి retvalue; \
    } \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు RETURN_IF_NOT_VALID_PARA_OR_పంక్తి(bidi, errcode, retvalue) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ(!IS_VALID_PARA_OR_పంక్తి(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        తిరిగి retvalue; \
    } \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు RETURN_IF_BAD_RANGE(arg, ప్రారంభం, హద్దు, errcode, retvalue) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ((arg)<(ప్రారంభం) || (arg)>=(హద్దు)) { \
        (errcode)=U_ILLEGAL_వాదం_ERROR; \
        తిరిగి retvalue; \
    } \
} UPRV_ఖండం_MACRO_చివర

#నిర్వచించు RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrcode) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ((pErrcode)==ఖాళీ || U_FAILURE(*pErrcode)) తిరిగి; \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు RETURN_VOID_IF_NOT_VALID_PARA(bidi, errcode) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        తిరిగి; \
    } \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు RETURN_VOID_IF_NOT_VALID_PARA_OR_పంక్తి(bidi, errcode) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ(!IS_VALID_PARA_OR_పంక్తి(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        తిరిగి; \
    } \
} UPRV_ఖండం_MACRO_చివర
#నిర్వచించు RETURN_VOID_IF_BAD_RANGE(arg, ప్రారంభం, హద్దు, errcode) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ((arg)<(ప్రారంభం) || (arg)>=(హద్దు)) { \
        (errcode)=U_ILLEGAL_వాదం_ERROR; \
        తిరిగి; \
    } \
} UPRV_ఖండం_MACRO_చివర

/* helper function to (re)allocate memory if allowed */
U_CFUNC UBool
ubidi_get_జ్ఞాపకం(Bidi_జ్ఞాపకం_For_కేటాయింపు *p_జ్ఞాపకం, int32_t *p_కొలత, UBool mayAllocate, size_t కొలత_Needed);

/* helper macros for each allocated array in UBiDi */
#నిర్వచించు get_Dir_Props_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->dir_Props_జ్ఞాపకం, &(pBiDi)->dir_Props_కొలత, \
                        (pBiDi)->may_Allocate_పాఠ్యం, (నిడివి))

#నిర్వచించు get_స్థాయిలు_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->స్థాయిలు_జ్ఞాపకం, &(pBiDi)->స్థాయిలు_కొలత, \
                        (pBiDi)->may_Allocate_పాఠ్యం, (నిడివి))

#నిర్వచించు get_Runs_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->runs_జ్ఞాపకం, &(pBiDi)->runs_కొలత, \
                        (pBiDi)->mayAllocateRuns, (నిడివి)*పరిమాణం(నిరంతరం))

/* additional macros used by ubidi_open() - always allow allocation */
#నిర్వచించు get_Initial_Dir_Props_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->dir_Props_జ్ఞాపకం, &(pBiDi)->dir_Props_కొలత, \
                        true, (నిడివి))

#నిర్వచించు get_Initial_స్థాయిలు_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->స్థాయిలు_జ్ఞాపకం, &(pBiDi)->స్థాయిలు_కొలత, \
                        true, (నిడివి))

#నిర్వచించు get_Initial_Openings_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->openings_జ్ఞాపకం, &(pBiDi)->openings_కొలత, \
                        true, (నిడివి)*పరిమాణం(Opening))

#నిర్వచించు get_Initial_Paras_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->paras_జ్ఞాపకం, &(pBiDi)->paras_కొలత, \
                        true, (నిడివి)*పరిమాణం(Para))

#నిర్వచించు get_Initial_Runs_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->runs_జ్ఞాపకం, &(pBiDi)->runs_కొలత, \
                        true, (నిడివి)*పరిమాణం(నిరంతరం))

#నిర్వచించు get_Initial_Isolates_జ్ఞాపకం(pBiDi, నిడివి) \
        ubidi_get_జ్ఞాపకం((Bidi_జ్ఞాపకం_For_కేటాయింపు *)&(pBiDi)->isolates_జ్ఞాపకం, &(pBiDi)->isolates_కొలత, \
                        true, (నిడివి)*పరిమాణం(Isolate))

#అయితే_ముగింపు
