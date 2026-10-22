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

#ضمن "cmemory.h"
#ضمن "unicode/ubidi.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/ushape.h"
#ضمن "unicode/utf16.h"
#ضمن "ustr_imp.h"
#ضمن "unicode/ubiditransform.h"

/* Some convenience defines */
#وصف_ڪريو LTR                     UBIDI_LTR
#وصف_ڪريو RTL                     UBIDI_RTL
#وصف_ڪريو LOGICAL                 UBIDI_LOGICAL
#وصف_ڪريو VISUAL                  UBIDI_VISUAL
#وصف_ڪريو SHAPE_LOGICAL           U_SHAPE_متن_DIRECTION_LOGICAL
#وصف_ڪريو SHAPE_VISUAL            U_SHAPE_متن_DIRECTION_VISUAL_LTR

#وصف_ڪريو CHECK_LEN(STR, LEN, ERROR) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا (LEN == 0) أرجع 0; \
    إذا (LEN < -1) { *(ERROR) = U_ILLEGAL_دليل_ERROR; أرجع 0; } \
    إذا (LEN == -1) LEN = u_strlen(STR); \
} UPRV_بلاڪ_MACRO_پڇاڙي

#وصف_ڪريو MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
تعريفنوع UBool (*UBiDiAction)(UBiDiTransform *, UError_ڪوڊ *);

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
تعريفنوع بنية {
    UBiDiLevel        اندريون_Level;               /* input level */
    UBi_Di_ترتيب        اندريون_ترتيب;               /* input order */
    UBiDiLevel        ٻاهريون_Level;              /* output level */
    UBi_Di_ترتيب        ٻاهريون_ترتيب;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        بنياد_Level;             /* paragraph level to be used with setPara */
    ثابت UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

بنية UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ثابت ReorderingScheme  *p_سرگرم_Scheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_ڊيگهائي;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_سائيز;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_سائيز;           /* output text capacity */
    uint32_t                *p_Dest_ڊيگهائي;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UError_ڪوڊ *p_Error_ڪوڊ)
{
    UBiDiTransform *pBiDiTransform = خالي_اشارو;
    إذا (U_SUCCESS(*p_Error_ڪوڊ)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, حجم(UBiDiTransform));
        إذا (pBiDiTransform == خالي_اشارو) {
            *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        }
    }
    أرجع pBiDiTransform;
}

