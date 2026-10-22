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

#جيڪڏهن_وصف_نه_ٿيل UBIDIIMP_H
#وصف_ڪريو UBIDIIMP_H

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
#وصف_ڪريو UBIDI_اندروني_RUNS_ONLY 64

// ICU-20853=ICU-20935 Solaris #defines CS and ES in sys/regset.h
#جيڪڏهن_وصف_ٿيل CS
#   undef CS
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل ES
#   undef ES
#جيڪڏهن_پورو

تعريفنوع uint8_t DirProp;
تعريفنوع uint32_t Flags;

/*  Comparing the description of the BiDi algorithm with this implementation
    is easier with the same names for the BiDi types in the code as there.
    See UCharDirection in uchar.h .
*/
تعداد {
    L=  U_کاٻو_منزل_ساڄو,                /*  0 */
    R=  U_ساڄو_منزل_کاٻو,                /*  1 */
    EN= U_EUROPEAN_عدد,              /*  2 */
    ES= U_EUROPEAN_عدد_SEPARATOR,    /*  3 */
    ET= U_EUROPEAN_عدد_TERMINATOR,   /*  4 */
    AN= U_ARABIC_عدد,                /*  5 */
    CS= U_COMMON_عدد_SEPARATOR,      /*  6 */
    B=  U_بلاڪ_SEPARATOR,              /*  7 */
    S=  U_SEGMENT_SEPARATOR,            /*  8 */
    WS= U_WHITE_SPACE_NEUTRAL,          /*  9 */
    ON= U_OTHER_NEUTRAL,                /* 10 */
    LRE=U_کاٻو_منزل_ساڄو_EMBEDDING,      /* 11 */
    LRO=U_کاٻو_منزل_ساڄو_OVERRIDE,       /* 12 */
    AL= U_ساڄو_منزل_کاٻو_ARABIC,         /* 13 */
    RLE=U_ساڄو_منزل_کاٻو_EMBEDDING,      /* 14 */
    RLO=U_ساڄو_منزل_کاٻو_OVERRIDE,       /* 15 */
    PDF=U_ڪڍو_DIRECTIONAL_FORMAT,       /* 16 */
    NSM=U_DIR_NON_SPACING_MARK,         /* 17 */
    BN= U_BOUNDARY_NEUTRAL,             /* 18 */
    FSI=U_پهريون_STRONG_ISOLATE,         /* 19 */
    LRI=U_کاٻو_منزل_ساڄو_ISOLATE,        /* 20 */
    RLI=U_ساڄو_منزل_کاٻو_ISOLATE,        /* 21 */
    PDI=U_ڪڍو_DIRECTIONAL_ISOLATE,      /* 22 */
    ENL,    /* EN after W7 */           /* 23 */
    ENR,    /* EN not subject to W7 */  /* 24 */
    dir_Prop_ڳڻپ
};

/*  Sometimes, bit values are more appropriate
    to deal with directionality properties.
    Abbreviations in these macro names refer to names
    used in the BiDi algorithm.
*/
#وصف_ڪريو DIRPROP_FLAG(dir) (1UL<<(dir))
#وصف_ڪريو خالص_DIRPROP(prop)  ((prop)&~0xE0)    ?????????????????????????

/* special flag for multiple runs from explicit embedding codes */
#وصف_ڪريو DIRPROP_FLAG_MULTI_RUNS (1UL<<31)

/* are there any characters that are LTR or RTL? */
#وصف_ڪريو نقاب_LTR (DIRPROP_FLAG(L)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(ENL)|DIRPROP_FLAG(ENR)|DIRPROP_FLAG(AN)|DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(LRI))
#وصف_ڪريو نقاب_RTL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(RLI))
#وصف_ڪريو نقاب_R_AL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL))
#وصف_ڪريو نقاب_STRONG_EN_AN (DIRPROP_FLAG(L)|DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(AN))

/* explicit embedding codes */
#وصف_ڪريو نقاب_EXPLICIT (DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(PDF))

/* explicit isolate codes */
#وصف_ڪريو نقاب_ISO (DIRPROP_FLAG(LRI)|DIRPROP_FLAG(RLI)|DIRPROP_FLAG(FSI)|DIRPROP_FLAG(PDI))

