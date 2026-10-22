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

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubidi.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ushape.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "unicode/ubiditransform.h"

/* Some convenience defines */
#ವ್ಯಾಖ್ಯಾನಿಸು LTR                     UBIDI_LTR
#ವ್ಯಾಖ್ಯಾನಿಸು RTL                     UBIDI_RTL
#ವ್ಯಾಖ್ಯಾನಿಸು LOGICAL                 UBIDI_LOGICAL
#ವ್ಯಾಖ್ಯಾನಿಸು VISUAL                  UBIDI_VISUAL
#ವ್ಯಾಖ್ಯಾನಿಸು SHAPE_LOGICAL           U_SHAPE_ಪಠ್ಯ_DIRECTION_LOGICAL
#ವ್ಯಾಖ್ಯಾನಿಸು SHAPE_VISUAL            U_SHAPE_ಪಠ್ಯ_DIRECTION_VISUAL_LTR

#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_LEN(STR, LEN, ERROR) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ (LEN == 0) ಹಿಂತಿರುಗಿ 0; \
    ಆದರೆ (LEN < -1) { *(ERROR) = U_ILLEGAL_ವಾದ_ERROR; ಹಿಂತಿರುಗಿ 0; } \
    ಆದರೆ (LEN == -1) LEN = u_strlen(STR); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ UBool (*UBiDiAction)(UBiDiTransform *, UError_ಸಂಕೇತ *);

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    UBiDiLevel        ಒಳ_Level;               /* input level */
    UBi_Di_ಕ್ರಮ        ಒಳ_ಕ್ರಮ;               /* input order */
    UBiDiLevel        ಹೊರಹೋಗು_Level;              /* output level */
    UBi_Di_ಕ್ರಮ        ಹೊರಹೋಗು_ಕ್ರಮ;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        ತಳ_Level;             /* paragraph level to be used with setPara */
    ಸ್ಥಿರಾಂಕ UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

