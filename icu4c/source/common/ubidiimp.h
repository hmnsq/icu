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

#अगर_अघोषित UBIDIIMP_H
#घोषणा UBIDIIMP_H

#समावेश "unicode/utypes.h"
#समावेश "unicode/ubidi.h"
#समावेश "unicode/uchar.h"
#समावेश "ubidi_props.h"

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
#घोषणा UBIDI_INTERNAL_RUNS_ONLY 64

// ICU-20853=ICU-20935 Solaris #defines CS and ES in sys/regset.h
#अगर_घोषित CS
#   undef CS
#पूर्ण_अगर
#अगर_घोषित ES
#   undef ES
#पूर्ण_अगर

प्रकार uint8_t DirProp;
प्रकार uint32_t Flags;

/*  Comparing the description of the BiDi algorithm with this implementation
    is easier with the same names for the BiDi types in the code as there.
    See UCharDirection in uchar.h .
*/
क्रमागत {
    L=  U_बायाँ_TO_दायाँ,                /*  0 */
    R=  U_दायाँ_TO_बायाँ,                /*  1 */
    EN= U_EUROPEAN_संख्या,              /*  2 */
    ES= U_EUROPEAN_संख्या_SEPARATOR,    /*  3 */
    ET= U_EUROPEAN_संख्या_TERMINATOR,   /*  4 */
    AN= U_ARABIC_संख्या,                /*  5 */
    CS= U_COMMON_संख्या_SEPARATOR,      /*  6 */
    B=  U_BLOCK_SEPARATOR,              /*  7 */
    S=  U_SEGMENT_SEPARATOR,            /*  8 */
    WS= U_WHITE_SPACE_NEUTRAL,          /*  9 */
    ON= U_OTHER_NEUTRAL,                /* 10 */
    LRE=U_बायाँ_TO_दायाँ_EMBEDDING,      /* 11 */
    LRO=U_बायाँ_TO_दायाँ_OVERRIDE,       /* 12 */
    AL= U_दायाँ_TO_बायाँ_ARABIC,         /* 13 */
    RLE=U_दायाँ_TO_बायाँ_EMBEDDING,      /* 14 */
    RLO=U_दायाँ_TO_बायाँ_OVERRIDE,       /* 15 */
    PDF=U_निकालो_DIRECTIONAL_FORMAT,       /* 16 */
    NSM=U_DIR_NON_SPACING_MARK,         /* 17 */
    BN= U_BOUNDARY_NEUTRAL,             /* 18 */
    FSI=U_FIRST_STRONG_ISOLATE,         /* 19 */
    LRI=U_बायाँ_TO_दायाँ_ISOLATE,        /* 20 */
    RLI=U_दायाँ_TO_बायाँ_ISOLATE,        /* 21 */
    PDI=U_निकालो_DIRECTIONAL_ISOLATE,      /* 22 */
    ENL,    /* EN after W7 */           /* 23 */
    ENR,    /* EN not subject to W7 */  /* 24 */
    dir_Prop_गिनती
};

/*  Sometimes, bit values are more appropriate
    to deal with directionality properties.
    Abbreviations in these macro names refer to names
    used in the BiDi algorithm.
*/
#घोषणा DIRPROP_FLAG(dir) (1UL<<(dir))
#घोषणा PURE_DIRPROP(prop)  ((prop)&~0xE0)    ?????????????????????????

/* special flag for multiple runs from explicit embedding codes */
#घोषणा DIRPROP_FLAG_MULTI_RUNS (1UL<<31)

/* are there any characters that are LTR or RTL? */
#घोषणा MASK_LTR (DIRPROP_FLAG(L)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(ENL)|DIRPROP_FLAG(ENR)|DIRPROP_FLAG(AN)|DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(LRI))
#घोषणा MASK_RTL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(RLI))
#घोषणा MASK_R_AL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL))
#घोषणा MASK_STRONG_EN_AN (DIRPROP_FLAG(L)|DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(AN))

/* explicit embedding codes */
#घोषणा MASK_EXPLICIT (DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(PDF))

/* explicit isolate codes */
#घोषणा MASK_ISO (DIRPROP_FLAG(LRI)|DIRPROP_FLAG(RLI)|DIRPROP_FLAG(FSI)|DIRPROP_FLAG(PDI))