#وصف_ڪريو نقاب_BN_EXPLICIT (DIRPROP_FLAG(BN)|نقاب_EXPLICIT)

/* paragraph and segment separators */
#وصف_ڪريو نقاب_B_S (DIRPROP_FLAG(B)|DIRPROP_FLAG(S))

/* all types that are counted as White Space or Neutral in some steps */
#وصف_ڪريو نقاب_WS (نقاب_B_S|DIRPROP_FLAG(WS)|نقاب_BN_EXPLICIT|نقاب_ISO)

/* types that are neutrals or could becomes neutrals in (Wn) */
#وصف_ڪريو نقاب_POSSIBLE_N (DIRPROP_FLAG(ON)|DIRPROP_FLAG(CS)|DIRPROP_FLAG(ES)|DIRPROP_FLAG(ET)|نقاب_WS)

/*
 *  These types may be changed to "e",
 *  the embedding type (L or R) of the run,
 *  in the BiDi algorithm (N2)
 */
#وصف_ڪريو نقاب_EMBEDDING (DIRPROP_FLAG(NSM)|نقاب_POSSIBLE_N)

/* the dirProp's L and R are defined to 0 and 1 values in UCharDirection */
#وصف_ڪريو GET_LR_FROM_LEVEL(level) ((DirProp)((level)&1))

#وصف_ڪريو IS_DEFAULT_LEVEL(level) ((level)>=0xfe)

/*
 *  The following bit is used for the directional isolate status.
 *  Stack entries corresponding to isolate sequences are greater than ISOLATE.
 */
#وصف_ڪريو ISOLATE  0x0100

U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_اشاريو(ثابت UBiDi *pBiDi, int32_t اشاريو);

#وصف_ڪريو GET_PARALEVEL(ubidi, اشاريو) \
            ((UBiDiLevel)(!(ubidi)->defaultParaLevel || (اشاريو)<(ubidi)->paras[0].حد_بندي ? \
                         (ubidi)->paraLevel : ubidi_get_Para_Level_At_اشاريو((ubidi), (اشاريو))))

/* number of paras entries allocated initially without malloc */
#وصف_ڪريو SIMPLE_PARAS_ڳڻپ      10
/* number of isolate entries allocated initially without malloc */
#وصف_ڪريو SIMPLE_ISOLATES_ڳڻپ   5
/* number of isolate run entries for paired brackets allocated initially without malloc */
#وصف_ڪريو SIMPLE_OPENINGS_ڳڻپ   20

#وصف_ڪريو CR  0x000D
#وصف_ڪريو LF  0x000A

/* Run structure for reordering --------------------------------------------- */
تعداد {
    LRM_BEFORE=1,
    LRM_AFTER=2,
    RLM_BEFORE=4,
    RLM_AFTER=8
};

تعريفنوع بنية Para {
    int32_t حد_بندي;
    int32_t level;
} Para;

تعداد {                                  /* flags for Opening.flags */
    مليو_L=DIRPROP_FLAG(L),
    مليو_R=DIRPROP_FLAG(R)
};

تعريفنوع بنية Opening {
    int32_t position;                   /* position of opening bracket */
    int32_t ميلاپ;                      /* matching char or -position of closing bracket */
    int32_t contextPos;                 /* position of last strong char found before opening */
    uint16_t flags;                     /* bits for L or R/AL found within the pair */
    UBiDiDirection contextDir;          /* L or R according to last strong char before opening */
    uint8_t filler;                     /* to complete a nice multiple of 4 chars */
} Opening;

تعريفنوع بنية Iso_لاڳيتو {
    int32_t  contextPos;                /* position of char determining context */
    uint16_t شروعات;                     /* index of first opening entry for this run */
    uint16_t حد_بندي;                     /* index after last opening entry for this run */
    UBiDiLevel level;                   /* level of this run */
    DirProp آخري_Strong;                 /* bidi class of last strong char found in this run */
    DirProp آخري_بنياد;                   /* bidi class of last base char found in this run */
    UBiDiDirection contextDir;          /* L or R to use as context for following openings */
} Iso_لاڳيتو;

