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

#यदि_अपरिभाषितम् UBIDIIMP_H
#परिभाषय UBIDIIMP_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ubidi.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "ubidi_props.h"

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
#परिभाषय UBIDI_आन्तरिकम्_RUNS_ONLY 64

// ICU-20853=ICU-20935 Solaris #defines CS and ES in sys/regset.h
#यदि_परिभाषितम् CS
#   undef CS
#यदि_अन्तः
#यदि_परिभाषितम् ES
#   undef ES
#यदि_अन्तः

प्रकारनिर्देश uint8_t DirProp;
प्रकारनिर्देश uint32_t Flags;

/*  Comparing the description of the BiDi algorithm with this implementation
    is easier with the same names for the BiDi types in the code as there.
    See UCharDirection in uchar.h .
*/
गणना {
    L=  U_वामः_गन्तव्यम्_दक्षिणः,                /*  0 */
    R=  U_दक्षिणः_गन्तव्यम्_वामः,                /*  1 */
    EN= U_EUROPEAN_सङ्ख्या,              /*  2 */
    ES= U_EUROPEAN_सङ्ख्या_SEPARATOR,    /*  3 */
    ET= U_EUROPEAN_सङ्ख्या_TERMINATOR,   /*  4 */
    AN= U_ARABIC_सङ्ख्या,                /*  5 */
    CS= U_COMMON_सङ्ख्या_SEPARATOR,      /*  6 */
    B=  U_खण्डम्_SEPARATOR,              /*  7 */
    S=  U_SEGMENT_SEPARATOR,            /*  8 */
    WS= U_WHITE_SPACE_NEUTRAL,          /*  9 */
    ON= U_OTHER_NEUTRAL,                /* 10 */
    LRE=U_वामः_गन्तव्यम्_दक्षिणः_EMBEDDING,      /* 11 */
    LRO=U_वामः_गन्तव्यम्_दक्षिणः_OVERRIDE,       /* 12 */
    AL= U_दक्षिणः_गन्तव्यम्_वामः_ARABIC,         /* 13 */
    RLE=U_दक्षिणः_गन्तव्यम्_वामः_EMBEDDING,      /* 14 */
    RLO=U_दक्षिणः_गन्तव्यम्_वामः_OVERRIDE,       /* 15 */
    PDF=U_निष्कासय_DIRECTIONAL_FORMAT,       /* 16 */
    NSM=U_DIR_NON_SPACING_MARK,         /* 17 */
    BN= U_BOUNDARY_NEUTRAL,             /* 18 */
    FSI=U_FIRST_STRONG_ISOLATE,         /* 19 */
    LRI=U_वामः_गन्तव्यम्_दक्षिणः_ISOLATE,        /* 20 */
    RLI=U_दक्षिणः_गन्तव्यम्_वामः_ISOLATE,        /* 21 */
    PDI=U_निष्कासय_DIRECTIONAL_ISOLATE,      /* 22 */
    ENL,    /* EN after W7 */           /* 23 */
    ENR,    /* EN not subject to W7 */  /* 24 */
    dir_Prop_गणनम्
};

/*  Sometimes, bit values are more appropriate
    to deal with directionality properties.
    Abbreviations in these macro names refer to names
    used in the BiDi algorithm.
*/
#परिभाषय DIRPROP_FLAG(dir) (1UL<<(dir))
#परिभाषय विशुद्धम्_DIRPROP(prop)  ((prop)&~0xE0)    ?????????????????????????

/* special flag for multiple runs from explicit embedding codes */
#परिभाषय DIRPROP_FLAG_MULTI_RUNS (1UL<<31)

/* are there any characters that are LTR or RTL? */
#परिभाषय आवरणम्_LTR (DIRPROP_FLAG(L)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(ENL)|DIRPROP_FLAG(ENR)|DIRPROP_FLAG(AN)|DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(LRI))
#परिभाषय आवरणम्_RTL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(RLI))
#परिभाषय आवरणम्_R_AL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL))
#परिभाषय आवरणम्_STRONG_EN_AN (DIRPROP_FLAG(L)|DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(AN))

/* explicit embedding codes */
#परिभाषय आवरणम्_EXPLICIT (DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(PDF))

/* explicit isolate codes */
#परिभाषय आवरणम्_ISO (DIRPROP_FLAG(LRI)|DIRPROP_FLAG(RLI)|DIRPROP_FLAG(FSI)|DIRPROP_FLAG(PDI))