U_CAPI فراغ U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    إذا (pBiDiTransform != خالي_اشارو) {
        إذا (pBiDiTransform->pBidi != خالي_اشارو) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        إذا (pBiDiTransform->src != خالي_اشارو) {
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
ساكن UBool
action_resolve(UBiDiTransform *pTransform, UError_ڪوڊ *p_Error_ڪوڊ)
{
    ubidi_سيٽ_Para(pTransform->pBidi, pTransform->src, pTransform->src_ڊيگهائي,
            pTransform->p_سرگرم_Scheme->بنياد_Level, خالي_اشارو, p_Error_ڪوڊ);
    أرجع خطأ;
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
ساكن UBool
action_reorder(UBiDiTransform *pTransform, UError_ڪوڊ *p_Error_ڪوڊ)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_سائيز,
            static_cast<uint16_t>(pTransform->reorderingOptions), p_Error_ڪوڊ);

    *pTransform->p_Dest_ڊيگهائي = pTransform->src_ڊيگهائي;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    أرجع سچ;
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
ساكن UBool
action_سيٽ_Inverse(UBiDiTransform *pTransform, UError_ڪوڊ *p_Error_ڪوڊ)
{
    (فراغ)p_Error_ڪوڊ;
    ubidi_سيٽ_Inverse(pTransform->pBidi, سچ);
    ubidi_سيٽ_Reordering_گهڻائي(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_سڌو);
    أرجع خطأ;
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
ساكن UBool
action_سيٽ_Runs_Only(UBiDiTransform *pTransform, UError_ڪوڊ *p_Error_ڪوڊ)
{
    (فراغ)p_Error_ڪوڊ;
    ubidi_سيٽ_Reordering_گهڻائي(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    أرجع خطأ;
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
ساكن UBool
action_ابتو(UBiDiTransform *pTransform, UError_ڪوڊ *p_Error_ڪوڊ)
{
    ubidi_write_ابتو(pTransform->src, pTransform->src_ڊيگهائي,
            pTransform->dest, pTransform->dest_سائيز,
            UBIDI_REORDER_DEFAULT, p_Error_ڪوڊ);
    *pTransform->p_Dest_ڊيگهائي = pTransform->src_ڊيگهائي;
    أرجع سچ;
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
ساكن فراغ
updateSrc(UBiDiTransform *pTransform, ثابت char16_t *newSrc, uint32_t new_ڊيگهائي,
        uint32_t new_سائيز, UError_ڪوڊ *p_Error_ڪوڊ)
{
    إذا (new_سائيز < new_ڊيگهائي) {
        *p_Error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
        أرجع;
    }
    إذا (new_سائيز > pTransform->src_سائيز) {
        new_سائيز += 50; // allocate slightly more than needed right now
        إذا (pTransform->src != خالي_اشارو) {
            uprv_free(pTransform->src);
            pTransform->src = خالي_اشارو;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_سائيز * حجم(char16_t)));
        إذا (pTransform->src == خالي_اشارو) {
            *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            أرجع;
        }
        pTransform->src_سائيز = new_سائيز;
    }
    u_strncpy(pTransform->src, newSrc, new_ڊيگهائي);
    pTransform->src_ڊيگهائي = u_terminateUChars(pTransform->src,
    		pTransform->src_سائيز, new_ڊيگهائي, p_Error_ڪوڊ);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
ساكن فراغ
doShape(UBiDiTransform *pTransform, uint32_t options, UError_ڪوڊ *p_Error_ڪوڊ)
{
    *pTransform->p_Dest_ڊيگهائي = u_shapeArabic(pTransform->src,
            pTransform->src_ڊيگهائي, pTransform->dest, pTransform->dest_سائيز,
            options, p_Error_ڪوڊ);
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
ساكن UBool
action_shapeArabic(UBiDiTransform *pTransform, UError_ڪوڊ *p_Error_ڪوڊ)
{
    إذا ((pTransform->letters | pTransform->digits) == 0) {
        أرجع خطأ;
    }
    إذا (pTransform->p_سرگرم_Scheme->lettersDir == pTransform->p_سرگرم_Scheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->p_سرگرم_Scheme->lettersDir,
                p_Error_ڪوڊ);
    } وإلا {
        doShape(pTransform, pTransform->digits | pTransform->p_سرگرم_Scheme->digitsDir, p_Error_ڪوڊ);
        إذا (U_SUCCESS(*p_Error_ڪوڊ)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_ڊيگهائي,
                    *pTransform->p_Dest_ڊيگهائي, p_Error_ڪوڊ);
            doShape(pTransform, pTransform->letters | pTransform->p_سرگرم_Scheme->lettersDir,
                    p_Error_ڪوڊ);
        }
    }
    أرجع سچ;
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
ساكن UBool
action_mirror(UBiDiTransform *pTransform, UError_ڪوڊ *p_Error_ڪوڊ)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    إذا (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        أرجع خطأ;
    }
    افعل {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_ايندڙ(pTransform->src, i, pTransform->src_ڊيگهائي, c); 
        إذا (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_ڊيگهائي(c);
        إذا (j + k <= pTransform->dest_سائيز) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } وإلا {
            j += k;
        }
    } طالما (i < pTransform->src_ڊيگهائي);
    
    *pTransform->p_Dest_ڊيگهائي = j;
    إذا (pTransform->dest_سائيز < j) {
        *p_Error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
        أرجع خطأ;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    أرجع سچ;
}

/**
 * All possible reordering schemes.
 *
 */
ساكن ثابت ReorderingScheme Schemes[] =
{
    /* 0: Logical LTR => Visual LTR */
    {LTR, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, خالي_اشارو}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, خالي_اشارو}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_ابتو, خالي_اشارو}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_ابتو, خالي_اشارو}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_سيٽ_Inverse, action_resolve, action_reorder, خالي_اشارو}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_ابتو, action_shapeArabic, action_سيٽ_Inverse, action_resolve, action_reorder, خالي_اشارو}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_سيٽ_Inverse, action_resolve, action_reorder, action_shapeArabic, خالي_اشارو}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_ابتو, action_سيٽ_Inverse, action_resolve, action_reorder, action_shapeArabic, خالي_اشارو}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_سيٽ_Runs_Only, action_resolve, action_reorder, خالي_اشارو}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_سيٽ_Runs_Only, action_resolve, action_reorder, action_shapeArabic, خالي_اشارو}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_سيٽ_Inverse, action_resolve, action_mirror, action_ابتو, خالي_اشارو}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_ابتو, action_shapeArabic, action_سيٽ_Inverse, action_resolve, action_mirror, خالي_اشارو}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, خالي_اشارو}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, خالي_اشارو}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, خالي_اشارو}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_ابتو, action_resolve, action_mirror, action_shapeArabic, action_ابتو, خالي_اشارو}}
};

