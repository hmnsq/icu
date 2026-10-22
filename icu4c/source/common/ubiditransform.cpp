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

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubidi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ushape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ubiditransform.h"

/* Some convenience defines */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LTR                     UBIDI_LTR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTL                     UBIDI_RTL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOGICAL                 UBIDI_LOGICAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VISUAL                  UBIDI_VISUAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHAPE_LOGICAL           U_SHAPE_ਪਾਠ_DIRECTION_LOGICAL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SHAPE_VISUAL            U_SHAPE_ਪਾਠ_DIRECTION_VISUAL_LTR

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_LEN(STR, LEN, ERROR) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ (LEN == 0) ਵਾਪਸ 0; \
    ਜੇ (LEN < -1) { *(ERROR) = U_ILLEGAL_ਦਲੀਲ_ERROR; ਵਾਪਸ 0; } \
    ਜੇ (LEN == -1) LEN = u_strlen(STR); \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ UBool (*UBiDiAction)(UBiDiTransform *, UError_ਕੋਡ *);

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    UBiDiLevel        ਵਿੱਚ_Level;               /* input level */
    UBi_Di_ਕ੍ਰਮ        ਵਿੱਚ_ਕ੍ਰਮ;               /* input order */
    UBiDiLevel        ਬਾਹਰਲਾ_Level;              /* output level */
    UBi_Di_ਕ੍ਰਮ        ਬਾਹਰਲਾ_ਕ੍ਰਮ;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        ਅਧਾਰ_Level;             /* paragraph level to be used with setPara */
    ਅਚਲ UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

ਬਣਤਰ UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ਅਚਲ ReorderingScheme  *p_ਸਰਗਰਮ_Scheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_ਲੰਮਾਈ;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_ਮਾਪ;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_ਮਾਪ;           /* output text capacity */
    uint32_t                *p_Dest_ਲੰਮਾਈ;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    UBiDiTransform *pBiDiTransform = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (U_SUCCESS(*p_Error_ਕੋਡ)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, ਆਕਾਰ(UBiDiTransform));
        ਜੇ (pBiDiTransform == ਖਾਲੀ_ਸੂਚਕ) {
            *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        }
    }
    ਵਾਪਸ pBiDiTransform;
}