#परिभाषय आवरणम्_BN_EXPLICIT (DIRPROP_FLAG(BN)|आवरणम्_EXPLICIT)

/* paragraph and segment separators */
#परिभाषय आवरणम्_B_S (DIRPROP_FLAG(B)|DIRPROP_FLAG(S))

/* all types that are counted as White Space or Neutral in some steps */
#परिभाषय आवरणम्_WS (आवरणम्_B_S|DIRPROP_FLAG(WS)|आवरणम्_BN_EXPLICIT|आवरणम्_ISO)

/* types that are neutrals or could becomes neutrals in (Wn) */
#परिभाषय आवरणम्_POSSIBLE_N (DIRPROP_FLAG(ON)|DIRPROP_FLAG(CS)|DIRPROP_FLAG(ES)|DIRPROP_FLAG(ET)|आवरणम्_WS)

/*
 *  These types may be changed to "e",
 *  the embedding type (L or R) of the run,
 *  in the BiDi algorithm (N2)
 */
#परिभाषय आवरणम्_EMBEDDING (DIRPROP_FLAG(NSM)|आवरणम्_POSSIBLE_N)

/* the dirProp's L and R are defined to 0 and 1 values in UCharDirection */
#परिभाषय GET_LR_FROM_LEVEL(level) ((DirProp)((level)&1))

#परिभाषय IS_DEFAULT_LEVEL(level) ((level)>=0xfe)

/*
 *  The following bit is used for the directional isolate status.
 *  Stack entries corresponding to isolate sequences are greater than ISOLATE.
 */
#परिभाषय ISOLATE  0x0100

U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_सूचकांक(ध्रुव UBiDi *pBiDi, int32_t सूचकांक);

#परिभाषय GET_PARALEVEL(ubidi, सूचकांक) \
            ((UBiDiLevel)(!(ubidi)->defaultParaLevel || (सूचकांक)<(ubidi)->paras[0].सीमा ? \
                         (ubidi)->paraLevel : ubidi_get_Para_Level_At_सूचकांक((ubidi), (सूचकांक))))

/* number of paras entries allocated initially without malloc */
#परिभाषय SIMPLE_PARAS_गणनम्      10
/* number of isolate entries allocated initially without malloc */
#परिभाषय SIMPLE_ISOLATES_गणनम्   5
/* number of isolate run entries for paired brackets allocated initially without malloc */
#परिभाषय SIMPLE_OPENINGS_गणनम्   20

#परिभाषय CR  0x000D
#परिभाषय LF  0x000A

/* Run structure for reordering --------------------------------------------- */
गणना {
    LRM_BEFORE=1,
    LRM_AFTER=2,
    RLM_BEFORE=4,
    RLM_AFTER=8
};

प्रकारनिर्देश संरचना Para {
    int32_t सीमा;
    int32_t level;
} Para;

गणना {                                  /* flags for Opening.flags */
    प्राप्तम्_L=DIRPROP_FLAG(L),
    प्राप्तम्_R=DIRPROP_FLAG(R)
};

प्रकारनिर्देश संरचना Opening {
    int32_t position;                   /* position of opening bracket */
    int32_t मेलनम्;                      /* matching char or -position of closing bracket */
    int32_t contextPos;                 /* position of last strong char found before opening */
    uint16_t flags;                     /* bits for L or R/AL found within the pair */
    UBiDiDirection contextDir;          /* L or R according to last strong char before opening */
    uint8_t filler;                     /* to complete a nice multiple of 4 chars */
} Opening;

प्रकारनिर्देश संरचना Iso_सततम् {
    int32_t  contextPos;                /* position of char determining context */
    uint16_t start;                     /* index of first opening entry for this run */
    uint16_t सीमा;                     /* index after last opening entry for this run */
    UBiDiLevel level;                   /* level of this run */
    DirProp चरमः_Strong;                 /* bidi class of last strong char found in this run */
    DirProp चरमः_आधारः;                   /* bidi class of last base char found in this run */
    UBiDiDirection contextDir;          /* L or R to use as context for following openings */
} Iso_सततम्;

