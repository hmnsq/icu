/*
******************************************************************************
*
* © 2016 and later: Unicode, Inc. and others.
* License & terms of use: http://www.unicode.org/copyright.html
*
******************************************************************************
*   file name:  ubiditransform.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2016jul24
*   created by: Lina Kemmel
*
*/

#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/ubidi.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/ushape.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "unicode/ubiditransform.h"

/* Some convenience defines */
#define LTR                     UBIDI_LTR
#define RTL                     UBIDI_RTL
#define LOGICAL                 UBIDI_LOGICAL
#define VISUAL                  UBIDI_VISUAL
#define SHAPE_LOGICAL           U_SHAPE_TEXT_DIRECTION_LOGICAL
#define SHAPE_VISUAL            U_SHAPE_TEXT_DIRECTION_VISUAL_LTR

#define CHECK_LEN(STR, LEN, ERROR) UPRV_BLOCK_MACRO_BEGIN { \
    যদি (LEN == 0) ফেরত 0; \
    যদি (LEN < -1) { *(ERROR) = U_ILLEGAL_ARGUMENT_ERROR; ফেরত 0; } \
    যদি (LEN == -1) LEN = u_strlen(STR); \
} UPRV_BLOCK_MACRO_END

#define MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
প্রকারলেখ UBool (*UBiDiAction)(UBiDiTransform *, UErrorCode *);

/**
 * Structure that holds a predefined reordering scheme, including the following
 * information:
 * <ul>
 * <li>an input base direction,</li>
 * <li>an input order,</li>
 * <li>an output base direction,</li>
 * <li>an output order,</li>
 * <li>a digit shaping direction,</li>
 * <li>a letter shaping direction,</li>
 * <li>a base direction that should be applied when the reordering engine is
 *     invoked (which can not always be derived from the caller-defined
 *     options),</li>
 * <li>an array of pointers to functions that accomplish the bidi layout
 *     transformation.</li>
 * </ul>
 */
প্রকারলেখ কাঠামো {
    UBiDiLevel        inLevel;               /* input level */
    UBiDiOrder        inOrder;               /* input order */
    UBiDiLevel        outLevel;              /* output level */
    UBiDiOrder        outOrder;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        baseLevel;             /* paragraph level to be used with setPara */
    ধ্রুবক UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

কাঠামো UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ধ্রুবক ReorderingScheme  *pActiveScheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_দৈর্ঘ্য;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_আকার;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_আকার;           /* output text capacity */
    uint32_t                *p_Dest_দৈর্ঘ্য;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UErrorCode *pErrorCode)
{
    UBiDiTransform *pBiDiTransform = nullptr;
    যদি (U_SUCCESS(*pErrorCode)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, আকার(UBiDiTransform));
        যদি (pBiDiTransform == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        }
    }
    ফেরত pBiDiTransform;
}

U_CAPI শূন্য U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    যদি (pBiDiTransform != nullptr) {
        যদি (pBiDiTransform->pBidi != nullptr) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        যদি (pBiDiTransform->src != nullptr) {
            uprv_free(pBiDiTransform->src);
        }
        uprv_free(pBiDiTransform);
    }
}

/**
 * Performs Bidi resolution of text.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
স্থির UBool
action_resolve(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_setPara(pTransform->pBidi, pTransform->src, pTransform->src_দৈর্ঘ্য,
            pTransform->pActiveScheme->baseLevel, nullptr, pErrorCode);
    ফেরত মিথ্যা;
}

/**
 * Performs basic reordering of text (Logical -> Visual LTR).
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
স্থির UBool
action_reorder(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_আকার,
            static_cast<uint16_t>(pTransform->reorderingOptions), pErrorCode);

    *pTransform->p_Dest_দৈর্ঘ্য = pTransform->src_দৈর্ঘ্য;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    ফেরত সত্য;
}

/**
 * Sets "inverse" mode on the <code>UBiDi</code> object.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
স্থির UBool
action_setInverse(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    (শূন্য)pErrorCode;
    ubidi_setInverse(pTransform->pBidi, সত্য);
    ubidi_setReorderingMode(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_DIRECT);
    ফেরত মিথ্যা;
}

/**
 * Sets "runs only" reordering mode indicating a Logical LTR <-> Logical RTL
 * transformation.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
স্থির UBool
action_setRunsOnly(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    (শূন্য)pErrorCode;
    ubidi_setReorderingMode(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    ফেরত মিথ্যা;
}

/**
 * Performs string reverse.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
স্থির UBool
action_reverse(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_writeReverse(pTransform->src, pTransform->src_দৈর্ঘ্য,
            pTransform->dest, pTransform->dest_আকার,
            UBIDI_REORDER_DEFAULT, pErrorCode);
    *pTransform->p_Dest_দৈর্ঘ্য = pTransform->src_দৈর্ঘ্য;
    ফেরত সত্য;
}

/**
 * Applies a new value to the text that serves as input at the current
 * processing step. This value is identical to the original one when we begin
 * the processing, but usually changes as the transformation progresses.
 * 
 * @param pTransform A pointer to the <code>UBiDiTransform</code> structure.
 * @param newSrc A pointer whose value is to be used as input text.
 * @param newLength A length of the new text in <code>char16_t</code>s.
 * @param newSize A new source capacity in <code>char16_t</code>s.
 * @param pErrorCode Pointer to the error code value.
 */