U_CAPI ਖਾਲੀ U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    ਜੇ (pBiDiTransform != ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (pBiDiTransform->pBidi != ਖਾਲੀ_ਸੂਚਕ) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        ਜੇ (pBiDiTransform->src != ਖਾਲੀ_ਸੂਚਕ) {
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
ਸਥਿਰ UBool
action_resolve(UBiDiTransform *pTransform, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ubidi_ਸੈੱਟ_Para(pTransform->pBidi, pTransform->src, pTransform->src_ਲੰਮਾਈ,
            pTransform->p_ਸਰਗਰਮ_Scheme->ਅਧਾਰ_Level, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
    ਵਾਪਸ ਝੂਠ;
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
ਸਥਿਰ UBool
action_reorder(UBiDiTransform *pTransform, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_ਮਾਪ,
            static_cast<uint16_t>(pTransform->reorderingOptions), p_Error_ਕੋਡ);

    *pTransform->p_Dest_ਲੰਮਾਈ = pTransform->src_ਲੰਮਾਈ;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    ਵਾਪਸ ਸੱਚ;
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
ਸਥਿਰ UBool
action_ਸੈੱਟ_Inverse(UBiDiTransform *pTransform, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    ubidi_ਸੈੱਟ_Inverse(pTransform->pBidi, ਸੱਚ);
    ubidi_ਸੈੱਟ_Reordering_ਬਹੁਲਕ(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_ਸਿੱਧਾ);
    ਵਾਪਸ ਝੂਠ;
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
ਸਥਿਰ UBool
action_ਸੈੱਟ_Runs_Only(UBiDiTransform *pTransform, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    (ਖਾਲੀ)p_Error_ਕੋਡ;
    ubidi_ਸੈੱਟ_Reordering_ਬਹੁਲਕ(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    ਵਾਪਸ ਝੂਠ;
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
ਸਥਿਰ UBool
action_ਉਲਟ(UBiDiTransform *pTransform, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ubidi_write_ਉਲਟ(pTransform->src, pTransform->src_ਲੰਮਾਈ,
            pTransform->dest, pTransform->dest_ਮਾਪ,
            UBIDI_REORDER_DEFAULT, p_Error_ਕੋਡ);
    *pTransform->p_Dest_ਲੰਮਾਈ = pTransform->src_ਲੰਮਾਈ;
    ਵਾਪਸ ਸੱਚ;
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
ਸਥਿਰ ਖਾਲੀ
updateSrc(UBiDiTransform *pTransform, ਅਚਲ char16_t *newSrc, uint32_t new_ਲੰਮਾਈ,
        uint32_t new_ਮਾਪ, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਜੇ (new_ਮਾਪ < new_ਲੰਮਾਈ) {
        *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ (new_ਮਾਪ > pTransform->src_ਮਾਪ) {
        new_ਮਾਪ += 50; // allocate slightly more than needed right now
        ਜੇ (pTransform->src != ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(pTransform->src);
            pTransform->src = ਖਾਲੀ_ਸੂਚਕ;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_ਮਾਪ * ਆਕਾਰ(char16_t)));
        ਜੇ (pTransform->src == ਖਾਲੀ_ਸੂਚਕ) {
            *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            ਵਾਪਸ;
        }
        pTransform->src_ਮਾਪ = new_ਮਾਪ;
    }
    u_strncpy(pTransform->src, newSrc, new_ਲੰਮਾਈ);
    pTransform->src_ਲੰਮਾਈ = u_terminateUChars(pTransform->src,
    		pTransform->src_ਮਾਪ, new_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
ਸਥਿਰ ਖਾਲੀ
doShape(UBiDiTransform *pTransform, uint32_t options, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    *pTransform->p_Dest_ਲੰਮਾਈ = u_shapeArabic(pTransform->src,
            pTransform->src_ਲੰਮਾਈ, pTransform->dest, pTransform->dest_ਮਾਪ,
            options, p_Error_ਕੋਡ);
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
ਸਥਿਰ UBool
action_shapeArabic(UBiDiTransform *pTransform, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਜੇ ((pTransform->letters | pTransform->digits) == 0) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ (pTransform->p_ਸਰਗਰਮ_Scheme->lettersDir == pTransform->p_ਸਰਗਰਮ_Scheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->p_ਸਰਗਰਮ_Scheme->lettersDir,
                p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        doShape(pTransform, pTransform->digits | pTransform->p_ਸਰਗਰਮ_Scheme->digitsDir, p_Error_ਕੋਡ);
        ਜੇ (U_SUCCESS(*p_Error_ਕੋਡ)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_ਲੰਮਾਈ,
                    *pTransform->p_Dest_ਲੰਮਾਈ, p_Error_ਕੋਡ);
            doShape(pTransform, pTransform->letters | pTransform->p_ਸਰਗਰਮ_Scheme->lettersDir,
                    p_Error_ਕੋਡ);
        }
    }
    ਵਾਪਸ ਸੱਚ;
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
ਸਥਿਰ UBool
action_mirror(UBiDiTransform *pTransform, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    ਜੇ (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਕਰੋ {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_ਅਗਲਾ(pTransform->src, i, pTransform->src_ਲੰਮਾਈ, c); 
        ਜੇ (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_ਲੰਮਾਈ(c);
        ਜੇ (j + k <= pTransform->dest_ਮਾਪ) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } ਨਹੀਂਤਾਂ {
            j += k;
        }
    } ਜਦੋਂ (i < pTransform->src_ਲੰਮਾਈ);
    
    *pTransform->p_Dest_ਲੰਮਾਈ = j;
    ਜੇ (pTransform->dest_ਮਾਪ < j) {
        *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    ਵਾਪਸ ਸੱਚ;
}

/**
 * All possible reordering schemes.
 *
 */
ਸਥਿਰ ਅਚਲ ReorderingScheme Schemes[] =
{
    /* 0: Logical LTR => Visual LTR */
    {LTR, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, ਖਾਲੀ_ਸੂਚਕ}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, ਖਾਲੀ_ਸੂਚਕ}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_ਉਲਟ, ਖਾਲੀ_ਸੂਚਕ}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_ਉਲਟ, ਖਾਲੀ_ਸੂਚਕ}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_ਸੈੱਟ_Inverse, action_resolve, action_reorder, ਖਾਲੀ_ਸੂਚਕ}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_ਉਲਟ, action_shapeArabic, action_ਸੈੱਟ_Inverse, action_resolve, action_reorder, ਖਾਲੀ_ਸੂਚਕ}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_ਸੈੱਟ_Inverse, action_resolve, action_reorder, action_shapeArabic, ਖਾਲੀ_ਸੂਚਕ}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_ਉਲਟ, action_ਸੈੱਟ_Inverse, action_resolve, action_reorder, action_shapeArabic, ਖਾਲੀ_ਸੂਚਕ}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_ਸੈੱਟ_Runs_Only, action_resolve, action_reorder, ਖਾਲੀ_ਸੂਚਕ}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_ਸੈੱਟ_Runs_Only, action_resolve, action_reorder, action_shapeArabic, ਖਾਲੀ_ਸੂਚਕ}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_ਸੈੱਟ_Inverse, action_resolve, action_mirror, action_ਉਲਟ, ਖਾਲੀ_ਸੂਚਕ}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_ਉਲਟ, action_shapeArabic, action_ਸੈੱਟ_Inverse, action_resolve, action_mirror, ਖਾਲੀ_ਸੂਚਕ}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, ਖਾਲੀ_ਸੂਚਕ}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, ਖਾਲੀ_ਸੂਚਕ}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, ਖਾਲੀ_ਸੂਚਕ}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_ਉਲਟ, action_resolve, action_mirror, action_shapeArabic, action_ਉਲਟ, ਖਾਲੀ_ਸੂਚਕ}}
};

