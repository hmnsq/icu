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

#اگر_تعریف_نشده UBIDIIMP_H
#تعریف_کن UBIDIIMP_H

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
#تعریف_کن UBIDI_داخلی_RUNS_ONLY 64

// ICU-20853=ICU-20935 Solaris #defines CS and ES in sys/regset.h
#اگر_تعریف_شده CS
#   undef CS
#پایان_اگر
#اگر_تعریف_شده ES
#   undef ES
#پایان_اگر

تعريفنوع uint8_t DirProp;
تعريفنوع uint32_t Flags;

/*  Comparing the description of the BiDi algorithm with this implementation
    is easier with the same names for the BiDi types in the code as there.
    See UCharDirection in uchar.h .
*/
تعداد {
    L=  U_چپ_مقصد_راست,                /*  0 */
    R=  U_راست_مقصد_چپ,                /*  1 */
    EN= U_EUROPEAN_عدد,              /*  2 */
    ES= U_EUROPEAN_عدد_SEPARATOR,    /*  3 */
    ET= U_EUROPEAN_عدد_TERMINATOR,   /*  4 */
    AN= U_ARABIC_عدد,                /*  5 */
    CS= U_COMMON_عدد_SEPARATOR,      /*  6 */
    B=  U_بلوک_SEPARATOR,              /*  7 */
    S=  U_SEGMENT_SEPARATOR,            /*  8 */
    WS= U_WHITE_SPACE_NEUTRAL,          /*  9 */
    ON= U_OTHER_NEUTRAL,                /* 10 */
    LRE=U_چپ_مقصد_راست_EMBEDDING,      /* 11 */
    LRO=U_چپ_مقصد_راست_OVERRIDE,       /* 12 */
    AL= U_راست_مقصد_چپ_ARABIC,         /* 13 */
    RLE=U_راست_مقصد_چپ_EMBEDDING,      /* 14 */
    RLO=U_راست_مقصد_چپ_OVERRIDE,       /* 15 */
    PDF=U_برداشت_DIRECTIONAL_FORMAT,       /* 16 */
    NSM=U_DIR_NON_SPACING_MARK,         /* 17 */
    BN= U_BOUNDARY_NEUTRAL,             /* 18 */
    FSI=U_اولی_STRONG_ISOLATE,         /* 19 */
    LRI=U_چپ_مقصد_راست_ISOLATE,        /* 20 */
    RLI=U_راست_مقصد_چپ_ISOLATE,        /* 21 */
    PDI=U_برداشت_DIRECTIONAL_ISOLATE,      /* 22 */
    ENL,    /* EN after W7 */           /* 23 */
    ENR,    /* EN not subject to W7 */  /* 24 */
    dir_Prop_شمار
};

/*  Sometimes, bit values are more appropriate
    to deal with directionality properties.
    Abbreviations in these macro names refer to names
    used in the BiDi algorithm.
*/
#تعریف_کن DIRPROP_FLAG(dir) (1UL<<(dir))
#تعریف_کن خالص_DIRPROP(prop)  ((prop)&~0xE0)    ?????????????????????????

/* special flag for multiple runs from explicit embedding codes */
#تعریف_کن DIRPROP_FLAG_MULTI_RUNS (1UL<<31)

/* are there any characters that are LTR or RTL? */
#تعریف_کن ماسک_LTR (DIRPROP_FLAG(L)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(ENL)|DIRPROP_FLAG(ENR)|DIRPROP_FLAG(AN)|DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(LRI))
#تعریف_کن ماسک_RTL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(RLI))
#تعریف_کن ماسک_R_AL (DIRPROP_FLAG(R)|DIRPROP_FLAG(AL))
#تعریف_کن ماسک_STRONG_EN_AN (DIRPROP_FLAG(L)|DIRPROP_FLAG(R)|DIRPROP_FLAG(AL)|DIRPROP_FLAG(EN)|DIRPROP_FLAG(AN))

/* explicit embedding codes */
#تعریف_کن ماسک_EXPLICIT (DIRPROP_FLAG(LRE)|DIRPROP_FLAG(LRO)|DIRPROP_FLAG(RLE)|DIRPROP_FLAG(RLO)|DIRPROP_FLAG(PDF))