স্থির শূন্য
updateSrc(UBiDiTransform *pTransform, ধ্রুবক char16_t *newSrc, uint32_t new_দৈর্ঘ্য,
        uint32_t new_আকার, UErrorCode *pErrorCode)
{
    যদি (new_আকার < new_দৈর্ঘ্য) {
        *pErrorCode = U_বাফার_OVERFLOW_ERROR;
        ফেরত;
    }
    যদি (new_আকার > pTransform->src_আকার) {
        new_আকার += 50; // allocate slightly more than needed right now
        যদি (pTransform->src != nullptr) {
            uprv_free(pTransform->src);
            pTransform->src = nullptr;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_আকার * আকার(char16_t)));
        যদি (pTransform->src == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            ফেরত;
        }
        pTransform->src_আকার = new_আকার;
    }
    u_strncpy(pTransform->src, newSrc, new_দৈর্ঘ্য);
    pTransform->src_দৈর্ঘ্য = u_terminateUChars(pTransform->src,
    		pTransform->src_আকার, new_দৈর্ঘ্য, pErrorCode);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
স্থির শূন্য
doShape(UBiDiTransform *pTransform, uint32_t options, UErrorCode *pErrorCode)
{
    *pTransform->p_Dest_দৈর্ঘ্য = u_shapeArabic(pTransform->src,
            pTransform->src_দৈর্ঘ্য, pTransform->dest, pTransform->dest_আকার,
            options, pErrorCode);
}

/**
 * Performs digit and letter shaping.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
স্থির UBool
action_shapeArabic(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    যদি ((pTransform->letters | pTransform->digits) == 0) {
        ফেরত মিথ্যা;
    }
    যদি (pTransform->pActiveScheme->lettersDir == pTransform->pActiveScheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->pActiveScheme->lettersDir,
                pErrorCode);
    } অন্যথা {
        doShape(pTransform, pTransform->digits | pTransform->pActiveScheme->digitsDir, pErrorCode);
        যদি (U_SUCCESS(*pErrorCode)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_দৈর্ঘ্য,
                    *pTransform->p_Dest_দৈর্ঘ্য, pErrorCode);
            doShape(pTransform, pTransform->letters | pTransform->pActiveScheme->lettersDir,
                    pErrorCode);
        }
    }
    ফেরত সত্য;
}

/**
 * Performs character mirroring.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
স্থির UBool
action_mirror(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    যদি (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        ফেরত মিথ্যা;
    }
    করো {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_পরবর্তী(pTransform->src, i, pTransform->src_দৈর্ঘ্য, c); 
        যদি (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_দৈর্ঘ্য(c);
        যদি (j + k <= pTransform->dest_আকার) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } অন্যথা {
            j += k;
        }
    } যখন (i < pTransform->src_দৈর্ঘ্য);
    
    *pTransform->p_Dest_দৈর্ঘ্য = j;
    যদি (pTransform->dest_আকার < j) {
        *pErrorCode = U_বাফার_OVERFLOW_ERROR;
        ফেরত মিথ্যা;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    ফেরত সত্য;
}

/**
 * All possible reordering schemes.
 *
 */
স্থির ধ্রুবক ReorderingScheme Schemes[] =
{
    /* 0: Logical LTR => Visual LTR */
    {LTR, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, nullptr}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, nullptr}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_reverse, nullptr}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_reverse, nullptr}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_setInverse, action_resolve, action_reorder, nullptr}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_reverse, action_shapeArabic, action_setInverse, action_resolve, action_reorder, nullptr}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_setInverse, action_resolve, action_reorder, action_shapeArabic, nullptr}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_reverse, action_setInverse, action_resolve, action_reorder, action_shapeArabic, nullptr}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_setRunsOnly, action_resolve, action_reorder, nullptr}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_setRunsOnly, action_resolve, action_reorder, action_shapeArabic, nullptr}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_setInverse, action_resolve, action_mirror, action_reverse, nullptr}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_reverse, action_shapeArabic, action_setInverse, action_resolve, action_mirror, nullptr}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, nullptr}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, nullptr}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, nullptr}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_reverse, action_resolve, action_mirror, action_shapeArabic, action_reverse, nullptr}}
};