#घोषणा MASK_BN_EXPLICIT (DIRPROP_FLAG(BN)|MASK_EXPLICIT)

/* paragraph and segment separators */
#घोषणा MASK_B_S (DIRPROP_FLAG(B)|DIRPROP_FLAG(S))

/* all types that are counted as White Space or Neutral in some steps */
#घोषणा MASK_WS (MASK_B_S|DIRPROP_FLAG(WS)|MASK_BN_EXPLICIT|MASK_ISO)

/* types that are neutrals or could becomes neutrals in (Wn) */
#घोषणा MASK_POSSIBLE_N (DIRPROP_FLAG(ON)|DIRPROP_FLAG(CS)|DIRPROP_FLAG(ES)|DIRPROP_FLAG(ET)|MASK_WS)

/*
 *  These types may be changed to "e",
 *  the embedding type (L or R) of the run,
 *  in the BiDi algorithm (N2)
 */
#घोषणा MASK_EMBEDDING (DIRPROP_FLAG(NSM)|MASK_POSSIBLE_N)

/* the dirProp's L and R are defined to 0 and 1 values in UCharDirection */
#घोषणा GET_LR_FROM_LEVEL(level) ((DirProp)((level)&1))

#घोषणा IS_DEFAULT_LEVEL(level) ((level)>=0xfe)

/*
 *  The following bit is used for the directional isolate status.
 *  Stack entries corresponding to isolate sequences are greater than ISOLATE.
 */
#घोषणा ISOLATE  0x0100

U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_सूचकांक(स्थिर UBiDi *pBiDi, int32_t सूचकांक);

#घोषणा GET_PARALEVEL(ubidi, सूचकांक) \
            ((UBiDiLevel)(!(ubidi)->defaultParaLevel || (सूचकांक)<(ubidi)->paras[0].limit ? \
                         (ubidi)->paraLevel : ubidi_get_Para_Level_At_सूचकांक((ubidi), (सूचकांक))))

/* number of paras entries allocated initially without malloc */
#घोषणा SIMPLE_PARAS_गिनती      10
/* number of isolate entries allocated initially without malloc */
#घोषणा SIMPLE_ISOLATES_गिनती   5
/* number of isolate run entries for paired brackets allocated initially without malloc */
#घोषणा SIMPLE_OPENINGS_गिनती   20

#घोषणा CR  0x000D
#घोषणा LF  0x000A

/* Run structure for reordering --------------------------------------------- */
क्रमागत {
    LRM_BEFORE=1,
    LRM_AFTER=2,
    RLM_BEFORE=4,
    RLM_AFTER=8
};

प्रकार काष्ठा Para {
    int32_t limit;
    int32_t level;
} Para;

क्रमागत {                                  /* flags for Opening.flags */
    FOUND_L=DIRPROP_FLAG(L),
    FOUND_R=DIRPROP_FLAG(R)
};

प्रकार काष्ठा Opening {
    int32_t position;                   /* position of opening bracket */
    int32_t match;                      /* matching char or -position of closing bracket */
    int32_t contextPos;                 /* position of last strong char found before opening */
    uint16_t flags;                     /* bits for L or R/AL found within the pair */
    UBiDiDirection contextDir;          /* L or R according to last strong char before opening */
    uint8_t filler;                     /* to complete a nice multiple of 4 chars */
} Opening;

प्रकार काष्ठा IsoRun {
    int32_t  contextPos;                /* position of char determining context */
    uint16_t start;                     /* index of first opening entry for this run */
    uint16_t limit;                     /* index after last opening entry for this run */
    UBiDiLevel level;                   /* level of this run */
    DirProp lastStrong;                 /* bidi class of last strong char found in this run */
    DirProp lastBase;                   /* bidi class of last base char found in this run */
    UBiDiDirection contextDir;          /* L or R to use as context for following openings */
} IsoRun;

