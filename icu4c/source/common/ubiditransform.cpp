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

#समावेश "cmemory.h"
#समावेश "unicode/ubidi.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/ushape.h"
#समावेश "unicode/utf16.h"
#समावेश "ustr_imp.h"
#समावेश "unicode/ubiditransform.h"

/* Some convenience defines */
#घोषणा LTR                     UBIDI_LTR
#घोषणा RTL                     UBIDI_RTL
#घोषणा LOGICAL                 UBIDI_LOGICAL
#घोषणा VISUAL                  UBIDI_VISUAL
#घोषणा SHAPE_LOGICAL           U_SHAPE_TEXT_DIRECTION_LOGICAL
#घोषणा SHAPE_VISUAL            U_SHAPE_TEXT_DIRECTION_VISUAL_LTR

#घोषणा CHECK_LEN(STR, LEN, ERROR) UPRV_BLOCK_MACRO_BEGIN { \
    अगर (LEN == 0) वापस 0; \
    अगर (LEN < -1) { *(ERROR) = U_ILLEGAL_ARGUMENT_ERROR; वापस 0; } \
    अगर (LEN == -1) LEN = u_strlen(STR); \
} UPRV_BLOCK_MACRO_END

#घोषणा MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
प्रकार UBool (*UBiDiAction)(UBiDiTransform *, UErrorCode *);

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
प्रकार काष्ठा {
    UBiDiLevel        inLevel;               /* input level */
    UBiDiOrder        inOrder;               /* input order */
    UBiDiLevel        outLevel;              /* output level */
    UBiDiOrder        outOrder;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        baseLevel;             /* paragraph level to be used with setPara */
    स्थिर UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

काष्ठा UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    स्थिर ReorderingScheme  *pActiveScheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_लंबाई;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_आकार;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_आकार;           /* output text capacity */
    uint32_t                *p_Dest_लंबाई;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UErrorCode *pErrorCode)
{
    UBiDiTransform *pBiDiTransform = nullptr;
    अगर (U_SUCCESS(*pErrorCode)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, माप(UBiDiTransform));
        अगर (pBiDiTransform == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        }
    }
    वापस pBiDiTransform;
}