प्रकारनिर्देश संरचना Bracket_दत्त {
    UBiDi   *pBiDi;
    /* array of opening entries which should be enough in most cases; no malloc() */
    Opening simpleOpenings[SIMPLE_OPENINGS_गणनम्];
    Opening *openings;                  /* pointer to current array of entries */
    int32_t openings_गणनम्;              /* number of allocated entries */
    int32_t iso_सततम्_चरमः;                 /* index of last used entry */
    /* array of nested isolated sequence entries; can never excess UBIDI_MAX_EXPLICIT_LEVEL
       + 1 for index 0, + 1 for before the first isolated sequence */
    Iso_सततम्  isoRuns[UBIDI_MAX_EXPLICIT_LEVEL+2];
    UBool isNumbersSpecial;             /* reordering mode for NUMBERS_SPECIAL */
} Bracket_दत्त;

प्रकारनिर्देश संरचना Isolate {
    int32_t startON;
    int32_t start1;
    int32_t state;
    int16_t stateImp;
} Isolate;

प्रकारनिर्देश संरचना सततम् {
    int32_t logicalStart,   /* first character of the run; b31 indicates even/odd level */
            visual_सीमा,    /* last visual position of the run +1 */
            निवेशय_Remove;   /* if >0, flags for inserting LRM/RLM before/after run,
                               if <0, count of bidi controls within run            */
} सततम्;

/* in a Run, logicalStart will get this bit set if the run level is odd */
#परिभाषय सूचकांक_ODD_BIT (1UL<<31)

#परिभाषय MAKE_सूचकांक_ODD_PAIR(सूचकांक, level) ((सूचकांक)|((int32_t)((level)&1)<<31))
#परिभाषय सङ्कलय_ODD_BIT_FROM_LEVEL(x, level)  ((x)|=((int32_t)((level)&1)<<31))
#परिभाषय REMOVE_ODD_BIT(x)                 ((x)&=~सूचकांक_ODD_BIT)

#परिभाषय GET_सूचकांक(x)   ((x)&~सूचकांक_ODD_BIT)
#परिभाषय GET_ODD_BIT(x) ((uint32_t)(x)>>31)
#परिभाषय IS_ODD_सततम्(x)  ((UBool)(((x)&सूचकांक_ODD_BIT)!=0))
#परिभाषय IS_EVEN_सततम्(x) ((UBool)(((x)&सूचकांक_ODD_BIT)==0))

U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UError_संहिता *p_Error_संहिता);