স্থির ধ্রুবক uint32_t nSchemes = আকার(Schemes) / আকার(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
স্থির শূন্য
resolveBaseDirection(ধ্রুবক char16_t *text, uint32_t length,
        UBiDiLevel *pInLevel, UBiDiLevel *pOutLevel)
{
    নির্বাচন (*pInLevel) {
        ক্ষেত্র UBIDI_DEFAULT_LTR:
        ক্ষেত্র UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_getBaseDirection(text, length));
            *pInLevel = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *pInLevel == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            বিরতি;
        }
        পূর্বনির্ধারিত:
            *pInLevel &= 1;
            বিরতি;
    }
    নির্বাচন (*pOutLevel) {
        ক্ষেত্র UBIDI_DEFAULT_LTR:
        ক্ষেত্র UBIDI_DEFAULT_RTL:
            *pOutLevel = *pInLevel;
            বিরতি;
        পূর্বনির্ধারিত:
            *pOutLevel &= 1;
            বিরতি;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
স্থির ধ্রুবক ReorderingScheme*
findMatchingScheme(UBiDiLevel inLevel, UBiDiLevel outLevel,
        UBiDiOrder inOrder, UBiDiOrder outOrder)
{
    uint32_t i;
    জন্য (i = 0; i < nSchemes; i++) {
        ধ্রুবক ReorderingScheme *pScheme = Schemes + i;
        যদি (inLevel == pScheme->inLevel && outLevel == pScheme->outLevel
                && inOrder == pScheme->inOrder && outOrder == pScheme->outOrder) {
            ফেরত pScheme;
        }
    }
    ফেরত nullptr;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
            char16_t *dest, int32_t dest_আকার,
            UBiDiLevel inParaLevel, UBiDiOrder inOrder,
            UBiDiLevel outParaLevel, UBiDiOrder outOrder,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UErrorCode *pErrorCode)
{
    uint32_t dest_দৈর্ঘ্য = 0;
    UBool textChanged = মিথ্যা;
    ধ্রুবক UBiDiTransform *pOrigTransform = pBiDiTransform;
    ধ্রুবক UBiDiAction *action = nullptr;

    যদি (U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি (src == nullptr || dest == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    CHECK_LEN(src, src_দৈর্ঘ্য, pErrorCode);
    CHECK_LEN(dest, dest_আকার, pErrorCode);

    যদি (pBiDiTransform == nullptr) {
        pBiDiTransform = ubiditransform_open(pErrorCode);
        যদি (U_FAILURE(*pErrorCode)) {
            ফেরত 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolveBaseDirection(src, src_দৈর্ঘ্য, &inParaLevel, &outParaLevel);

    pBiDiTransform->pActiveScheme = findMatchingScheme(inParaLevel, outParaLevel,
            inOrder, outOrder);
    যদি (pBiDiTransform->pActiveScheme == nullptr) {
        যাও cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_TEXT_DIRECTION_MASK;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_MASK;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_MASK;

    updateSrc(pBiDiTransform, src, src_দৈর্ঘ্য, dest_আকার > src_দৈর্ঘ্য ? dest_আকার : src_দৈর্ঘ্য, pErrorCode);
    যদি (U_FAILURE(*pErrorCode)) {
        যাও cleanup;
    }
    যদি (pBiDiTransform->pBidi == nullptr) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, pErrorCode);
        যদি (U_FAILURE(*pErrorCode)) {
            যাও cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_আকার = dest_আকার;
    pBiDiTransform->p_Dest_দৈর্ঘ্য = &dest_দৈর্ঘ্য;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    জন্য (action = pBiDiTransform->pActiveScheme->actions; *action && U_SUCCESS(*pErrorCode); action++) {
        যদি ((*action)(pBiDiTransform, pErrorCode)) {
            যদি (action[1] != nullptr) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_দৈর্ঘ্য,
                        *pBiDiTransform->p_Dest_দৈর্ঘ্য, pErrorCode);
            }
            textChanged = সত্য;
        }
    }
    ubidi_setInverse(pBiDiTransform->pBidi, মিথ্যা);

    যদি (!textChanged && U_SUCCESS(*pErrorCode)) {
        /* Text was not changed - just copy src to dest */
        যদি (dest_আকার < src_দৈর্ঘ্য) {
            *pErrorCode = U_বাফার_OVERFLOW_ERROR;
        } অন্যথা {
            u_strncpy(dest, src, src_দৈর্ঘ্য);
            dest_দৈর্ঘ্য = src_দৈর্ঘ্য;
        }
    }
cleanup:
    যদি (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } অন্যথা {
        pBiDiTransform->dest = nullptr;
        pBiDiTransform->p_Dest_দৈর্ঘ্য = nullptr;
        pBiDiTransform->src_দৈর্ঘ্য = 0;
        pBiDiTransform->dest_আকার = 0;
    }
    ফেরত (U_FAILURE(*pErrorCode) && *pErrorCode != U_বাফার_OVERFLOW_ERROR) ? 0 : dest_দৈর্ঘ্য;
}
