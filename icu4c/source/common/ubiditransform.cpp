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

#شامل "cmemory.h"
#شامل "unicode/ubidi.h"
#شامل "unicode/ustring.h"
#شامل "unicode/ushape.h"
#شامل "unicode/utf16.h"
#شامل "ustr_imp.h"
#شامل "unicode/ubiditransform.h"

/* Some convenience defines */
#define LTR                     UBIDI_LTR
#define RTL                     UBIDI_RTL
#define LOGICAL                 UBIDI_LOGICAL
#define VISUAL                  UBIDI_VISUAL
#define SHAPE_LOGICAL           U_SHAPE_TEXT_DIRECTION_LOGICAL
#define SHAPE_VISUAL            U_SHAPE_TEXT_DIRECTION_VISUAL_LTR

#define CHECK_LEN(STR, LEN, ERROR) UPRV_BLOCK_MACRO_BEGIN { \
    اگر (LEN == 0) بازگشت 0; \
    اگر (LEN < -1) { *(ERROR) = U_ILLEGAL_ARGUMENT_ERROR; بازگشت 0; } \
    اگر (LEN == -1) LEN = u_strlen(STR); \
} UPRV_BLOCK_MACRO_END

#define MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
تعریفنوع UBool (*UBiDiAction)(UBiDiTransform *, UErrorCode *);

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
تعریفنوع ساختار {
    UBiDiLevel        inLevel;               /* input level */
    UBiDiOrder        inOrder;               /* input order */
    UBiDiLevel        outLevel;              /* output level */
    UBiDiOrder        outOrder;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        baseLevel;             /* paragraph level to be used with setPara */
    ثابت UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

ساختار UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ثابت ReorderingScheme  *pActiveScheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                srcLength;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                srcSize;            /* input text capacity excluding the trailing zero */
    uint32_t                destSize;           /* output text capacity */
    uint32_t                *pDestLength;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UErrorCode *pErrorCode)
{
    UBiDiTransform *pBiDiTransform = nullptr;
    اگر (U_SUCCESS(*pErrorCode)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, اندازه(UBiDiTransform));
        اگر (pBiDiTransform == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        }
    }
    بازگشت pBiDiTransform;
}