प्रकार काष्ठा Bracket_तथ्य {
    UBiDi   *pBiDi;
    /* array of opening entries which should be enough in most cases; no malloc() */
    Opening simpleOpenings[SIMPLE_OPENINGS_गिनती];
    Opening *openings;                  /* pointer to current array of entries */
    int32_t openings_गिनती;              /* number of allocated entries */
    int32_t isoRunLast;                 /* index of last used entry */
    /* array of nested isolated sequence entries; can never excess UBIDI_MAX_EXPLICIT_LEVEL
       + 1 for index 0, + 1 for before the first isolated sequence */
    IsoRun  isoRuns[UBIDI_MAX_EXPLICIT_LEVEL+2];
    UBool isNumbersSpecial;             /* reordering mode for NUMBERS_SPECIAL */
} Bracket_तथ्य;

प्रकार काष्ठा Isolate {
    int32_t startON;
    int32_t start1;
    int32_t state;
    int16_t stateImp;
} Isolate;

प्रकार काष्ठा Run {
    int32_t logicalStart,   /* first character of the run; b31 indicates even/odd level */
            visualLimit,    /* last visual position of the run +1 */
            जोड़ो_Remove;   /* if >0, flags for inserting LRM/RLM before/after run,
                               if <0, count of bidi controls within run            */
} Run;

/* in a Run, logicalStart will get this bit set if the run level is odd */
#घोषणा सूचकांक_ODD_BIT (1UL<<31)

#घोषणा MAKE_सूचकांक_ODD_PAIR(सूचकांक, level) ((सूचकांक)|((int32_t)((level)&1)<<31))
#घोषणा ADD_ODD_BIT_FROM_LEVEL(x, level)  ((x)|=((int32_t)((level)&1)<<31))
#घोषणा REMOVE_ODD_BIT(x)                 ((x)&=~सूचकांक_ODD_BIT)

#घोषणा GET_सूचकांक(x)   ((x)&~सूचकांक_ODD_BIT)
#घोषणा GET_ODD_BIT(x) ((uint32_t)(x)>>31)
#घोषणा IS_ODD_RUN(x)  ((UBool)(((x)&सूचकांक_ODD_BIT)!=0))
#घोषणा IS_EVEN_RUN(x) ((UBool)(((x)&सूचकांक_ODD_BIT)==0))

U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UErrorCode *pErrorCode);