/** BiDi control code points */
गणना {
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

#परिभाषय IS_BIDI_CONTROL_CHAR(c) (((uint32_t)(c)&0xfffffffc)==ZWNJ_CHAR || (uint32_t)((c)-LRE_CHAR)<5 || (uint32_t)((c)-LRI_CHAR)<4)

/* InsertPoints structure for noting where to put BiDi marks ---------------- */

प्रकारनिर्देश संरचना Point {
    int32_t pos;            /* position in text */
    int32_t flag;           /* flag for LRM/RLM, before/after */
} Point;

प्रकारनिर्देश संरचना निवेशय_Points_2 {
    int32_t क्षमता;       /* number of points allocated */
    int32_t आकार_2;           /* number of points used */
    int32_t confirmed;      /* number of points confirmed */
    UError_संहिता error_संहिता;   /* for eventual memory shortage */
    Point *points;          /* pointer to array of points */
} निवेशय_Points_2;


/* UBiDi structure ----------------------------------------------------------- */

संरचना UBiDi {
    /* pointer to parent paragraph object (pointer to self if this object is
     * a paragraph object); set to NULL in a newly opened object; set to a
     * real value after a successful execution of ubidi_setPara or ubidi_setLine
     */
    ध्रुव UBiDi * pParaBiDi;

    /* alias pointer to the current text */
    ध्रुव UChar *text;

    /* length of the current text */
    int32_t original_दीर्घता;

    /* if the UBIDI_OPTION_STREAMING option is set, this is the length
     * of text actually processed by ubidi_setPara, which may be shorter than
     * the original length.
     * Otherwise, it is identical to the original length.
     */
    int32_t दीर्घता;

    /* if the UBIDI_OPTION_REMOVE_CONTROLS option is set, and/or
     * marks are allowed to be inserted in one of the reordering mode, the
     * length of the result string may be different from the processed length.
     */
    int32_t परिणाम_दीर्घता;

    /* memory sizes in bytes */
    int32_t dir_Props_आकार, स्तराः_आकार, openings_आकार, paras_आकार, runs_आकार, isolates_आकार;

    /* allocated memory */
    DirProp *dir_Props_स्मृतिः;
    UBiDiLevel *स्तराः_स्मृतिः;
    Opening *openings_स्मृतिः;
    Para *paras_स्मृतिः;
    सततम् *runs_स्मृतिः;
    Isolate *isolates_स्मृतिः;

    /* indicators for whether memory may be allocated after ubidi_open() */
    UBool mayAllocateText, mayAllocateRuns;

    /* arrays with one value per text-character */
    DirProp *dirProps;
    UBiDiLevel *स्तराः;

    /* are we performing an approximation of the "inverse BiDi" algorithm? */
    UBool isInverse;

    /* are we using the basic algorithm or its variation? */
    UBi_Di_Reordering_बहुलकम् reordering_बहुलकम्;

    /* UBIDI_REORDER_xxx values must be ordered so that all the regular
     * logical to visual modes come first, and all inverse BiDi modes
     * come last.
     */
    #परिभाषय UBIDI_REORDER_चरमः_LOGICAL_गन्तव्यम्_VISUAL    UBIDI_REORDER_NUMBERS_SPECIAL

    /* bitmask for reordering options */
    uint32_t reorderingOptions;

    /* must block separators receive level 0? */
    UBool क्रमः_Paragraphs_LTR;

    /* the paragraph level */
    UBiDiLevel paraLevel;
    /* original paraLevel when contextual */
    /* must be one of UBIDI_DEFAULT_xxx or 0 if not contextual */
    UBiDiLevel defaultParaLevel;

    /* context data */
    ध्रुव UChar *prologue;
    int32_t pro_दीर्घता;
    ध्रुव UChar *epilogue;
    int32_t epi_दीर्घता;

    /* the following is set in ubidi_setPara, used in processPropertySeq */
    ध्रुव संरचना ImpTabPair * pImpTabPair;  /* pointer to levels state table pair */

    /* the overall paragraph or line directionality - see UBiDiDirection */
    UBiDiDirection direction;

    /* flags is a bit set for which directional properties are in the text */
    Flags flags;

    /* lastArabicPos is index to the last AL in the text, -1 if none */
    int32_t चरमः_Arabic_Pos;

    /* characters after trailingWSStart are WS and are */
    /* implicitly at the paraLevel (rule (L1)) - levels may not reflect that */
    int32_t trailingWSStart;

    /* fields for paragraph handling */
    int32_t para_गणनम्;                  /* set in getDirProps() */
    /* filled in getDirProps() */
    Para *paras;

    /* for relatively short text, we only need a tiny array of paras (no malloc()) */
    Para simpleParas[SIMPLE_PARAS_गणनम्];

    /* fields for line reordering */
    int32_t सततम्_गणनम्;     /* ==-1: runs not set up yet */
    सततम् *runs;

    /* for non-mixed text, we only need a tiny array of runs (no malloc()) */
    सततम् simpleRuns[1];

    /* maximum or current nesting depth of isolate sequences */
    /* Within resolveExplicitLevels() and checkExplicitLevels(), this is the maximal
       nesting encountered.
       Within resolveImplicitLevels(), this is the index of the current isolates
       stack entry. */
    int32_t isolate_गणनम्;
    Isolate *isolates;

    /* for simple text, have a small stack (no malloc()) */
    Isolate simpleIsolates[SIMPLE_ISOLATES_गणनम्];

    /* for inverse Bidi with insertion of directional marks */
    निवेशय_Points_2 निवेशय_Points;

    /* for option UBIDI_OPTION_REMOVE_CONTROLS */
    int32_t control_गणनम्;

    /* for Bidi class callback */
    UBiDiClassCallback *fnClassCallback;    /* action pointer */
    ध्रुव शून्य *coClassCallback;            /* context pointer */
};

#परिभाषय IS_VALID_PARA(x) ((x) && ((x)->pParaBiDi==(x)))
#परिभाषय IS_VALID_PARA_OR_पङ्क्तिः(x) ((x) && ((x)->pParaBiDi==(x) || (((x)->pParaBiDi) && (x)->pParaBiDi->pParaBiDi==(x)->pParaBiDi)))

प्रकारनिर्देश सङ्घ {
    DirProp *dir_Props_स्मृतिः;
    UBiDiLevel *स्तराः_स्मृतिः;
    Opening *openings_स्मृतिः;
    Para *paras_स्मृतिः;
    सततम् *runs_स्मृतिः;
    Isolate *isolates_स्मृतिः;
} Bidi_स्मृतिः_For_आवण्टनम्;

/* Macros for initial checks at function entry */
#परिभाषय RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrcode, retvalue) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि((pErrcode)==रिक्त || U_FAILURE(*pErrcode)) प्रत्यागम retvalue; \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय RETURN_IF_NOT_VALID_PARA(bidi, errcode, retvalue) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        प्रत्यागम retvalue; \
    } \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय RETURN_IF_NOT_VALID_PARA_OR_पङ्क्तिः(bidi, errcode, retvalue) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि(!IS_VALID_PARA_OR_पङ्क्तिः(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        प्रत्यागम retvalue; \
    } \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय RETURN_IF_BAD_RANGE(arg, start, सीमा, errcode, retvalue) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि((arg)<(start) || (arg)>=(सीमा)) { \
        (errcode)=U_ILLEGAL_तर्कः_ERROR; \
        प्रत्यागम retvalue; \
    } \
} UPRV_खण्डम्_MACRO_अन्तः