/* explicit isolate codes */
#تعریف_کن ماسک_ISO (DIRPROP_FLAG(LRI)|DIRPROP_FLAG(RLI)|DIRPROP_FLAG(FSI)|DIRPROP_FLAG(PDI))

#تعریف_کن ماسک_BN_EXPLICIT (DIRPROP_FLAG(BN)|ماسک_EXPLICIT)

/* paragraph and segment separators */
#تعریف_کن ماسک_B_S (DIRPROP_FLAG(B)|DIRPROP_FLAG(S))

/* all types that are counted as White Space or Neutral in some steps */
#تعریف_کن ماسک_WS (ماسک_B_S|DIRPROP_FLAG(WS)|ماسک_BN_EXPLICIT|ماسک_ISO)

/* types that are neutrals or could becomes neutrals in (Wn) */
#تعریف_کن ماسک_POSSIBLE_N (DIRPROP_FLAG(ON)|DIRPROP_FLAG(CS)|DIRPROP_FLAG(ES)|DIRPROP_FLAG(ET)|ماسک_WS)

/*
 *  These types may be changed to "e",
 *  the embedding type (L or R) of the run,
 *  in the BiDi algorithm (N2)
 */
#تعریف_کن ماسک_EMBEDDING (DIRPROP_FLAG(NSM)|ماسک_POSSIBLE_N)

/* the dirProp's L and R are defined to 0 and 1 values in UCharDirection */
#تعریف_کن GET_LR_FROM_LEVEL(level) ((DirProp)((level)&1))

#تعریف_کن IS_DEFAULT_LEVEL(level) ((level)>=0xfe)

/*
 *  The following bit is used for the directional isolate status.
 *  Stack entries corresponding to isolate sequences are greater than ISOLATE.
 */
#تعریف_کن ISOLATE  0x0100

U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_شاخص(ثابت UBiDi *pBiDi, int32_t شاخص);

#تعریف_کن GET_PARALEVEL(ubidi, شاخص) \
            ((UBiDiLevel)(!(ubidi)->defaultParaLevel || (شاخص)<(ubidi)->paras[0].حد ? \
                         (ubidi)->paraLevel : ubidi_get_Para_Level_At_شاخص((ubidi), (شاخص))))

/* number of paras entries allocated initially without malloc */
#تعریف_کن SIMPLE_PARAS_شمار      10
/* number of isolate entries allocated initially without malloc */
#تعریف_کن SIMPLE_ISOLATES_شمار   5
/* number of isolate run entries for paired brackets allocated initially without malloc */
#تعریف_کن SIMPLE_OPENINGS_شمار   20

#تعریف_کن CR  0x000D
#تعریف_کن LF  0x000A

/* Run structure for reordering --------------------------------------------- */
تعداد {
    LRM_BEFORE=1,
    LRM_AFTER=2,
    RLM_BEFORE=4,
    RLM_AFTER=8
};

تعريفنوع بنية Para {
    int32_t حد;
    int32_t level;
} Para;

تعداد {                                  /* flags for Opening.flags */
    یافت_L=DIRPROP_FLAG(L),
    یافت_R=DIRPROP_FLAG(R)
};

تعريفنوع بنية Opening {
    int32_t position;                   /* position of opening bracket */
    int32_t تطابق;                      /* matching char or -position of closing bracket */
    int32_t contextPos;                 /* position of last strong char found before opening */
    uint16_t flags;                     /* bits for L or R/AL found within the pair */
    UBiDiDirection contextDir;          /* L or R according to last strong char before opening */
    uint8_t filler;                     /* to complete a nice multiple of 4 chars */
} Opening;

تعريفنوع بنية Iso_پیاپی {
    int32_t  contextPos;                /* position of char determining context */
    uint16_t شروع;                     /* index of first opening entry for this run */
    uint16_t حد;                     /* index after last opening entry for this run */
    UBiDiLevel level;                   /* level of this run */
    DirProp آخرین_Strong;                 /* bidi class of last strong char found in this run */
    DirProp آخرین_پایه;                   /* bidi class of last base char found in this run */
    UBiDiDirection contextDir;          /* L or R to use as context for following openings */
} Iso_پیاپی;