ساكن ثابت uint32_t nSchemes = حجم(Schemes) / حجم(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
ساكن فراغ
resolve_بنياد_Direction(ثابت char16_t *متن, uint32_t length,
        UBiDiLevel *p_اندريون_Level, UBiDiLevel *p_ٻاهريون_Level)
{
    بدل (*p_اندريون_Level) {
        حالة UBIDI_DEFAULT_LTR:
        حالة UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_get_بنياد_Direction(متن, length));
            *p_اندريون_Level = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *p_اندريون_Level == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            اكسر;
        }
        افتراضي:
            *p_اندريون_Level &= 1;
            اكسر;
    }
    بدل (*p_ٻاهريون_Level) {
        حالة UBIDI_DEFAULT_LTR:
        حالة UBIDI_DEFAULT_RTL:
            *p_ٻاهريون_Level = *p_اندريون_Level;
            اكسر;
        افتراضي:
            *p_ٻاهريون_Level &= 1;
            اكسر;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
ساكن ثابت ReorderingScheme*
ڳوليو_Matching_Scheme(UBiDiLevel اندريون_Level, UBiDiLevel ٻاهريون_Level,
        UBi_Di_ترتيب اندريون_ترتيب, UBi_Di_ترتيب ٻاهريون_ترتيب)
{
    uint32_t i;
    لكل (i = 0; i < nSchemes; i++) {
        ثابت ReorderingScheme *pScheme = Schemes + i;
        إذا (اندريون_Level == pScheme->اندريون_Level && ٻاهريون_Level == pScheme->ٻاهريون_Level
                && اندريون_ترتيب == pScheme->اندريون_ترتيب && ٻاهريون_ترتيب == pScheme->ٻاهريون_ترتيب) {
            أرجع pScheme;
        }
    }
    أرجع خالي_اشارو;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ثابت char16_t *src, int32_t src_ڊيگهائي,
            char16_t *dest, int32_t dest_سائيز,
            UBiDiLevel اندريون_Para_Level, UBi_Di_ترتيب اندريون_ترتيب,
            UBiDiLevel ٻاهريون_Para_Level, UBi_Di_ترتيب ٻاهريون_ترتيب,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UError_ڪوڊ *p_Error_ڪوڊ)
{
    uint32_t dest_ڊيگهائي = 0;
    UBool متن_بدليو = خطأ;
    ثابت UBiDiTransform *pOrigTransform = pBiDiTransform;
    ثابت UBiDiAction *action = خالي_اشارو;

    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا (src == خالي_اشارو || dest == خالي_اشارو) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    CHECK_LEN(src, src_ڊيگهائي, p_Error_ڪوڊ);
    CHECK_LEN(dest, dest_سائيز, p_Error_ڪوڊ);

    إذا (pBiDiTransform == خالي_اشارو) {
        pBiDiTransform = ubiditransform_open(p_Error_ڪوڊ);
        إذا (U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolve_بنياد_Direction(src, src_ڊيگهائي, &اندريون_Para_Level, &ٻاهريون_Para_Level);

    pBiDiTransform->p_سرگرم_Scheme = ڳوليو_Matching_Scheme(اندريون_Para_Level, ٻاهريون_Para_Level,
            اندريون_ترتيب, ٻاهريون_ترتيب);
    إذا (pBiDiTransform->p_سرگرم_Scheme == خالي_اشارو) {
        اذهب cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_متن_DIRECTION_نقاب;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_نقاب;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_نقاب;

    updateSrc(pBiDiTransform, src, src_ڊيگهائي, dest_سائيز > src_ڊيگهائي ? dest_سائيز : src_ڊيگهائي, p_Error_ڪوڊ);
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        اذهب cleanup;
    }
    إذا (pBiDiTransform->pBidi == خالي_اشارو) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, p_Error_ڪوڊ);
        إذا (U_FAILURE(*p_Error_ڪوڊ)) {
            اذهب cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_سائيز = dest_سائيز;
    pBiDiTransform->p_Dest_ڊيگهائي = &dest_ڊيگهائي;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    لكل (action = pBiDiTransform->p_سرگرم_Scheme->actions; *action && U_SUCCESS(*p_Error_ڪوڊ); action++) {
        إذا ((*action)(pBiDiTransform, p_Error_ڪوڊ)) {
            إذا (action[1] != خالي_اشارو) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_ڊيگهائي,
                        *pBiDiTransform->p_Dest_ڊيگهائي, p_Error_ڪوڊ);
            }
            متن_بدليو = سچ;
        }
    }
    ubidi_سيٽ_Inverse(pBiDiTransform->pBidi, خطأ);

    إذا (!متن_بدليو && U_SUCCESS(*p_Error_ڪوڊ)) {
        /* Text was not changed - just copy src to dest */
        إذا (dest_سائيز < src_ڊيگهائي) {
            *p_Error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
        } وإلا {
            u_strncpy(dest, src, src_ڊيگهائي);
            dest_ڊيگهائي = src_ڊيگهائي;
        }
    }
cleanup:
    إذا (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } وإلا {
        pBiDiTransform->dest = خالي_اشارو;
        pBiDiTransform->p_Dest_ڊيگهائي = خالي_اشارو;
        pBiDiTransform->src_ڊيگهائي = 0;
        pBiDiTransform->dest_سائيز = 0;
    }
    أرجع (U_FAILURE(*p_Error_ڪوڊ) && *p_Error_ڪوڊ != U_بفر_حد_کان_مٿي_ERROR) ? 0 : dest_ڊيگهائي;
}