ರಚನೆ UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ಸ್ಥಿರಾಂಕ ReorderingScheme  *p_ಸಕ್ರಿಯ_Scheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_ಉದ್ದಳತೆ;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_ಗಾತ್ರ;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_ಗಾತ್ರ;           /* output text capacity */
    uint32_t                *p_Dest_ಉದ್ದಳತೆ;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    UBiDiTransform *pBiDiTransform = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, ಗಾತ್ರ(UBiDiTransform));
        ಆದರೆ (pBiDiTransform == ಶೂನ್ಯಸೂಚಕ) {
            *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        }
    }
    ಹಿಂತಿರುಗಿ pBiDiTransform;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    ಆದರೆ (pBiDiTransform != ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (pBiDiTransform->pBidi != ಶೂನ್ಯಸೂಚಕ) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        ಆದರೆ (pBiDiTransform->src != ಶೂನ್ಯಸೂಚಕ) {
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
ಸ್ಥಿರ UBool
action_resolve(UBiDiTransform *pTransform, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ubidi_ಗಣ_Para(pTransform->pBidi, pTransform->src, pTransform->src_ಉದ್ದಳತೆ,
            pTransform->p_ಸಕ್ರಿಯ_Scheme->ತಳ_Level, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
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
ಸ್ಥಿರ UBool
action_reorder(UBiDiTransform *pTransform, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_ಗಾತ್ರ,
            static_cast<uint16_t>(pTransform->reorderingOptions), p_Error_ಸಂಕೇತ);

    *pTransform->p_Dest_ಉದ್ದಳತೆ = pTransform->src_ಉದ್ದಳತೆ;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    ಹಿಂತಿರುಗಿ ನಿಜ;
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
ಸ್ಥಿರ UBool
action_ಗಣ_Inverse(UBiDiTransform *pTransform, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    ubidi_ಗಣ_Inverse(pTransform->pBidi, ನಿಜ);
    ubidi_ಗಣ_Reordering_ಬಹುಲಕ(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_ನೇರ);
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
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
ಸ್ಥಿರ UBool
action_ಗಣ_Runs_Only(UBiDiTransform *pTransform, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    (ಶೂನ್ಯ)p_Error_ಸಂಕೇತ;
    ubidi_ಗಣ_Reordering_ಬಹುಲಕ(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
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
ಸ್ಥಿರ UBool
action_ಹಿಮ್ಮುಖ(UBiDiTransform *pTransform, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ubidi_write_ಹಿಮ್ಮುಖ(pTransform->src, pTransform->src_ಉದ್ದಳತೆ,
            pTransform->dest, pTransform->dest_ಗಾತ್ರ,
            UBIDI_REORDER_DEFAULT, p_Error_ಸಂಕೇತ);
    *pTransform->p_Dest_ಉದ್ದಳತೆ = pTransform->src_ಉದ್ದಳತೆ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
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
ಸ್ಥಿರ ಶೂನ್ಯ
updateSrc(UBiDiTransform *pTransform, ಸ್ಥಿರಾಂಕ char16_t *newSrc, uint32_t new_ಉದ್ದಳತೆ,
        uint32_t new_ಗಾತ್ರ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಆದರೆ (new_ಗಾತ್ರ < new_ಉದ್ದಳತೆ) {
        *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (new_ಗಾತ್ರ > pTransform->src_ಗಾತ್ರ) {
        new_ಗಾತ್ರ += 50; // allocate slightly more than needed right now
        ಆದರೆ (pTransform->src != ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(pTransform->src);
            pTransform->src = ಶೂನ್ಯಸೂಚಕ;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_ಗಾತ್ರ * ಗಾತ್ರ(char16_t)));
        ಆದರೆ (pTransform->src == ಶೂನ್ಯಸೂಚಕ) {
            *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            ಹಿಂತಿರುಗಿ;
        }
        pTransform->src_ಗಾತ್ರ = new_ಗಾತ್ರ;
    }
    u_strncpy(pTransform->src, newSrc, new_ಉದ್ದಳತೆ);
    pTransform->src_ಉದ್ದಳತೆ = u_terminateUChars(pTransform->src,
    		pTransform->src_ಗಾತ್ರ, new_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
doShape(UBiDiTransform *pTransform, uint32_t options, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    *pTransform->p_Dest_ಉದ್ದಳತೆ = u_shapeArabic(pTransform->src,
            pTransform->src_ಉದ್ದಳತೆ, pTransform->dest, pTransform->dest_ಗಾತ್ರ,
            options, p_Error_ಸಂಕೇತ);
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
ಸ್ಥಿರ UBool
action_shapeArabic(UBiDiTransform *pTransform, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಆದರೆ ((pTransform->letters | pTransform->digits) == 0) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ (pTransform->p_ಸಕ್ರಿಯ_Scheme->lettersDir == pTransform->p_ಸಕ್ರಿಯ_Scheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->p_ಸಕ್ರಿಯ_Scheme->lettersDir,
                p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        doShape(pTransform, pTransform->digits | pTransform->p_ಸಕ್ರಿಯ_Scheme->digitsDir, p_Error_ಸಂಕೇತ);
        ಆದರೆ (U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_ಉದ್ದಳತೆ,
                    *pTransform->p_Dest_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
            doShape(pTransform, pTransform->letters | pTransform->p_ಸಕ್ರಿಯ_Scheme->lettersDir,
                    p_Error_ಸಂಕೇತ);
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
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
ಸ್ಥಿರ UBool
action_mirror(UBiDiTransform *pTransform, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    ಆದರೆ (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಮಾಡು {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_ಮುಂದಿನ(pTransform->src, i, pTransform->src_ಉದ್ದಳತೆ, c); 
        ಆದರೆ (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_ಉದ್ದಳತೆ(c);
        ಆದರೆ (j + k <= pTransform->dest_ಗಾತ್ರ) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } ಇಲ್ಲದಿದ್ದರೆ {
            j += k;
        }
    } ಆಗ (i < pTransform->src_ಉದ್ದಳತೆ);
    
    *pTransform->p_Dest_ಉದ್ದಳತೆ = j;
    ಆದರೆ (pTransform->dest_ಗಾತ್ರ < j) {
        *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * All possible reordering schemes.
 *
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ReorderingScheme Schemes[] =
{
    /* 0: Logical LTR => Visual LTR */
    {LTR, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, ಶೂನ್ಯಸೂಚಕ}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, ಶೂನ್ಯಸೂಚಕ}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_ಹಿಮ್ಮುಖ, ಶೂನ್ಯಸೂಚಕ}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_ಹಿಮ್ಮುಖ, ಶೂನ್ಯಸೂಚಕ}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_ಗಣ_Inverse, action_resolve, action_reorder, ಶೂನ್ಯಸೂಚಕ}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_ಹಿಮ್ಮುಖ, action_shapeArabic, action_ಗಣ_Inverse, action_resolve, action_reorder, ಶೂನ್ಯಸೂಚಕ}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_ಗಣ_Inverse, action_resolve, action_reorder, action_shapeArabic, ಶೂನ್ಯಸೂಚಕ}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_ಹಿಮ್ಮುಖ, action_ಗಣ_Inverse, action_resolve, action_reorder, action_shapeArabic, ಶೂನ್ಯಸೂಚಕ}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_ಗಣ_Runs_Only, action_resolve, action_reorder, ಶೂನ್ಯಸೂಚಕ}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_ಗಣ_Runs_Only, action_resolve, action_reorder, action_shapeArabic, ಶೂನ್ಯಸೂಚಕ}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_ಗಣ_Inverse, action_resolve, action_mirror, action_ಹಿಮ್ಮುಖ, ಶೂನ್ಯಸೂಚಕ}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_ಹಿಮ್ಮುಖ, action_shapeArabic, action_ಗಣ_Inverse, action_resolve, action_mirror, ಶೂನ್ಯಸೂಚಕ}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, ಶೂನ್ಯಸೂಚಕ}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, ಶೂನ್ಯಸೂಚಕ}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, ಶೂನ್ಯಸೂಚಕ}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_ಹಿಮ್ಮುಖ, action_resolve, action_mirror, action_shapeArabic, action_ಹಿಮ್ಮುಖ, ಶೂನ್ಯಸೂಚಕ}}
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t nSchemes = ಗಾತ್ರ(Schemes) / ಗಾತ್ರ(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
resolve_ತಳ_Direction(ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ, uint32_t length,
        UBiDiLevel *p_ಒಳ_Level, UBiDiLevel *p_ಹೊರಹೋಗು_Level)
{
    ಆಯ್ಕೆ (*p_ಒಳ_Level) {
        ಸ್ಥಿತಿ UBIDI_DEFAULT_LTR:
        ಸ್ಥಿತಿ UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_get_ತಳ_Direction(ಪಠ್ಯ, length));
            *p_ಒಳ_Level = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *p_ಒಳ_Level == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            ನಿಲ್ಲಿಸು;
        }
        ಪೂರ್ವನಿಯೋಜಿತ:
            *p_ಒಳ_Level &= 1;
            ನಿಲ್ಲಿಸು;
    }
    ಆಯ್ಕೆ (*p_ಹೊರಹೋಗು_Level) {
        ಸ್ಥಿತಿ UBIDI_DEFAULT_LTR:
        ಸ್ಥಿತಿ UBIDI_DEFAULT_RTL:
            *p_ಹೊರಹೋಗು_Level = *p_ಒಳ_Level;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            *p_ಹೊರಹೋಗು_Level &= 1;
            ನಿಲ್ಲಿಸು;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ReorderingScheme*
ಕಂಡುಹಿಡಿ_Matching_Scheme(UBiDiLevel ಒಳ_Level, UBiDiLevel ಹೊರಹೋಗು_Level,
        UBi_Di_ಕ್ರಮ ಒಳ_ಕ್ರಮ, UBi_Di_ಕ್ರಮ ಹೊರಹೋಗು_ಕ್ರಮ)
{
    uint32_t i;
    ಗಾಗಿ (i = 0; i < nSchemes; i++) {
        ಸ್ಥಿರಾಂಕ ReorderingScheme *pScheme = Schemes + i;
        ಆದರೆ (ಒಳ_Level == pScheme->ಒಳ_Level && ಹೊರಹೋಗು_Level == pScheme->ಹೊರಹೋಗು_Level
                && ಒಳ_ಕ್ರಮ == pScheme->ಒಳ_ಕ್ರಮ && ಹೊರಹೋಗು_ಕ್ರಮ == pScheme->ಹೊರಹೋಗು_ಕ್ರಮ) {
            ಹಿಂತಿರುಗಿ pScheme;
        }
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
            char16_t *dest, int32_t dest_ಗಾತ್ರ,
            UBiDiLevel ಒಳ_Para_Level, UBi_Di_ಕ್ರಮ ಒಳ_ಕ್ರಮ,
            UBiDiLevel ಹೊರಹೋಗು_Para_Level, UBi_Di_ಕ್ರಮ ಹೊರಹೋಗು_ಕ್ರಮ,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    uint32_t dest_ಉದ್ದಳತೆ = 0;
    UBool ಪಠ್ಯ_ಬದಲಾಯಿತು = ಸುಳ್ಳು;
    ಸ್ಥಿರಾಂಕ UBiDiTransform *pOrigTransform = pBiDiTransform;
    ಸ್ಥಿರಾಂಕ UBiDiAction *action = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (src == ಶೂನ್ಯಸೂಚಕ || dest == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    CHECK_LEN(src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    CHECK_LEN(dest, dest_ಗಾತ್ರ, p_Error_ಸಂಕೇತ);

    ಆದರೆ (pBiDiTransform == ಶೂನ್ಯಸೂಚಕ) {
        pBiDiTransform = ubiditransform_open(p_Error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolve_ತಳ_Direction(src, src_ಉದ್ದಳತೆ, &ಒಳ_Para_Level, &ಹೊರಹೋಗು_Para_Level);

    pBiDiTransform->p_ಸಕ್ರಿಯ_Scheme = ಕಂಡುಹಿಡಿ_Matching_Scheme(ಒಳ_Para_Level, ಹೊರಹೋಗು_Para_Level,
            ಒಳ_ಕ್ರಮ, ಹೊರಹೋಗು_ಕ್ರಮ);
    ಆದರೆ (pBiDiTransform->p_ಸಕ್ರಿಯ_Scheme == ಶೂನ್ಯಸೂಚಕ) {
        ಹೋಗು cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_ಪಠ್ಯ_DIRECTION_ಮುಖವಾಡ;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_ಮುಖವಾಡ;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_ಮುಖವಾಡ;

    updateSrc(pBiDiTransform, src, src_ಉದ್ದಳತೆ, dest_ಗಾತ್ರ > src_ಉದ್ದಳತೆ ? dest_ಗಾತ್ರ : src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹೋಗು cleanup;
    }
    ಆದರೆ (pBiDiTransform->pBidi == ಶೂನ್ಯಸೂಚಕ) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, p_Error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹೋಗು cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_ಗಾತ್ರ = dest_ಗಾತ್ರ;
    pBiDiTransform->p_Dest_ಉದ್ದಳತೆ = &dest_ಉದ್ದಳತೆ;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    ಗಾಗಿ (action = pBiDiTransform->p_ಸಕ್ರಿಯ_Scheme->actions; *action && U_SUCCESS(*p_Error_ಸಂಕೇತ); action++) {
        ಆದರೆ ((*action)(pBiDiTransform, p_Error_ಸಂಕೇತ)) {
            ಆದರೆ (action[1] != ಶೂನ್ಯಸೂಚಕ) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_ಉದ್ದಳತೆ,
                        *pBiDiTransform->p_Dest_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
            }
            ಪಠ್ಯ_ಬದಲಾಯಿತು = ನಿಜ;
        }
    }
    ubidi_ಗಣ_Inverse(pBiDiTransform->pBidi, ಸುಳ್ಳು);

    ಆದರೆ (!ಪಠ್ಯ_ಬದಲಾಯಿತು && U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        /* Text was not changed - just copy src to dest */
        ಆದರೆ (dest_ಗಾತ್ರ < src_ಉದ್ದಳತೆ) {
            *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
            u_strncpy(dest, src, src_ಉದ್ದಳತೆ);
            dest_ಉದ್ದಳತೆ = src_ಉದ್ದಳತೆ;
        }
    }
cleanup:
    ಆದರೆ (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } ಇಲ್ಲದಿದ್ದರೆ {
        pBiDiTransform->dest = ಶೂನ್ಯಸೂಚಕ;
        pBiDiTransform->p_Dest_ಉದ್ದಳತೆ = ಶೂನ್ಯಸೂಚಕ;
        pBiDiTransform->src_ಉದ್ದಳತೆ = 0;
        pBiDiTransform->dest_ಗಾತ್ರ = 0;
    }
    ಹಿಂತಿರುಗಿ (U_FAILURE(*p_Error_ಸಂಕೇತ) && *p_Error_ಸಂಕೇತ != U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) ? 0 : dest_ಉದ್ದಳತೆ;
}
