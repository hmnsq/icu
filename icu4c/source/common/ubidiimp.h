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

#جے_بنا_تعریف UBIDIIMP_H
#تعریف_کرو UBIDIIMP_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/ubidi.h"
#ضمن "unicode/uchar.h"
#ضمن "ubidi_props.h"

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
#تعریف_کرو UBIDI_اندرونی_RUNS_ONLY 64

// ICU-20853=ICU-20935 Solaris #defines CS and ES in sys/regset.h
#جے_تعریف_شدہ CS
#   undef CS
#جے_انت
#جے_تعریف_شدہ ES
#   undef ES
#جے_انت

تعريفنوع uint8_t DirProp;
تعريفنوع uint32_t Flags;

/*  Comparing the description of the BiDi algorithm with this implementation
    is easier with the same names for the BiDi types in the code as there.
    See UCharDirection in uchar.h .
*/
تعداد {
    L=  U_بایاں_منزل_دایاں,                /*  0 */
    R=  U_دایاں_منزل_بایاں,                /*  1 */
    EN= U_EUROPEAN_نمبر,              /*  2 */
    ES= U_EUROPEAN_نمبر_SEPARATOR,    /*  3 */
    ET= U_EUROPEAN_نمبر_TERMINATOR,   /*  4 */
    AN= U_ARABIC_نمبر,                /*  5 */
    CS= U_COMMON_نمبر_SEPARATOR,      /*  6 */
    B=  U_بلاک_SEPARATOR,              /*  7 */
    S=  U_SEGMENT_SEPARATOR,            /*  8 */
    WS= U_WHITE_SPACE_NEUTRAL,          /*  9 */
    ON= U_OTHER_NEUTRAL,                /* 10 */
    LRE=U_بایاں_منزل_دایاں_EMBEDDING,      /* 11 */
    LRO=U_بایاں_منزل_دایاں_OVERRIDE,       /* 12 */
    AL= U_دایاں_منزل_بایاں_ARABIC,         /* 13 */
    RLE=U_دایاں_منزل_بایاں_EMBEDDING,      /* 14 */
    RLO=U_دایاں_منزل_بایاں_OVERRIDE,       /* 15 */
    PDF=U_نکالو_DIRECTIONAL_FORMAT,       /* 16 */
    NSM=U_DIR_NON_SPACING_MARK,         /* 17 */
    BN= U_BOUNDARY_NEUTRAL,             /* 18 */
    FSI=U_پہلا_STRONG_ISOLATE,         /* 19 */
    LRI=U_بایاں_منزل_دایاں_ISOLATE,        /* 20 */
    RLI=U_دایاں_منزل_بایاں_ISOLATE,        /* 21 */
    PDI=U_نکالو_DIRECTIONAL_ISOLATE,      /* 22 */
    ENL,    /* EN after W7 */           /* 23 */
    ENR,    /* EN not subject to W7 */  /* 24 */
    dir_Prop_گنتی
};

/*  Sometimes, bit values are more appropriate
    to deal with directionality properties.
    Abbreviations in these macro names refer to names
    used in the BiDi algorithm.
*/
#تعریف_کرو DIRPROP_FLAG(dir) (1UL<<(dir))
#تعریف_کرو خالص_DIRPROP(prop)  ((prop)&~0xE0)    ?????????????????????????

/* special flag for multiple runs from explicit embedding codes */
#تعریف_کرو DIRPROP_FLAG_MULTI_RUNS (1UL<<31)

/* are there any characters that are LTR or RTL? */
#تعریف_کرو نقاب_LTR (DIRPROP_FLAG(L)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(ENL)|DIRPROP_FLAG(ENR)|DIRPROP_FLAG(AN)|DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(LRI))
#تعریف_کرو نقاب_RTL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(RLI))
#تعریف_کرو نقاب_R_AL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL))
#تعریف_کرو نقاب_STRONG_EN_AN (DIRPROP_FLAG(L)|DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(AN))

/* explicit embedding codes */
#تعریف_کرو نقاب_EXPLICIT (DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(PDF))

/* explicit isolate codes */
#تعریف_کرو نقاب_ISO (DIRPROP_FLAG(LRI)|DIRPROP_FLAG(RLI)|DIRPROP_FLAG(FSI)|DIRPROP_FLAG(PDI))