تعريفنوع بنية Bracket_داده {
    UBiDi   *pBiDi;
    /* array of opening entries which should be enough in most cases; no malloc() */
    Opening simpleOpenings[SIMPLE_OPENINGS_شمار];
    Opening *openings;                  /* pointer to current array of entries */
    int32_t openings_شمار;              /* number of allocated entries */
    int32_t iso_پیاپی_آخرین;                 /* index of last used entry */
    /* array of nested isolated sequence entries; can never excess UBIDI_MAX_EXPLICIT_LEVEL
       + 1 for index 0, + 1 for before the first isolated sequence */
    Iso_پیاپی  isoRuns[UBIDI_MAX_EXPLICIT_LEVEL+2];
    UBool isNumbersSpecial;             /* reordering mode for NUMBERS_SPECIAL */
} Bracket_داده;

تعريفنوع بنية Isolate {
    int32_t شروع_ON;
    int32_t start1;
    int32_t state;
    int16_t stateImp;
} Isolate;

تعريفنوع بنية پیاپی {
    int32_t logical_شروع,   /* first character of the run; b31 indicates even/odd level */
            visual_حد,    /* last visual position of the run +1 */
            درج_Remove;   /* if >0, flags for inserting LRM/RLM before/after run,
                               if <0, count of bidi controls within run            */
} پیاپی;

/* in a Run, logicalStart will get this bit set if the run level is odd */
#تعریف_کن شاخص_ODD_بیت (1UL<<31)

#تعریف_کن MAKE_شاخص_ODD_PAIR(شاخص, level) ((شاخص)|((int32_t)((level)&1)<<31))
#تعریف_کن جمع_کن_ODD_بیت_FROM_LEVEL(x, level)  ((x)|=((int32_t)((level)&1)<<31))
#تعریف_کن REMOVE_ODD_بیت(x)                 ((x)&=~شاخص_ODD_بیت)

#تعریف_کن GET_شاخص(x)   ((x)&~شاخص_ODD_بیت)
#تعریف_کن GET_ODD_بیت(x) ((uint32_t)(x)>>31)
#تعریف_کن IS_ODD_پیاپی(x)  ((UBool)(((x)&شاخص_ODD_بیت)!=0))
#تعریف_کن IS_EVEN_پیاپی(x) ((UBool)(((x)&شاخص_ODD_بیت)==0))

U_CFUNC UBool
ubidi_getRuns(UBiDi *pBiDi, UError_کد *p_Error_کد);

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

#تعریف_کن IS_BIDI_CONTROL_CHAR(c) (((uint32_t)(c)&0xfffffffc)==ZWNJ_CHAR || (uint32_t)((c)-LRE_CHAR)<5 || (uint32_t)((c)-LRI_CHAR)<4)

/* InsertPoints structure for noting where to put BiDi marks ---------------- */

تعريفنوع بنية نقطه {
    int32_t pos;            /* position in text */
    int32_t flag;           /* flag for LRM/RLM, before/after */
} نقطه;