U_CAPI تهی U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    اگر (pBiDiTransform != nullptr) {
        اگر (pBiDiTransform->pBidi != nullptr) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        اگر (pBiDiTransform->src != nullptr) {
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
ایستا UBool
action_resolve(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_setPara(pTransform->pBidi, pTransform->src, pTransform->srcLength,
            pTransform->pActiveScheme->baseLevel, nullptr, pErrorCode);
    بازگشت نادرست;
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
ایستا UBool
action_reorder(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->destSize,
            static_cast<uint16_t>(pTransform->reorderingOptions), pErrorCode);

    *pTransform->pDestLength = pTransform->srcLength;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    بازگشت درست;
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
ایستا UBool
action_setInverse(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    (تهی)pErrorCode;
    ubidi_setInverse(pTransform->pBidi, درست);
    ubidi_setReorderingMode(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_DIRECT);
    بازگشت نادرست;
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
ایستا UBool
action_setRunsOnly(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    (تهی)pErrorCode;
    ubidi_setReorderingMode(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    بازگشت نادرست;
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
ایستا UBool
action_reverse(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_writeReverse(pTransform->src, pTransform->srcLength,
            pTransform->dest, pTransform->destSize,
            UBIDI_REORDER_DEFAULT, pErrorCode);
    *pTransform->pDestLength = pTransform->srcLength;
    بازگشت درست;
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
ایستا تهی
updateSrc(UBiDiTransform *pTransform, ثابت char16_t *newSrc, uint32_t newLength,
        uint32_t newSize, UErrorCode *pErrorCode)
{
    اگر (newSize < newLength) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        بازگشت;
    }
    اگر (newSize > pTransform->srcSize) {
        newSize += 50; // allocate slightly more than needed right now
        اگر (pTransform->src != nullptr) {
            uprv_free(pTransform->src);
            pTransform->src = nullptr;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(newSize * اندازه(char16_t)));
        اگر (pTransform->src == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            بازگشت;
        }
        pTransform->srcSize = newSize;
    }
    u_strncpy(pTransform->src, newSrc, newLength);
    pTransform->srcLength = u_terminateUChars(pTransform->src,
    		pTransform->srcSize, newLength, pErrorCode);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
ایستا تهی
doShape(UBiDiTransform *pTransform, uint32_t options, UErrorCode *pErrorCode)
{
    *pTransform->pDestLength = u_shapeArabic(pTransform->src,
            pTransform->srcLength, pTransform->dest, pTransform->destSize,
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
ایستا UBool
action_shapeArabic(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    اگر ((pTransform->letters | pTransform->digits) == 0) {
        بازگشت نادرست;
    }
    اگر (pTransform->pActiveScheme->lettersDir == pTransform->pActiveScheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->pActiveScheme->lettersDir,
                pErrorCode);
    } وگرنه {
        doShape(pTransform, pTransform->digits | pTransform->pActiveScheme->digitsDir, pErrorCode);
        اگر (U_SUCCESS(*pErrorCode)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->pDestLength,
                    *pTransform->pDestLength, pErrorCode);
            doShape(pTransform, pTransform->letters | pTransform->pActiveScheme->lettersDir,
                    pErrorCode);
        }
    }
    بازگشت درست;
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
ایستا UBool
action_mirror(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    اگر (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        بازگشت نادرست;
    }
    انجامبده {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_NEXT(pTransform->src, i, pTransform->srcLength, c); 
        اگر (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_LENGTH(c);
        اگر (j + k <= pTransform->destSize) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } وگرنه {
            j += k;
        }
    } وقتی (i < pTransform->srcLength);
    
    *pTransform->pDestLength = j;
    اگر (pTransform->destSize < j) {
        *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        بازگشت نادرست;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    بازگشت درست;
}

/**
 * All possible reordering schemes.
 *
 */
ایستا ثابت ReorderingScheme Schemes[] =
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

ایستا ثابت uint32_t nSchemes = اندازه(Schemes) / اندازه(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
ایستا تهی
resolveBaseDirection(ثابت char16_t *text, uint32_t length,
        UBiDiLevel *pInLevel, UBiDiLevel *pOutLevel)
{
    انتخاب (*pInLevel) {
        حالت UBIDI_DEFAULT_LTR:
        حالت UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_getBaseDirection(text, length));
            *pInLevel = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *pInLevel == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            بشکن;
        }
        پیشفرض:
            *pInLevel &= 1;
            بشکن;
    }
    انتخاب (*pOutLevel) {
        حالت UBIDI_DEFAULT_LTR:
        حالت UBIDI_DEFAULT_RTL:
            *pOutLevel = *pInLevel;
            بشکن;
        پیشفرض:
            *pOutLevel &= 1;
            بشکن;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
ایستا ثابت ReorderingScheme*
findMatchingScheme(UBiDiLevel inLevel, UBiDiLevel outLevel,
        UBiDiOrder inOrder, UBiDiOrder outOrder)
{
    uint32_t i;
    برای (i = 0; i < nSchemes; i++) {
        ثابت ReorderingScheme *pScheme = Schemes + i;
        اگر (inLevel == pScheme->inLevel && outLevel == pScheme->outLevel
                && inOrder == pScheme->inOrder && outOrder == pScheme->outOrder) {
            بازگشت pScheme;
        }
    }
    بازگشت nullptr;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ثابت char16_t *src, int32_t srcLength,
            char16_t *dest, int32_t destSize,
            UBiDiLevel inParaLevel, UBiDiOrder inOrder,
            UBiDiLevel outParaLevel, UBiDiOrder outOrder,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UErrorCode *pErrorCode)
{
    uint32_t destLength = 0;
    UBool textChanged = نادرست;
    ثابت UBiDiTransform *pOrigTransform = pBiDiTransform;
    ثابت UBiDiAction *action = nullptr;

    اگر (U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }
    اگر (src == nullptr || dest == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    CHECK_LEN(src, srcLength, pErrorCode);
    CHECK_LEN(dest, destSize, pErrorCode);

    اگر (pBiDiTransform == nullptr) {
        pBiDiTransform = ubiditransform_open(pErrorCode);
        اگر (U_FAILURE(*pErrorCode)) {
            بازگشت 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolveBaseDirection(src, srcLength, &inParaLevel, &outParaLevel);

    pBiDiTransform->pActiveScheme = findMatchingScheme(inParaLevel, outParaLevel,
            inOrder, outOrder);
    اگر (pBiDiTransform->pActiveScheme == nullptr) {
        برو cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_TEXT_DIRECTION_MASK;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_MASK;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_MASK;

    updateSrc(pBiDiTransform, src, srcLength, destSize > srcLength ? destSize : srcLength, pErrorCode);
    اگر (U_FAILURE(*pErrorCode)) {
        برو cleanup;
    }
    اگر (pBiDiTransform->pBidi == nullptr) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, pErrorCode);
        اگر (U_FAILURE(*pErrorCode)) {
            برو cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->destSize = destSize;
    pBiDiTransform->pDestLength = &destLength;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    برای (action = pBiDiTransform->pActiveScheme->actions; *action && U_SUCCESS(*pErrorCode); action++) {
        اگر ((*action)(pBiDiTransform, pErrorCode)) {
            اگر (action[1] != nullptr) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->pDestLength,
                        *pBiDiTransform->pDestLength, pErrorCode);
            }
            textChanged = درست;
        }
    }
    ubidi_setInverse(pBiDiTransform->pBidi, نادرست);

    اگر (!textChanged && U_SUCCESS(*pErrorCode)) {
        /* Text was not changed - just copy src to dest */
        اگر (destSize < srcLength) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
        } وگرنه {
            u_strncpy(dest, src, srcLength);
            destLength = srcLength;
        }
    }
cleanup:
    اگر (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } وگرنه {
        pBiDiTransform->dest = nullptr;
        pBiDiTransform->pDestLength = nullptr;
        pBiDiTransform->srcLength = 0;
        pBiDiTransform->destSize = 0;
    }
    بازگشت (U_FAILURE(*pErrorCode) && *pErrorCode != U_BUFFER_OVERFLOW_ERROR) ? 0 : destLength;
}