#تعریف_کرو نقاب_BN_EXPLICIT (DIRPROP_FLAG(BN)|نقاب_EXPLICIT)

/* paragraph and segment separators */
#تعریف_کرو نقاب_B_S (DIRPROP_FLAG(B)|DIRPROP_FLAG(S))

/* all types that are counted as White Space or Neutral in some steps */
#تعریف_کرو نقاب_WS (نقاب_B_S|DIRPROP_FLAG(WS)|نقاب_BN_EXPLICIT|نقاب_ISO)

/* types that are neutrals or could becomes neutrals in (Wn) */
#تعریف_کرو نقاب_POSSIBLE_N (DIRPROP_FLAG(ON)|DIRPROP_FLAG(CS)|DIRPROP_FLAG(ES)|DIRPROP_FLAG(ET)|نقاب_WS)

/*
 *  These types may be changed to "e",
 *  the embedding type (L or R) of the run,
 *  in the BiDi algorithm (N2)
 */
#تعریف_کرو نقاب_EMBEDDING (DIRPROP_FLAG(NSM)|نقاب_POSSIBLE_N)

/* the dirProp's L and R are defined to 0 and 1 values in UCharDirection */
#تعریف_کرو GET_LR_FROM_LEVEL(level) ((DirProp)((level)&1))

#تعریف_کرو IS_DEFAULT_LEVEL(level) ((level)>=0xfe)

/*
 *  The following bit is used for the directional isolate status.
 *  Stack entries corresponding to isolate sequences are greater than ISOLATE.
 */
#تعریف_کرو ISOLATE  0x0100

U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_اشاریہ(ثابت UBiDi *pBiDi, int32_t اشاریہ);

#تعریف_کرو GET_PARALEVEL(ubidi, اشاریہ) \
            ((UBiDiLevel)(!(ubidi)->defaultParaLevel || (اشاریہ)<(ubidi)->paras[0].حد_بندی ? \
                         (ubidi)->paraLevel : ubidi_get_Para_Level_At_اشاریہ((ubidi), (اشاریہ))))

/* number of paras entries allocated initially without malloc */
#تعریف_کرو SIMPLE_PARAS_گنتی      10
/* number of isolate entries allocated initially without malloc */
#تعریف_کرو SIMPLE_ISOLATES_گنتی   5
/* number of isolate run entries for paired brackets allocated initially without malloc */
#تعریف_کرو SIMPLE_OPENINGS_گنتی   20

#تعریف_کرو CR  0x000D
#تعریف_کرو LF  0x000A

/* Run structure for reordering --------------------------------------------- */
تعداد {
    LRM_BEFORE=1,
    LRM_AFTER=2,
    RLM_BEFORE=4,
    RLM_AFTER=8
};

تعريفنوع بنية Para {
    int32_t حد_بندی;
    int32_t level;
} Para;

تعداد {                                  /* flags for Opening.flags */
    لبھیا_L=DIRPROP_FLAG(L),
    لبھیا_R=DIRPROP_FLAG(R)
};

تعريفنوع بنية Opening {
    int32_t position;                   /* position of opening bracket */
    int32_t میل;                      /* matching char or -position of closing bracket */
    int32_t contextPos;                 /* position of last strong char found before opening */
    uint16_t flags;                     /* bits for L or R/AL found within the pair */
    UBiDiDirection contextDir;          /* L or R according to last strong char before opening */
    uint8_t filler;                     /* to complete a nice multiple of 4 chars */
} Opening;

تعريفنوع بنية Iso_لگاتار {
    int32_t  contextPos;                /* position of char determining context */
    uint16_t شروع;                     /* index of first opening entry for this run */
    uint16_t حد_بندی;                     /* index after last opening entry for this run */
    UBiDiLevel level;                   /* level of this run */
    DirProp آخری_Strong;                 /* bidi class of last strong char found in this run */
    DirProp آخری_ادھار;                   /* bidi class of last base char found in this run */
    UBiDiDirection contextDir;          /* L or R to use as context for following openings */
} Iso_لگاتار;