تعريفنوع بنية Bracket_معلومات {
    UBiDi   *pBiDi;
    /* array of opening entries which should be enough in most cases; no malloc() */
    Opening simpleOpenings[SIMPLE_OPENINGS_ڳڻپ];
    Opening *openings;                  /* pointer to current array of entries */
    int32_t openings_ڳڻپ;              /* number of allocated entries */
    int32_t iso_لاڳيتو_آخري;                 /* index of last used entry */
    /* array of nested isolated sequence entries; can never excess UBIDI_MAX_EXPLICIT_LEVEL
       + 1 for index 0, + 1 for before the first isolated sequence */
    Iso_لاڳيتو  isoRuns[UBIDI_MAX_EXPLICIT_LEVEL+2];
    UBool isNumbersSpecial;             /* reordering mode for NUMBERS_SPECIAL */
} Bracket_معلومات;

تعريفنوع بنية Isolate {
    int32_t شروعات_ON;
    int32_t start1;
    int32_t state;
    int16_t stateImp;
} Isolate;

تعريفنوع بنية لاڳيتو {
    int32_t logical_شروعات,   /* first character of the run; b31 indicates even/odd level */
            visual_حد_بندي,    /* last visual position of the run +1 */
            وجهو_Remove;   /* if >0, flags for inserting LRM/RLM before/after run,
                               if <0, count of bidi controls within run            */
} لاڳيتو;

/* in a Run, logicalStart will get this bit set if the run level is odd */
#وصف_ڪريو اشاريو_ODD_ٻٽو_انگ (1UL<<31)

#وصف_ڪريو MAKE_اشاريو_ODD_PAIR(اشاريو, level) ((اشاريو)|((int32_t)((level)&1)<<31))
#وصف_ڪريو جوڙيو_ODD_ٻٽو_انگ_FROM_LEVEL(x, level)  ((x)|=((int32_t)((level)&1)<<31))
#وصف_ڪريو REMOVE_ODD_ٻٽو_انگ(x)                 ((x)&=~اشاريو_ODD_ٻٽو_انگ)

#وصف_ڪريو GET_اشاريو(x)   ((x)&~اشاريو_ODD_ٻٽو_انگ)
#وصف_ڪريو GET_ODD_ٻٽو_انگ(x) ((uint32_t)(x)>>31)
#وصف_ڪريو IS_ODD_لاڳيتو(x)  ((UBool)(((x)&اشاريو_ODD_ٻٽو_انگ)!=0))
#وصف_ڪريو IS_EVEN_لاڳيتو(x) ((UBool)(((x)&اشاريو_ODD_ٻٽو_انگ)==0))

U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UError_ڪوڊ *p_Error_ڪوڊ);

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

#وصف_ڪريو IS_BIDI_CONTROL_CHAR(c) (((uint32_t)(c)&0xfffffffc)==ZWNJ_CHAR || (uint32_t)((c)-LRE_CHAR)<5 || (uint32_t)((c)-LRI_CHAR)<4)

/* InsertPoints structure for noting where to put BiDi marks ---------------- */

تعريفنوع بنية نقطو {
    int32_t pos;            /* position in text */
    int32_t flag;           /* flag for LRM/RLM, before/after */
} نقطو;