/** BiDi control code points */
क्रमागत {
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

#घोषणा IS_BIDI_CONTROL_CHAR(c) (((uint32_t)(c)&0xfffffffc)==ZWNJ_CHAR || (uint32_t)((c)-LRE_CHAR)<5 || (uint32_t)((c)-LRI_CHAR)<4)

/* InsertPoints structure for noting where to put BiDi marks ---------------- */

प्रकार काष्ठा Point {
    int32_t pos;            /* position in text */
    int32_t flag;           /* flag for LRM/RLM, before/after */
} Point;

प्रकार काष्ठा जोड़ो_Points_2 {
    int32_t capacity;       /* number of points allocated */
    int32_t आकार_2;           /* number of points used */
    int32_t confirmed;      /* number of points confirmed */
    UErrorCode errorCode;   /* for eventual memory shortage */
    Point *points;          /* pointer to array of points */
} जोड़ो_Points_2;


/* UBiDi structure ----------------------------------------------------------- */

काष्ठा UBiDi {
    /* pointer to parent paragraph object (pointer to self if this object is
     * a paragraph object); set to NULL in a newly opened object; set to a
     * real value after a successful execution of ubidi_setPara or ubidi_setLine
     */
    स्थिर UBiDi * pParaBiDi;

    /* alias pointer to the current text */
    स्थिर UChar *text;

    /* length of the current text */
    int32_t original_लंबाई;

    /* if the UBIDI_OPTION_STREAMING option is set, this is the length
     * of text actually processed by ubidi_setPara, which may be shorter than
     * the original length.
     * Otherwise, it is identical to the original length.
     */
    int32_t लंबाई;

    /* if the UBIDI_OPTION_REMOVE_CONTROLS option is set, and/or
     * marks are allowed to be inserted in one of the reordering mode, the
     * length of the result string may be different from the processed length.
     */
    int32_t परिणाम_लंबाई;

    /* memory sizes in bytes */
    int32_t dir_Props_आकार, levels_आकार, openings_आकार, paras_आकार, runs_आकार, isolates_आकार;

    /* allocated memory */
    DirProp *dirPropsMemory;
    UBiDiLevel *levelsMemory;
    Opening *openingsMemory;
    Para *parasMemory;
    Run *runsMemory;
    Isolate *isolatesMemory;

    /* indicators for whether memory may be allocated after ubidi_open() */
    UBool mayAllocateText, mayAllocateRuns;

    /* arrays with one value per text-character */
    DirProp *dirProps;
    UBiDiLevel *levels;

    /* are we performing an approximation of the "inverse BiDi" algorithm? */
    UBool isInverse;

    /* are we using the basic algorithm or its variation? */
    UBiDiReorderingMode reorderingMode;

    /* UBIDI_REORDER_xxx values must be ordered so that all the regular
     * logical to visual modes come first, and all inverse BiDi modes
     * come last.
     */
    #घोषणा UBIDI_REORDER_LAST_LOGICAL_TO_VISUAL    UBIDI_REORDER_NUMBERS_SPECIAL

    /* bitmask for reordering options */
    uint32_t reorderingOptions;

    /* must block separators receive level 0? */
    UBool orderParagraphsLTR;

    /* the paragraph level */
    UBiDiLevel paraLevel;
    /* original paraLevel when contextual */
    /* must be one of UBIDI_DEFAULT_xxx or 0 if not contextual */
    UBiDiLevel defaultParaLevel;

    /* context data */
    स्थिर UChar *prologue;
    int32_t pro_लंबाई;
    स्थिर UChar *epilogue;
    int32_t epi_लंबाई;

    /* the following is set in ubidi_setPara, used in processPropertySeq */
    स्थिर काष्ठा ImpTabPair * pImpTabPair;  /* pointer to levels state table pair */

    /* the overall paragraph or line directionality - see UBiDiDirection */
    UBiDiDirection direction;

    /* flags is a bit set for which directional properties are in the text */
    Flags flags;

    /* lastArabicPos is index to the last AL in the text, -1 if none */
    int32_t lastArabicPos;

    /* characters after trailingWSStart are WS and are */
    /* implicitly at the paraLevel (rule (L1)) - levels may not reflect that */
    int32_t trailingWSStart;

    /* fields for paragraph handling */
    int32_t para_गिनती;                  /* set in getDirProps() */
    /* filled in getDirProps() */
    Para *paras;

    /* for relatively short text, we only need a tiny array of paras (no malloc()) */
    Para simpleParas[SIMPLE_PARAS_गिनती];

    /* fields for line reordering */
    int32_t run_गिनती;     /* ==-1: runs not set up yet */
    Run *runs;

    /* for non-mixed text, we only need a tiny array of runs (no malloc()) */
    Run simpleRuns[1];

    /* maximum or current nesting depth of isolate sequences */
    /* Within resolveExplicitLevels() and checkExplicitLevels(), this is the maximal
       nesting encountered.
       Within resolveImplicitLevels(), this is the index of the current isolates
       stack entry. */
    int32_t isolate_गिनती;
    Isolate *isolates;

    /* for simple text, have a small stack (no malloc()) */
    Isolate simpleIsolates[SIMPLE_ISOLATES_गिनती];

    /* for inverse Bidi with insertion of directional marks */
    जोड़ो_Points_2 जोड़ो_Points;

    /* for option UBIDI_OPTION_REMOVE_CONTROLS */
    int32_t control_गिनती;

    /* for Bidi class callback */
    UBiDiClassCallback *fnClassCallback;    /* action pointer */
    स्थिर व्योम *coClassCallback;            /* context pointer */
};

#घोषणा IS_VALID_PARA(x) ((x) && ((x)->pParaBiDi==(x)))
#घोषणा IS_VALID_PARA_OR_पंक्ति(x) ((x) && ((x)->pParaBiDi==(x) || (((x)->pParaBiDi) && (x)->pParaBiDi->pParaBiDi==(x)->pParaBiDi)))

प्रकार जोड़ {
    DirProp *dirPropsMemory;
    UBiDiLevel *levelsMemory;
    Opening *openingsMemory;
    Para *parasMemory;
    Run *runsMemory;
    Isolate *isolatesMemory;
} BidiMemoryForAllocation;

/* Macros for initial checks at function entry */
#घोषणा RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrcode, retvalue) UPRV_BLOCK_MACRO_BEGIN { \
    अगर((pErrcode)==शून्य || U_FAILURE(*pErrcode)) वापस retvalue; \
} UPRV_BLOCK_MACRO_END
#घोषणा RETURN_IF_NOT_VALID_PARA(bidi, errcode, retvalue) UPRV_BLOCK_MACRO_BEGIN { \
    अगर(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        वापस retvalue; \
    } \
} UPRV_BLOCK_MACRO_END
#घोषणा RETURN_IF_NOT_VALID_PARA_OR_पंक्ति(bidi, errcode, retvalue) UPRV_BLOCK_MACRO_BEGIN { \
    अगर(!IS_VALID_PARA_OR_पंक्ति(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        वापस retvalue; \
    } \
} UPRV_BLOCK_MACRO_END
#घोषणा RETURN_IF_BAD_RANGE(arg, start, limit, errcode, retvalue) UPRV_BLOCK_MACRO_BEGIN { \
    अगर((arg)<(start) || (arg)>=(limit)) { \
        (errcode)=U_ILLEGAL_ARGUMENT_ERROR; \
        वापस retvalue; \
    } \
} UPRV_BLOCK_MACRO_END