تعريفنوع بنية Bracket_انکڑے {
    UBiDi   *pBiDi;
    /* array of opening entries which should be enough in most cases; no malloc() */
    Opening simpleOpenings[SIMPLE_OPENINGS_گنتی];
    Opening *openings;                  /* pointer to current array of entries */
    int32_t openings_گنتی;              /* number of allocated entries */
    int32_t iso_لگاتار_آخری;                 /* index of last used entry */
    /* array of nested isolated sequence entries; can never excess UBIDI_MAX_EXPLICIT_LEVEL
       + 1 for index 0, + 1 for before the first isolated sequence */
    Iso_لگاتار  isoRuns[UBIDI_MAX_EXPLICIT_LEVEL+2];
    UBool isNumbersSpecial;             /* reordering mode for NUMBERS_SPECIAL */
} Bracket_انکڑے;

تعريفنوع بنية Isolate {
    int32_t شروع_ON;
    int32_t start1;
    int32_t state;
    int16_t stateImp;
} Isolate;

تعريفنوع بنية لگاتار {
    int32_t logical_شروع,   /* first character of the run; b31 indicates even/odd level */
            visual_حد_بندی,    /* last visual position of the run +1 */
            داخل_کرو_Remove;   /* if >0, flags for inserting LRM/RLM before/after run,
                               if <0, count of bidi controls within run            */
} لگاتار;

/* in a Run, logicalStart will get this bit set if the run level is odd */
#تعریف_کرو اشاریہ_ODD_ثنائی_ہندسہ (1UL<<31)

#تعریف_کرو MAKE_اشاریہ_ODD_PAIR(اشاریہ, level) ((اشاریہ)|((int32_t)((level)&1)<<31))
#تعریف_کرو جوڑو_ODD_ثنائی_ہندسہ_FROM_LEVEL(x, level)  ((x)|=((int32_t)((level)&1)<<31))
#تعریف_کرو REMOVE_ODD_ثنائی_ہندسہ(x)                 ((x)&=~اشاریہ_ODD_ثنائی_ہندسہ)

#تعریف_کرو GET_اشاریہ(x)   ((x)&~اشاریہ_ODD_ثنائی_ہندسہ)
#تعریف_کرو GET_ODD_ثنائی_ہندسہ(x) ((uint32_t)(x)>>31)
#تعریف_کرو IS_ODD_لگاتار(x)  ((UBool)(((x)&اشاریہ_ODD_ثنائی_ہندسہ)!=0))
#تعریف_کرو IS_EVEN_لگاتار(x) ((UBool)(((x)&اشاریہ_ODD_ثنائی_ہندسہ)==0))

U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UError_کوڈ *p_Error_کوڈ);