تعريفنوع بنية وجهو_Points_2 {
    int32_t صلاحيت;       /* number of points allocated */
    int32_t سائيز_2;           /* number of points used */
    int32_t confirmed;      /* number of points confirmed */
    UError_ڪوڊ error_ڪوڊ;   /* for eventual memory shortage */
    نقطو *points;          /* pointer to array of points */
} وجهو_Points_2;


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
    int32_t original_ڊيگهائي;

    /* if the UBIDI_OPTION_STREAMING option is set, this is the length
     * of text actually processed by ubidi_setPara, which may be shorter than
     * the original length.
     * Otherwise, it is identical to the original length.
     */
    int32_t ڊيگهائي;

    /* if the UBIDI_OPTION_REMOVE_CONTROLS option is set, and/or
     * marks are allowed to be inserted in one of the reordering mode, the
     * length of the result string may be different from the processed length.
     */
    int32_t نتيجو_ڊيگهائي;

    /* memory sizes in bytes */
    int32_t dir_Props_سائيز, سطحون_سائيز, openings_سائيز, paras_سائيز, runs_سائيز, isolates_سائيز;

    /* allocated memory */
    DirProp *dir_Props_يادگيري;
    UBiDiLevel *سطحون_يادگيري;
    Opening *openings_يادگيري;
    Para *paras_يادگيري;
    لاڳيتو *runs_يادگيري;
    Isolate *isolates_يادگيري;

    /* indicators for whether memory may be allocated after ubidi_open() */
    UBool may_Allocate_متن, mayAllocateRuns;

    /* arrays with one value per text-character */
    DirProp *dirProps;
    UBiDiLevel *سطحون;

    /* are we performing an approximation of the "inverse BiDi" algorithm? */
    UBool isInverse;

    /* are we using the basic algorithm or its variation? */
    UBi_Di_Reordering_گهڻائي reordering_گهڻائي;

    /* UBIDI_REORDER_xxx values must be ordered so that all the regular
     * logical to visual modes come first, and all inverse BiDi modes
     * come last.
     */
    #وصف_ڪريو UBIDI_REORDER_آخري_LOGICAL_منزل_VISUAL    UBIDI_REORDER_NUMBERS_SPECIAL

    /* bitmask for reordering options */
    uint32_t reorderingOptions;

    /* must block separators receive level 0? */
    UBool ترتيب_Paragraphs_LTR;

    /* the paragraph level */
    UBiDiLevel paraLevel;
    /* original paraLevel when contextual */
    /* must be one of UBIDI_DEFAULT_xxx or 0 if not contextual */
    UBiDiLevel defaultParaLevel;

    /* context data */
    ثابت UChar *prologue;
    int32_t pro_ڊيگهائي;
    ثابت UChar *epilogue;
    int32_t epi_ڊيگهائي;

    /* the following is set in ubidi_setPara, used in processPropertySeq */
    ثابت بنية ImpTabPair * pImpTabPair;  /* pointer to levels state table pair */

    /* the overall paragraph or line directionality - see UBiDiDirection */
    UBiDiDirection direction;

    /* flags is a bit set for which directional properties are in the text */
    Flags flags;

    /* lastArabicPos is index to the last AL in the text, -1 if none */
    int32_t آخري_Arabic_Pos;

    /* characters after trailingWSStart are WS and are */
    /* implicitly at the paraLevel (rule (L1)) - levels may not reflect that */
    int32_t trailingWSStart;

    /* fields for paragraph handling */
    int32_t para_ڳڻپ;                  /* set in getDirProps() */
    /* filled in getDirProps() */
    Para *paras;

    /* for relatively short text, we only need a tiny array of paras (no malloc()) */
    Para simpleParas[SIMPLE_PARAS_ڳڻپ];

    /* fields for line reordering */
    int32_t لاڳيتو_ڳڻپ;     /* ==-1: runs not set up yet */
    لاڳيتو *runs;

    /* for non-mixed text, we only need a tiny array of runs (no malloc()) */
    لاڳيتو simpleRuns[1];

    /* maximum or current nesting depth of isolate sequences */
    /* Within resolveExplicitLevels() and checkExplicitLevels(), this is the maximal
       nesting encountered.
       Within resolveImplicitLevels(), this is the index of the current isolates
       stack entry. */
    int32_t isolate_ڳڻپ;
    Isolate *isolates;

    /* for simple text, have a small stack (no malloc()) */
    Isolate simpleIsolates[SIMPLE_ISOLATES_ڳڻپ];

    /* for inverse Bidi with insertion of directional marks */
    وجهو_Points_2 وجهو_Points;

    /* for option UBIDI_OPTION_REMOVE_CONTROLS */
    int32_t control_ڳڻپ;

    /* for Bidi class callback */
    UBiDiClassCallback *fnClassCallback;    /* action pointer */
    ثابت فراغ *coClassCallback;            /* context pointer */
};

#وصف_ڪريو IS_VALID_PARA(x) ((x) && ((x)->pParaBiDi==(x)))
#وصف_ڪريو IS_VALID_PARA_OR_سٽ(x) ((x) && ((x)->pParaBiDi==(x) || (((x)->pParaBiDi) && (x)->pParaBiDi->pParaBiDi==(x)->pParaBiDi)))