U_CAPI व्योम U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    अगर (pBiDiTransform != nullptr) {
        अगर (pBiDiTransform->pBidi != nullptr) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        अगर (pBiDiTransform->src != nullptr) {
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
अटल UBool
action_resolve(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_setPara(pTransform->pBidi, pTransform->src, pTransform->src_लंबाई,
            pTransform->pActiveScheme->baseLevel, nullptr, pErrorCode);
    वापस false;
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
अटल UBool
action_reorder(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_आकार,
            static_cast<uint16_t>(pTransform->reorderingOptions), pErrorCode);

    *pTransform->p_Dest_लंबाई = pTransform->src_लंबाई;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    वापस true;
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
अटल UBool
action_setInverse(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    (व्योम)pErrorCode;
    ubidi_setInverse(pTransform->pBidi, true);
    ubidi_setReorderingMode(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_DIRECT);
    वापस false;
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
अटल UBool
action_setRunsOnly(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    (व्योम)pErrorCode;
    ubidi_setReorderingMode(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    वापस false;
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
अटल UBool
action_reverse(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    ubidi_writeReverse(pTransform->src, pTransform->src_लंबाई,
            pTransform->dest, pTransform->dest_आकार,
            UBIDI_REORDER_DEFAULT, pErrorCode);
    *pTransform->p_Dest_लंबाई = pTransform->src_लंबाई;
    वापस true;
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
अटल व्योम
updateSrc(UBiDiTransform *pTransform, स्थिर char16_t *newSrc, uint32_t new_लंबाई,
        uint32_t new_आकार, UErrorCode *pErrorCode)
{
    अगर (new_आकार < new_लंबाई) {
        *pErrorCode = U_बफर_OVERFLOW_ERROR;
        वापस;
    }
    अगर (new_आकार > pTransform->src_आकार) {
        new_आकार += 50; // allocate slightly more than needed right now
        अगर (pTransform->src != nullptr) {
            uprv_free(pTransform->src);
            pTransform->src = nullptr;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_आकार * माप(char16_t)));
        अगर (pTransform->src == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            वापस;
        }
        pTransform->src_आकार = new_आकार;
    }
    u_strncpy(pTransform->src, newSrc, new_लंबाई);
    pTransform->src_लंबाई = u_terminateUChars(pTransform->src,
    		pTransform->src_आकार, new_लंबाई, pErrorCode);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
अटल व्योम
doShape(UBiDiTransform *pTransform, uint32_t options, UErrorCode *pErrorCode)
{
    *pTransform->p_Dest_लंबाई = u_shapeArabic(pTransform->src,
            pTransform->src_लंबाई, pTransform->dest, pTransform->dest_आकार,
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
अटल UBool
action_shapeArabic(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    अगर ((pTransform->letters | pTransform->digits) == 0) {
        वापस false;
    }
    अगर (pTransform->pActiveScheme->lettersDir == pTransform->pActiveScheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->pActiveScheme->lettersDir,
                pErrorCode);
    } अन्यथा {
        doShape(pTransform, pTransform->digits | pTransform->pActiveScheme->digitsDir, pErrorCode);
        अगर (U_SUCCESS(*pErrorCode)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_लंबाई,
                    *pTransform->p_Dest_लंबाई, pErrorCode);
            doShape(pTransform, pTransform->letters | pTransform->pActiveScheme->lettersDir,
                    pErrorCode);
        }
    }
    वापस true;
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
अटल UBool
action_mirror(UBiDiTransform *pTransform, UErrorCode *pErrorCode)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    अगर (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        वापस false;
    }
    करो {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_अग्र(pTransform->src, i, pTransform->src_लंबाई, c); 
        अगर (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_लंबाई(c);
        अगर (j + k <= pTransform->dest_आकार) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } अन्यथा {
            j += k;
        }
    } जबतक (i < pTransform->src_लंबाई);
    
    *pTransform->p_Dest_लंबाई = j;
    अगर (pTransform->dest_आकार < j) {
        *pErrorCode = U_बफर_OVERFLOW_ERROR;
        वापस false;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    वापस true;
}

/**
 * All possible reordering schemes.
 *
 */
अटल स्थिर ReorderingScheme Schemes[] =
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

अटल स्थिर uint32_t nSchemes = माप(Schemes) / माप(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
अटल व्योम
resolveBaseDirection(स्थिर char16_t *text, uint32_t length,
        UBiDiLevel *pInLevel, UBiDiLevel *pOutLevel)
{
    चयन (*pInLevel) {
        हाल UBIDI_DEFAULT_LTR:
        हाल UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_getBaseDirection(text, length));
            *pInLevel = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *pInLevel == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            अवरोध;
        }
        शेष:
            *pInLevel &= 1;
            अवरोध;
    }
    चयन (*pOutLevel) {
        हाल UBIDI_DEFAULT_LTR:
        हाल UBIDI_DEFAULT_RTL:
            *pOutLevel = *pInLevel;
            अवरोध;
        शेष:
            *pOutLevel &= 1;
            अवरोध;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
अटल स्थिर ReorderingScheme*
findMatchingScheme(UBiDiLevel inLevel, UBiDiLevel outLevel,
        UBiDiOrder inOrder, UBiDiOrder outOrder)
{
    uint32_t i;
    क्रम (i = 0; i < nSchemes; i++) {
        स्थिर ReorderingScheme *pScheme = Schemes + i;
        अगर (inLevel == pScheme->inLevel && outLevel == pScheme->outLevel
                && inOrder == pScheme->inOrder && outOrder == pScheme->outOrder) {
            वापस pScheme;
        }
    }
    वापस nullptr;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            स्थिर char16_t *src, int32_t src_लंबाई,
            char16_t *dest, int32_t dest_आकार,
            UBiDiLevel inParaLevel, UBiDiOrder inOrder,
            UBiDiLevel outParaLevel, UBiDiOrder outOrder,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UErrorCode *pErrorCode)
{
    uint32_t dest_लंबाई = 0;
    UBool textChanged = false;
    स्थिर UBiDiTransform *pOrigTransform = pBiDiTransform;
    स्थिर UBiDiAction *action = nullptr;

    अगर (U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर (src == nullptr || dest == nullptr) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    CHECK_LEN(src, src_लंबाई, pErrorCode);
    CHECK_LEN(dest, dest_आकार, pErrorCode);

    अगर (pBiDiTransform == nullptr) {
        pBiDiTransform = ubiditransform_open(pErrorCode);
        अगर (U_FAILURE(*pErrorCode)) {
            वापस 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolveBaseDirection(src, src_लंबाई, &inParaLevel, &outParaLevel);

    pBiDiTransform->pActiveScheme = findMatchingScheme(inParaLevel, outParaLevel,
            inOrder, outOrder);
    अगर (pBiDiTransform->pActiveScheme == nullptr) {
        जाओ cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_TEXT_DIRECTION_MASK;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_MASK;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_MASK;

    updateSrc(pBiDiTransform, src, src_लंबाई, dest_आकार > src_लंबाई ? dest_आकार : src_लंबाई, pErrorCode);
    अगर (U_FAILURE(*pErrorCode)) {
        जाओ cleanup;
    }
    अगर (pBiDiTransform->pBidi == nullptr) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, pErrorCode);
        अगर (U_FAILURE(*pErrorCode)) {
            जाओ cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_आकार = dest_आकार;
    pBiDiTransform->p_Dest_लंबाई = &dest_लंबाई;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    क्रम (action = pBiDiTransform->pActiveScheme->actions; *action && U_SUCCESS(*pErrorCode); action++) {
        अगर ((*action)(pBiDiTransform, pErrorCode)) {
            अगर (action[1] != nullptr) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_लंबाई,
                        *pBiDiTransform->p_Dest_लंबाई, pErrorCode);
            }
            textChanged = true;
        }
    }
    ubidi_setInverse(pBiDiTransform->pBidi, false);

    अगर (!textChanged && U_SUCCESS(*pErrorCode)) {
        /* Text was not changed - just copy src to dest */
        अगर (dest_आकार < src_लंबाई) {
            *pErrorCode = U_बफर_OVERFLOW_ERROR;
        } अन्यथा {
            u_strncpy(dest, src, src_लंबाई);
            dest_लंबाई = src_लंबाई;
        }
    }
cleanup:
    अगर (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } अन्यथा {
        pBiDiTransform->dest = nullptr;
        pBiDiTransform->p_Dest_लंबाई = nullptr;
        pBiDiTransform->src_लंबाई = 0;
        pBiDiTransform->dest_आकार = 0;
    }
    वापस (U_FAILURE(*pErrorCode) && *pErrorCode != U_बफर_OVERFLOW_ERROR) ? 0 : dest_लंबाई;
}