/** BiDi control code points */
تعداد {
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

#تعریف_کرو IS_BIDI_CONTROL_CHAR(c) (((uint32_t)(c)&0xfffffffc)==ZWNJ_CHAR || (uint32_t)((c)-LRE_CHAR)<5 || (uint32_t)((c)-LRI_CHAR)<4)

/* InsertPoints structure for noting where to put BiDi marks ---------------- */

تعريفنوع بنية نقطہ {
    int32_t pos;            /* position in text */
    int32_t flag;           /* flag for LRM/RLM, before/after */
} نقطہ;

تعريفنوع بنية داخل_کرو_Points_2 {
    int32_t سمرتھا;       /* number of points allocated */
    int32_t ناپ_2;           /* number of points used */
    int32_t confirmed;      /* number of points confirmed */
    UError_کوڈ error_کوڈ;   /* for eventual memory shortage */
    نقطہ *points;          /* pointer to array of points */
} داخل_کرو_Points_2;


/* UBiDi structure ----------------------------------------------------------- */

بنية UBiDi {
    /* pointer to parent paragraph object (pointer to self if this object is
     * a paragraph object); set to NULL in a newly opened object; set to a
     * real value after a successful execution of ubidi_setPara or ubidi_setLine
     */
    ثابت UBiDi * pParaBiDi;

    /* alias pointer to the current text */
    ثابت UChar *متن;

    /* length of the current text */
    int32_t original_لمائی;

    /* if the UBIDI_OPTION_STREAMING option is set, this is the length
     * of text actually processed by ubidi_setPara, which may be shorter than
     * the original length.
     * Otherwise, it is identical to the original length.
     */
    int32_t لمائی;

    /* if the UBIDI_OPTION_REMOVE_CONTROLS option is set, and/or
     * marks are allowed to be inserted in one of the reordering mode, the
     * length of the result string may be different from the processed length.
     */
    int32_t نتیجہ_لمائی;

    /* memory sizes in bytes */
    int32_t dir_Props_ناپ, پدھراں_ناپ, openings_ناپ, paras_ناپ, runs_ناپ, isolates_ناپ;

    /* allocated memory */
    DirProp *dir_Props_حافظہ;
    UBiDiLevel *پدھراں_حافظہ;
    Opening *openings_حافظہ;
    Para *paras_حافظہ;
    لگاتار *runs_حافظہ;
    Isolate *isolates_حافظہ;

    /* indicators for whether memory may be allocated after ubidi_open() */
    UBool may_Allocate_متن, mayAllocateRuns;

    /* arrays with one value per text-character */
    DirProp *dirProps;
    UBiDiLevel *پدھراں;

    /* are we performing an approximation of the "inverse BiDi" algorithm? */
    UBool isInverse;

    /* are we using the basic algorithm or its variation? */
    UBi_Di_Reordering_منوال reordering_منوال;

    /* UBIDI_REORDER_xxx values must be ordered so that all the regular
     * logical to visual modes come first, and all inverse BiDi modes
     * come last.
     */
    #تعریف_کرو UBIDI_REORDER_آخری_LOGICAL_منزل_VISUAL    UBIDI_REORDER_NUMBERS_SPECIAL

    /* bitmask for reordering options */
    uint32_t reorderingOptions;

    /* must block separators receive level 0? */
    UBool ترتیب_Paragraphs_LTR;

    /* the paragraph level */
    UBiDiLevel paraLevel;
    /* original paraLevel when contextual */
    /* must be one of UBIDI_DEFAULT_xxx or 0 if not contextual */
    UBiDiLevel defaultParaLevel;

    /* context data */
    ثابت UChar *prologue;
    int32_t pro_لمائی;
    ثابت UChar *epilogue;
    int32_t epi_لمائی;

    /* the following is set in ubidi_setPara, used in processPropertySeq */
    ثابت بنية ImpTabPair * pImpTabPair;  /* pointer to levels state table pair */

    /* the overall paragraph or line directionality - see UBiDiDirection */
    UBiDiDirection direction;

    /* flags is a bit set for which directional properties are in the text */
    Flags flags;

    /* lastArabicPos is index to the last AL in the text, -1 if none */
    int32_t آخری_Arabic_Pos;

    /* characters after trailingWSStart are WS and are */
    /* implicitly at the paraLevel (rule (L1)) - levels may not reflect that */
    int32_t trailingWSStart;

    /* fields for paragraph handling */
    int32_t para_گنتی;                  /* set in getDirProps() */
    /* filled in getDirProps() */
    Para *paras;

    /* for relatively short text, we only need a tiny array of paras (no malloc()) */
    Para simpleParas[SIMPLE_PARAS_گنتی];

    /* fields for line reordering */
    int32_t لگاتار_گنتی;     /* ==-1: runs not set up yet */
    لگاتار *runs;

    /* for non-mixed text, we only need a tiny array of runs (no malloc()) */
    لگاتار simpleRuns[1];

    /* maximum or current nesting depth of isolate sequences */
    /* Within resolveExplicitLevels() and checkExplicitLevels(), this is the maximal
       nesting encountered.
       Within resolveImplicitLevels(), this is the index of the current isolates
       stack entry. */
    int32_t isolate_گنتی;
    Isolate *isolates;

    /* for simple text, have a small stack (no malloc()) */
    Isolate simpleIsolates[SIMPLE_ISOLATES_گنتی];

    /* for inverse Bidi with insertion of directional marks */
    داخل_کرو_Points_2 داخل_کرو_Points;

    /* for option UBIDI_OPTION_REMOVE_CONTROLS */
    int32_t control_گنتی;

    /* for Bidi class callback */
    UBiDiClassCallback *fnClassCallback;    /* action pointer */
    ثابت فراغ *coClassCallback;            /* context pointer */
};

#تعریف_کرو IS_VALID_PARA(x) ((x) && ((x)->pParaBiDi==(x)))
#تعریف_کرو IS_VALID_PARA_OR_سطر(x) ((x) && ((x)->pParaBiDi==(x) || (((x)->pParaBiDi) && (x)->pParaBiDi->pParaBiDi==(x)->pParaBiDi)))

تعريفنوع اتحاد {
    DirProp *dir_Props_حافظہ;
    UBiDiLevel *پدھراں_حافظہ;
    Opening *openings_حافظہ;
    Para *paras_حافظہ;
    لگاتار *runs_حافظہ;
    Isolate *isolates_حافظہ;
} Bidi_حافظہ_For_ونڈ;

/* Macros for initial checks at function entry */
#تعریف_کرو RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrcode, retvalue) UPRV_بلاک_MACRO_BEGIN { \
    إذا((pErrcode)==عدم || U_FAILURE(*pErrcode)) أرجع retvalue; \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو RETURN_IF_NOT_VALID_PARA(bidi, errcode, retvalue) UPRV_بلاک_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو RETURN_IF_NOT_VALID_PARA_OR_سطر(bidi, errcode, retvalue) UPRV_بلاک_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA_OR_سطر(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو RETURN_IF_BAD_RANGE(arg, شروع, حد_بندی, errcode, retvalue) UPRV_بلاک_MACRO_BEGIN { \
    إذا((arg)<(شروع) || (arg)>=(حد_بندی)) { \
        (errcode)=U_ILLEGAL_دلیل_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلاک_MACRO_سرا

#تعریف_کرو RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrcode) UPRV_بلاک_MACRO_BEGIN { \
    إذا((pErrcode)==عدم || U_FAILURE(*pErrcode)) أرجع; \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو RETURN_VOID_IF_NOT_VALID_PARA(bidi, errcode) UPRV_بلاک_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع; \
    } \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو RETURN_VOID_IF_NOT_VALID_PARA_OR_سطر(bidi, errcode) UPRV_بلاک_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA_OR_سطر(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع; \
    } \
} UPRV_بلاک_MACRO_سرا
#تعریف_کرو RETURN_VOID_IF_BAD_RANGE(arg, شروع, حد_بندی, errcode) UPRV_بلاک_MACRO_BEGIN { \
    إذا((arg)<(شروع) || (arg)>=(حد_بندی)) { \
        (errcode)=U_ILLEGAL_دلیل_ERROR; \
        أرجع; \
    } \
} UPRV_بلاک_MACRO_سرا

/* helper function to (re)allocate memory if allowed */
U_CFUNC UBool
ubidi_get_حافظہ(Bidi_حافظہ_For_ونڈ *p_حافظہ, int32_t *p_ناپ, UBool mayAllocate, size_t ناپ_Needed);

/* helper macros for each allocated array in UBiDi */
#تعریف_کرو get_Dir_Props_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->dir_Props_حافظہ, &(pBiDi)->dir_Props_ناپ, \
                        (pBiDi)->may_Allocate_متن, (لمائی))