تعريفنوع اتحاد {
    DirProp *dir_Props_يادگيري;
    UBiDiLevel *سطحون_يادگيري;
    Opening *openings_يادگيري;
    Para *paras_يادگيري;
    لاڳيتو *runs_يادگيري;
    Isolate *isolates_يادگيري;
} Bidi_يادگيري_For_ورهاست;

/* Macros for initial checks at function entry */
#وصف_ڪريو RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrcode, retvalue) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((pErrcode)==عدم || U_FAILURE(*pErrcode)) أرجع retvalue; \
} UPRV_بلاڪ_MACRO_پڇاڙي
#وصف_ڪريو RETURN_IF_NOT_VALID_PARA(bidi, errcode, retvalue) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي
#وصف_ڪريو RETURN_IF_NOT_VALID_PARA_OR_سٽ(bidi, errcode, retvalue) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA_OR_سٽ(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي
#وصف_ڪريو RETURN_IF_BAD_RANGE(arg, شروعات, حد_بندي, errcode, retvalue) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((arg)<(شروعات) || (arg)>=(حد_بندي)) { \
        (errcode)=U_ILLEGAL_دليل_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

#وصف_ڪريو RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrcode) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((pErrcode)==عدم || U_FAILURE(*pErrcode)) أرجع; \
} UPRV_بلاڪ_MACRO_پڇاڙي
#وصف_ڪريو RETURN_VOID_IF_NOT_VALID_PARA(bidi, errcode) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي
#وصف_ڪريو RETURN_VOID_IF_NOT_VALID_PARA_OR_سٽ(bidi, errcode) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA_OR_سٽ(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي
#وصف_ڪريو RETURN_VOID_IF_BAD_RANGE(arg, شروعات, حد_بندي, errcode) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((arg)<(شروعات) || (arg)>=(حد_بندي)) { \
        (errcode)=U_ILLEGAL_دليل_ERROR; \
        أرجع; \
    } \
} UPRV_بلاڪ_MACRO_پڇاڙي

/* helper function to (re)allocate memory if allowed */
U_CFUNC UBool
ubidi_get_يادگيري(Bidi_يادگيري_For_ورهاست *p_يادگيري, int32_t *p_سائيز, UBool mayAllocate, size_t سائيز_Needed);

/* helper macros for each allocated array in UBiDi */
#وصف_ڪريو get_Dir_Props_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->dir_Props_يادگيري, &(pBiDi)->dir_Props_سائيز, \
                        (pBiDi)->may_Allocate_متن, (ڊيگهائي))

#وصف_ڪريو get_سطحون_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->سطحون_يادگيري, &(pBiDi)->سطحون_سائيز, \
                        (pBiDi)->may_Allocate_متن, (ڊيگهائي))

#وصف_ڪريو get_Runs_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->runs_يادگيري, &(pBiDi)->runs_سائيز, \
                        (pBiDi)->mayAllocateRuns, (ڊيگهائي)*حجم(لاڳيتو))

/* additional macros used by ubidi_open() - always allow allocation */
#وصف_ڪريو get_Initial_Dir_Props_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->dir_Props_يادگيري, &(pBiDi)->dir_Props_سائيز, \
                        true, (ڊيگهائي))

#وصف_ڪريو get_Initial_سطحون_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->سطحون_يادگيري, &(pBiDi)->سطحون_سائيز, \
                        true, (ڊيگهائي))

#وصف_ڪريو get_Initial_Openings_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->openings_يادگيري, &(pBiDi)->openings_سائيز, \
                        true, (ڊيگهائي)*حجم(Opening))

#وصف_ڪريو get_Initial_Paras_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->paras_يادگيري, &(pBiDi)->paras_سائيز, \
                        true, (ڊيگهائي)*حجم(Para))

#وصف_ڪريو get_Initial_Runs_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->runs_يادگيري, &(pBiDi)->runs_سائيز, \
                        true, (ڊيگهائي)*حجم(لاڳيتو))

#وصف_ڪريو get_Initial_Isolates_يادگيري(pBiDi, ڊيگهائي) \
        ubidi_get_يادگيري((Bidi_يادگيري_For_ورهاست *)&(pBiDi)->isolates_يادگيري, &(pBiDi)->isolates_سائيز, \
                        true, (ڊيگهائي)*حجم(Isolate))

#جيڪڏهن_پورو