ਸਥਿਰ ਅਚਲ uint32_t nSchemes = ਆਕਾਰ(Schemes) / ਆਕਾਰ(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
ਸਥਿਰ ਖਾਲੀ
resolve_ਅਧਾਰ_Direction(ਅਚਲ char16_t *ਪਾਠ, uint32_t length,
        UBiDiLevel *p_ਵਿੱਚ_Level, UBiDiLevel *p_ਬਾਹਰਲਾ_Level)
{
    ਚੋਣ (*p_ਵਿੱਚ_Level) {
        ਹਾਲਤ UBIDI_DEFAULT_LTR:
        ਹਾਲਤ UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_get_ਅਧਾਰ_Direction(ਪਾਠ, length));
            *p_ਵਿੱਚ_Level = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *p_ਵਿੱਚ_Level == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            ਰੋਕੋ;
        }
        ਮੂਲ:
            *p_ਵਿੱਚ_Level &= 1;
            ਰੋਕੋ;
    }
    ਚੋਣ (*p_ਬਾਹਰਲਾ_Level) {
        ਹਾਲਤ UBIDI_DEFAULT_LTR:
        ਹਾਲਤ UBIDI_DEFAULT_RTL:
            *p_ਬਾਹਰਲਾ_Level = *p_ਵਿੱਚ_Level;
            ਰੋਕੋ;
        ਮੂਲ:
            *p_ਬਾਹਰਲਾ_Level &= 1;
            ਰੋਕੋ;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
ਸਥਿਰ ਅਚਲ ReorderingScheme*
ਲੱਭੋ_Matching_Scheme(UBiDiLevel ਵਿੱਚ_Level, UBiDiLevel ਬਾਹਰਲਾ_Level,
        UBi_Di_ਕ੍ਰਮ ਵਿੱਚ_ਕ੍ਰਮ, UBi_Di_ਕ੍ਰਮ ਬਾਹਰਲਾ_ਕ੍ਰਮ)
{
    uint32_t i;
    ਲਈ (i = 0; i < nSchemes; i++) {
        ਅਚਲ ReorderingScheme *pScheme = Schemes + i;
        ਜੇ (ਵਿੱਚ_Level == pScheme->ਵਿੱਚ_Level && ਬਾਹਰਲਾ_Level == pScheme->ਬਾਹਰਲਾ_Level
                && ਵਿੱਚ_ਕ੍ਰਮ == pScheme->ਵਿੱਚ_ਕ੍ਰਮ && ਬਾਹਰਲਾ_ਕ੍ਰਮ == pScheme->ਬਾਹਰਲਾ_ਕ੍ਰਮ) {
            ਵਾਪਸ pScheme;
        }
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
            char16_t *dest, int32_t dest_ਮਾਪ,
            UBiDiLevel ਵਿੱਚ_Para_Level, UBi_Di_ਕ੍ਰਮ ਵਿੱਚ_ਕ੍ਰਮ,
            UBiDiLevel ਬਾਹਰਲਾ_Para_Level, UBi_Di_ਕ੍ਰਮ ਬਾਹਰਲਾ_ਕ੍ਰਮ,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    uint32_t dest_ਲੰਮਾਈ = 0;
    UBool ਪਾਠ_ਬਦਲਿਆ = ਝੂਠ;
    ਅਚਲ UBiDiTransform *pOrigTransform = pBiDiTransform;
    ਅਚਲ UBiDiAction *action = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ (src == ਖਾਲੀ_ਸੂਚਕ || dest == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    CHECK_LEN(src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    CHECK_LEN(dest, dest_ਮਾਪ, p_Error_ਕੋਡ);

    ਜੇ (pBiDiTransform == ਖਾਲੀ_ਸੂਚਕ) {
        pBiDiTransform = ubiditransform_open(p_Error_ਕੋਡ);
        ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolve_ਅਧਾਰ_Direction(src, src_ਲੰਮਾਈ, &ਵਿੱਚ_Para_Level, &ਬਾਹਰਲਾ_Para_Level);

    pBiDiTransform->p_ਸਰਗਰਮ_Scheme = ਲੱਭੋ_Matching_Scheme(ਵਿੱਚ_Para_Level, ਬਾਹਰਲਾ_Para_Level,
            ਵਿੱਚ_ਕ੍ਰਮ, ਬਾਹਰਲਾ_ਕ੍ਰਮ);
    ਜੇ (pBiDiTransform->p_ਸਰਗਰਮ_Scheme == ਖਾਲੀ_ਸੂਚਕ) {
        ਜਾਓ cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_ਪਾਠ_DIRECTION_ਨਕਾਬ;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_ਨਕਾਬ;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_ਨਕਾਬ;

    updateSrc(pBiDiTransform, src, src_ਲੰਮਾਈ, dest_ਮਾਪ > src_ਲੰਮਾਈ ? dest_ਮਾਪ : src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        ਜਾਓ cleanup;
    }
    ਜੇ (pBiDiTransform->pBidi == ਖਾਲੀ_ਸੂਚਕ) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, p_Error_ਕੋਡ);
        ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
            ਜਾਓ cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_ਮਾਪ = dest_ਮਾਪ;
    pBiDiTransform->p_Dest_ਲੰਮਾਈ = &dest_ਲੰਮਾਈ;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    ਲਈ (action = pBiDiTransform->p_ਸਰਗਰਮ_Scheme->actions; *action && U_SUCCESS(*p_Error_ਕੋਡ); action++) {
        ਜੇ ((*action)(pBiDiTransform, p_Error_ਕੋਡ)) {
            ਜੇ (action[1] != ਖਾਲੀ_ਸੂਚਕ) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_ਲੰਮਾਈ,
                        *pBiDiTransform->p_Dest_ਲੰਮਾਈ, p_Error_ਕੋਡ);
            }
            ਪਾਠ_ਬਦਲਿਆ = ਸੱਚ;
        }
    }
    ubidi_ਸੈੱਟ_Inverse(pBiDiTransform->pBidi, ਝੂਠ);

    ਜੇ (!ਪਾਠ_ਬਦਲਿਆ && U_SUCCESS(*p_Error_ਕੋਡ)) {
        /* Text was not changed - just copy src to dest */
        ਜੇ (dest_ਮਾਪ < src_ਲੰਮਾਈ) {
            *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        } ਨਹੀਂਤਾਂ {
            u_strncpy(dest, src, src_ਲੰਮਾਈ);
            dest_ਲੰਮਾਈ = src_ਲੰਮਾਈ;
        }
    }
cleanup:
    ਜੇ (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } ਨਹੀਂਤਾਂ {
        pBiDiTransform->dest = ਖਾਲੀ_ਸੂਚਕ;
        pBiDiTransform->p_Dest_ਲੰਮਾਈ = ਖਾਲੀ_ਸੂਚਕ;
        pBiDiTransform->src_ਲੰਮਾਈ = 0;
        pBiDiTransform->dest_ਮਾਪ = 0;
    }
    ਵਾਪਸ (U_FAILURE(*p_Error_ਕੋਡ) && *p_Error_ਕੋਡ != U_ਬਫਰ_ਉਛਾਲ_ERROR) ? 0 : dest_ਲੰਮਾਈ;
}