تعريفنوع بنية درج_Points_2 {
    int32_t ظرفیت;       /* number of points allocated */
    int32_t اندازه_2;           /* number of points used */
    int32_t confirmed;      /* number of points confirmed */
    UError_کد error_کد;   /* for eventual memory shortage */
    نقطه *points;          /* pointer to array of points */
} درج_Points_2;


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
    int32_t original_درازی;

    /* if the UBIDI_OPTION_STREAMING option is set, this is the length
     * of text actually processed by ubidi_setPara, which may be shorter than
     * the original length.
     * Otherwise, it is identical to the original length.
     */
    int32_t درازی;

    /* if the UBIDI_OPTION_REMOVE_CONTROLS option is set, and/or
     * marks are allowed to be inserted in one of the reordering mode, the
     * length of the result string may be different from the processed length.
     */
    int32_t نتیجه_درازی;

    /* memory sizes in bytes */
    int32_t dir_Props_اندازه, سطوح_اندازه, openings_اندازه, paras_اندازه, runs_اندازه, isolates_اندازه;

    /* allocated memory */
    DirProp *dir_Props_حافظه;
    UBiDiLevel *سطوح_حافظه;
    Opening *openings_حافظه;
    Para *paras_حافظه;
    پیاپی *runs_حافظه;
    Isolate *isolates_حافظه;

    /* indicators for whether memory may be allocated after ubidi_open() */
    UBool may_Allocate_متن, mayAllocateRuns;

    /* arrays with one value per text-character */
    DirProp *dirProps;
    UBiDiLevel *سطوح;

    /* are we performing an approximation of the "inverse BiDi" algorithm? */
    UBool isInverse;

    /* are we using the basic algorithm or its variation? */
    UBi_Di_Reordering_نما reordering_نما;

    /* UBIDI_REORDER_xxx values must be ordered so that all the regular
     * logical to visual modes come first, and all inverse BiDi modes
     * come last.
     */
    #تعریف_کن UBIDI_REORDER_آخرین_LOGICAL_مقصد_VISUAL    UBIDI_REORDER_NUMBERS_SPECIAL

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
    int32_t pro_درازی;
    ثابت UChar *epilogue;
    int32_t epi_درازی;

    /* the following is set in ubidi_setPara, used in processPropertySeq */
    ثابت بنية ImpTabPair * pImpTabPair;  /* pointer to levels state table pair */

    /* the overall paragraph or line directionality - see UBiDiDirection */
    UBiDiDirection direction;

    /* flags is a bit set for which directional properties are in the text */
    Flags flags;

    /* lastArabicPos is index to the last AL in the text, -1 if none */
    int32_t آخرین_Arabic_Pos;

    /* characters after trailingWSStart are WS and are */
    /* implicitly at the paraLevel (rule (L1)) - levels may not reflect that */
    int32_t trailingWSStart;

    /* fields for paragraph handling */
    int32_t para_شمار;                  /* set in getDirProps() */
    /* filled in getDirProps() */
    Para *paras;

    /* for relatively short text, we only need a tiny array of paras (no malloc()) */
    Para simpleParas[SIMPLE_PARAS_شمار];

    /* fields for line reordering */
    int32_t پیاپی_شمار;     /* ==-1: runs not set up yet */
    پیاپی *runs;

    /* for non-mixed text, we only need a tiny array of runs (no malloc()) */
    پیاپی simpleRuns[1];

    /* maximum or current nesting depth of isolate sequences */
    /* Within resolveExplicitLevels() and checkExplicitLevels(), this is the maximal
       nesting encountered.
       Within resolveImplicitLevels(), this is the index of the current isolates
       stack entry. */
    int32_t isolate_شمار;
    Isolate *isolates;

    /* for simple text, have a small stack (no malloc()) */
    Isolate simpleIsolates[SIMPLE_ISOLATES_شمار];

    /* for inverse Bidi with insertion of directional marks */
    درج_Points_2 درج_Points;

    /* for option UBIDI_OPTION_REMOVE_CONTROLS */
    int32_t control_شمار;

    /* for Bidi class callback */
    UBiDiClassCallback *fnClassCallback;    /* action pointer */
    ثابت فراغ *coClassCallback;            /* context pointer */
};

#تعریف_کن IS_VALID_PARA(x) ((x) && ((x)->pParaBiDi==(x)))
#تعریف_کن IS_VALID_PARA_OR_سطر(x) ((x) && ((x)->pParaBiDi==(x) || (((x)->pParaBiDi) && (x)->pParaBiDi->pParaBiDi==(x)->pParaBiDi)))

تعريفنوع اتحاد {
    DirProp *dir_Props_حافظه;
    UBiDiLevel *سطوح_حافظه;
    Opening *openings_حافظه;
    Para *paras_حافظه;
    پیاپی *runs_حافظه;
    Isolate *isolates_حافظه;
} Bidi_حافظه_For_تخصیص;