#परिभाषय RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrcode) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि((pErrcode)==रिक्त || U_FAILURE(*pErrcode)) प्रत्यागम; \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय RETURN_VOID_IF_NOT_VALID_PARA(bidi, errcode) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        प्रत्यागम; \
    } \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय RETURN_VOID_IF_NOT_VALID_PARA_OR_पङ्क्तिः(bidi, errcode) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि(!IS_VALID_PARA_OR_पङ्क्तिः(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        प्रत्यागम; \
    } \
} UPRV_खण्डम्_MACRO_अन्तः
#परिभाषय RETURN_VOID_IF_BAD_RANGE(arg, start, सीमा, errcode) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि((arg)<(start) || (arg)>=(सीमा)) { \
        (errcode)=U_ILLEGAL_तर्कः_ERROR; \
        प्रत्यागम; \
    } \
} UPRV_खण्डम्_MACRO_अन्तः

/* helper function to (re)allocate memory if allowed */
U_CFUNC UBool
ubidi_get_स्मृतिः(Bidi_स्मृतिः_For_आवण्टनम् *p_स्मृतिः, int32_t *p_आकार, UBool mayAllocate, size_t आकार_Needed);

/* helper macros for each allocated array in UBiDi */
#परिभाषय get_Dir_Props_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->dir_Props_स्मृतिः, &(pBiDi)->dir_Props_आकार, \
                        (pBiDi)->mayAllocateText, (दीर्घता))

#परिभाषय get_स्तराः_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->स्तराः_स्मृतिः, &(pBiDi)->स्तराः_आकार, \
                        (pBiDi)->mayAllocateText, (दीर्घता))

#परिभाषय get_Runs_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->runs_स्मृतिः, &(pBiDi)->runs_आकार, \
                        (pBiDi)->mayAllocateRuns, (दीर्घता)*परिमाण(सततम्))

/* additional macros used by ubidi_open() - always allow allocation */
#परिभाषय get_Initial_Dir_Props_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->dir_Props_स्मृतिः, &(pBiDi)->dir_Props_आकार, \
                        true, (दीर्घता))

#परिभाषय get_Initial_स्तराः_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->स्तराः_स्मृतिः, &(pBiDi)->स्तराः_आकार, \
                        true, (दीर्घता))

#परिभाषय get_Initial_Openings_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->openings_स्मृतिः, &(pBiDi)->openings_आकार, \
                        true, (दीर्घता)*परिमाण(Opening))

#परिभाषय get_Initial_Paras_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->paras_स्मृतिः, &(pBiDi)->paras_आकार, \
                        true, (दीर्घता)*परिमाण(Para))

#परिभाषय get_Initial_Runs_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->runs_स्मृतिः, &(pBiDi)->runs_आकार, \
                        true, (दीर्घता)*परिमाण(सततम्))

#परिभाषय get_Initial_Isolates_स्मृतिः(pBiDi, दीर्घता) \
        ubidi_get_स्मृतिः((Bidi_स्मृतिः_For_आवण्टनम् *)&(pBiDi)->isolates_स्मृतिः, &(pBiDi)->isolates_आकार, \
                        true, (दीर्घता)*परिमाण(Isolate))

#यदि_अन्तः