#घोषणा RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrcode) UPRV_BLOCK_MACRO_BEGIN { \
    अगर((pErrcode)==शून्य || U_FAILURE(*pErrcode)) वापस; \
} UPRV_BLOCK_MACRO_END
#घोषणा RETURN_VOID_IF_NOT_VALID_PARA(bidi, errcode) UPRV_BLOCK_MACRO_BEGIN { \
    अगर(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        वापस; \
    } \
} UPRV_BLOCK_MACRO_END
#घोषणा RETURN_VOID_IF_NOT_VALID_PARA_OR_पंक्ति(bidi, errcode) UPRV_BLOCK_MACRO_BEGIN { \
    अगर(!IS_VALID_PARA_OR_पंक्ति(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        वापस; \
    } \
} UPRV_BLOCK_MACRO_END
#घोषणा RETURN_VOID_IF_BAD_RANGE(arg, start, limit, errcode) UPRV_BLOCK_MACRO_BEGIN { \
    अगर((arg)<(start) || (arg)>=(limit)) { \
        (errcode)=U_ILLEGAL_ARGUMENT_ERROR; \
        वापस; \
    } \
} UPRV_BLOCK_MACRO_END

/* helper function to (re)allocate memory if allowed */
U_CFUNC UBool
ubidi_getMemory(BidiMemoryForAllocation *pMemory, int32_t *p_आकार, UBool mayAllocate, माप_प्रकार आकार_Needed);

/* helper macros for each allocated array in UBiDi */
#घोषणा getDirPropsMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->dirPropsMemory, &(pBiDi)->dir_Props_आकार, \
                        (pBiDi)->mayAllocateText, (लंबाई))

#घोषणा getLevelsMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->levelsMemory, &(pBiDi)->levels_आकार, \
                        (pBiDi)->mayAllocateText, (लंबाई))

#घोषणा getRunsMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->runsMemory, &(pBiDi)->runs_आकार, \
                        (pBiDi)->mayAllocateRuns, (लंबाई)*माप(Run))

/* additional macros used by ubidi_open() - always allow allocation */
#घोषणा getInitialDirPropsMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->dirPropsMemory, &(pBiDi)->dir_Props_आकार, \
                        true, (लंबाई))

#घोषणा getInitialLevelsMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->levelsMemory, &(pBiDi)->levels_आकार, \
                        true, (लंबाई))

#घोषणा getInitialOpeningsMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->openingsMemory, &(pBiDi)->openings_आकार, \
                        true, (लंबाई)*माप(Opening))

#घोषणा getInitialParasMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->parasMemory, &(pBiDi)->paras_आकार, \
                        true, (लंबाई)*माप(Para))

#घोषणा getInitialRunsMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->runsMemory, &(pBiDi)->runs_आकार, \
                        true, (लंबाई)*माप(Run))

#घोषणा getInitialIsolatesMemory(pBiDi, लंबाई) \
        ubidi_getMemory((BidiMemoryForAllocation *)&(pBiDi)->isolatesMemory, &(pBiDi)->isolates_आकार, \
                        true, (लंबाई)*माप(Isolate))

#पूर्ण_अगर