#تعریف_کرو get_پدھراں_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->پدھراں_حافظہ, &(pBiDi)->پدھراں_ناپ, \
                        (pBiDi)->may_Allocate_متن, (لمائی))

#تعریف_کرو get_Runs_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->runs_حافظہ, &(pBiDi)->runs_ناپ, \
                        (pBiDi)->mayAllocateRuns, (لمائی)*حجم(لگاتار))

/* additional macros used by ubidi_open() - always allow allocation */
#تعریف_کرو get_Initial_Dir_Props_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->dir_Props_حافظہ, &(pBiDi)->dir_Props_ناپ, \
                        true, (لمائی))

#تعریف_کرو get_Initial_پدھراں_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->پدھراں_حافظہ, &(pBiDi)->پدھراں_ناپ, \
                        true, (لمائی))

#تعریف_کرو get_Initial_Openings_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->openings_حافظہ, &(pBiDi)->openings_ناپ, \
                        true, (لمائی)*حجم(Opening))

#تعریف_کرو get_Initial_Paras_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->paras_حافظہ, &(pBiDi)->paras_ناپ, \
                        true, (لمائی)*حجم(Para))

#تعریف_کرو get_Initial_Runs_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->runs_حافظہ, &(pBiDi)->runs_ناپ, \
                        true, (لمائی)*حجم(لگاتار))

#تعریف_کرو get_Initial_Isolates_حافظہ(pBiDi, لمائی) \
        ubidi_get_حافظہ((Bidi_حافظہ_For_ونڈ *)&(pBiDi)->isolates_حافظہ, &(pBiDi)->isolates_ناپ, \
                        true, (لمائی)*حجم(Isolate))

#جے_انت