/* Macros for initial checks at function entry */
#تعریف_کن RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrcode, retvalue) UPRV_بلوک_MACRO_BEGIN { \
    إذا((pErrcode)==عدم || U_FAILURE(*pErrcode)) أرجع retvalue; \
} UPRV_بلوک_MACRO_انتها
#تعریف_کن RETURN_IF_NOT_VALID_PARA(bidi, errcode, retvalue) UPRV_بلوک_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلوک_MACRO_انتها
#تعریف_کن RETURN_IF_NOT_VALID_PARA_OR_سطر(bidi, errcode, retvalue) UPRV_بلوک_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA_OR_سطر(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلوک_MACRO_انتها
#تعریف_کن RETURN_IF_BAD_RANGE(arg, شروع, حد, errcode, retvalue) UPRV_بلوک_MACRO_BEGIN { \
    إذا((arg)<(شروع) || (arg)>=(حد)) { \
        (errcode)=U_ILLEGAL_آرگومان_ERROR; \
        أرجع retvalue; \
    } \
} UPRV_بلوک_MACRO_انتها

#تعریف_کن RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrcode) UPRV_بلوک_MACRO_BEGIN { \
    إذا((pErrcode)==عدم || U_FAILURE(*pErrcode)) أرجع; \
} UPRV_بلوک_MACRO_انتها
#تعریف_کن RETURN_VOID_IF_NOT_VALID_PARA(bidi, errcode) UPRV_بلوک_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع; \
    } \
} UPRV_بلوک_MACRO_انتها
#تعریف_کن RETURN_VOID_IF_NOT_VALID_PARA_OR_سطر(bidi, errcode) UPRV_بلوک_MACRO_BEGIN { \
    إذا(!IS_VALID_PARA_OR_سطر(bidi)) { \
        errcode=U_INVALID_STATE_ERROR; \
        أرجع; \
    } \
} UPRV_بلوک_MACRO_انتها
#تعریف_کن RETURN_VOID_IF_BAD_RANGE(arg, شروع, حد, errcode) UPRV_بلوک_MACRO_BEGIN { \
    إذا((arg)<(شروع) || (arg)>=(حد)) { \
        (errcode)=U_ILLEGAL_آرگومان_ERROR; \
        أرجع; \
    } \
} UPRV_بلوک_MACRO_انتها

/* helper function to (re)allocate memory if allowed */
U_CFUNC UBool
ubidi_get_حافظه(Bidi_حافظه_For_تخصیص *p_حافظه, int32_t *p_اندازه, UBool mayAllocate, size_t اندازه_Needed);

/* helper macros for each allocated array in UBiDi */
#تعریف_کن get_Dir_Props_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->dir_Props_حافظه, &(pBiDi)->dir_Props_اندازه, \
                        (pBiDi)->may_Allocate_متن, (درازی))

#تعریف_کن get_سطوح_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->سطوح_حافظه, &(pBiDi)->سطوح_اندازه, \
                        (pBiDi)->may_Allocate_متن, (درازی))

#تعریف_کن get_Runs_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->runs_حافظه, &(pBiDi)->runs_اندازه, \
                        (pBiDi)->mayAllocateRuns, (درازی)*حجم(پیاپی))

/* additional macros used by ubidi_open() - always allow allocation */
#تعریف_کن get_Initial_Dir_Props_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->dir_Props_حافظه, &(pBiDi)->dir_Props_اندازه, \
                        true, (درازی))

#تعریف_کن get_Initial_سطوح_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->سطوح_حافظه, &(pBiDi)->سطوح_اندازه, \
                        true, (درازی))

#تعریف_کن get_Initial_Openings_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->openings_حافظه, &(pBiDi)->openings_اندازه, \
                        true, (درازی)*حجم(Opening))

#تعریف_کن get_Initial_Paras_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->paras_حافظه, &(pBiDi)->paras_اندازه, \
                        true, (درازی)*حجم(Para))

#تعریف_کن get_Initial_Runs_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->runs_حافظه, &(pBiDi)->runs_اندازه, \
                        true, (درازی)*حجم(پیاپی))

#تعریف_کن get_Initial_Isolates_حافظه(pBiDi, درازی) \
        ubidi_get_حافظه((Bidi_حافظه_For_تخصیص *)&(pBiDi)->isolates_حافظه, &(pBiDi)->isolates_اندازه, \
                        true, (درازی)*حجم(Isolate))

#پایان_اگر
