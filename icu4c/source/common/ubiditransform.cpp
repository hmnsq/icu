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
#تعریف_کرو LTR                     UBIDI_LTR
#تعریف_کرو RTL                     UBIDI_RTL
#تعریف_کرو LOGICAL                 UBIDI_LOGICAL
#تعریف_کرو VISUAL                  UBIDI_VISUAL
#تعریف_کرو SHAPE_LOGICAL           U_SHAPE_متن_DIRECTION_LOGICAL
#تعریف_کرو SHAPE_VISUAL            U_SHAPE_متن_DIRECTION_VISUAL_LTR

#تعریف_کرو CHECK_LEN(STR, LEN, ERROR) UPRV_بلاک_MACRO_BEGIN { \
    إذا (LEN == 0) أرجع 0; \
    إذا (LEN < -1) { *(ERROR) = U_ILLEGAL_دلیل_ERROR; أرجع 0; } \
    إذا (LEN == -1) LEN = u_strlen(STR); \
} UPRV_بلاک_MACRO_سرا

#تعریف_کرو MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
تعريفنوع UBool (*UBiDiAction)(UBiDiTransform *, UError_کوڈ *);

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
    UBiDiLevel        وچ_Level;               /* input level */
    UBi_Di_ترتیب        وچ_ترتیب;               /* input order */
    UBiDiLevel        باہرلا_Level;              /* output level */
    UBi_Di_ترتیب        باہرلا_ترتیب;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        ادھار_Level;             /* paragraph level to be used with setPara */
    ثابت UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

بنية UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ثابت ReorderingScheme  *p_سرگرم_Scheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_لمائی;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_ناپ;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_ناپ;           /* output text capacity */
    uint32_t                *p_Dest_لمائی;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UError_کوڈ *p_Error_کوڈ)
{
    UBiDiTransform *pBiDiTransform = خالی_سوچک;
    إذا (U_SUCCESS(*p_Error_کوڈ)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, حجم(UBiDiTransform));
        إذا (pBiDiTransform == خالی_سوچک) {
            *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
        }
    }
    أرجع pBiDiTransform;
}

U_CAPI فراغ U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    إذا (pBiDiTransform != خالی_سوچک) {
        إذا (pBiDiTransform->pBidi != خالی_سوچک) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        إذا (pBiDiTransform->src != خالی_سوچک) {
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
action_resolve(UBiDiTransform *pTransform, UError_کوڈ *p_Error_کوڈ)
{
    ubidi_سیٹ_Para(pTransform->pBidi, pTransform->src, pTransform->src_لمائی,
            pTransform->p_سرگرم_Scheme->ادھار_Level, خالی_سوچک, p_Error_کوڈ);
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
action_reorder(UBiDiTransform *pTransform, UError_کوڈ *p_Error_کوڈ)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_ناپ,
            static_cast<uint16_t>(pTransform->reorderingOptions), p_Error_کوڈ);

    *pTransform->p_Dest_لمائی = pTransform->src_لمائی;
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
action_سیٹ_Inverse(UBiDiTransform *pTransform, UError_کوڈ *p_Error_کوڈ)
{
    (فراغ)p_Error_کوڈ;
    ubidi_سیٹ_Inverse(pTransform->pBidi, سچ);
    ubidi_سیٹ_Reordering_منوال(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_سدھا);
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
action_سیٹ_Runs_Only(UBiDiTransform *pTransform, UError_کوڈ *p_Error_کوڈ)
{
    (فراغ)p_Error_کوڈ;
    ubidi_سیٹ_Reordering_منوال(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
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
action_الٹ(UBiDiTransform *pTransform, UError_کوڈ *p_Error_کوڈ)
{
    ubidi_write_الٹ(pTransform->src, pTransform->src_لمائی,
            pTransform->dest, pTransform->dest_ناپ,
            UBIDI_REORDER_DEFAULT, p_Error_کوڈ);
    *pTransform->p_Dest_لمائی = pTransform->src_لمائی;
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
updateSrc(UBiDiTransform *pTransform, ثابت char16_t *newSrc, uint32_t new_لمائی,
        uint32_t new_ناپ, UError_کوڈ *p_Error_کوڈ)
{
    إذا (new_ناپ < new_لمائی) {
        *p_Error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
        أرجع;
    }
    إذا (new_ناپ > pTransform->src_ناپ) {
        new_ناپ += 50; // allocate slightly more than needed right now
        إذا (pTransform->src != خالی_سوچک) {
            uprv_free(pTransform->src);
            pTransform->src = خالی_سوچک;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_ناپ * حجم(char16_t)));
        إذا (pTransform->src == خالی_سوچک) {
            *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            أرجع;
        }
        pTransform->src_ناپ = new_ناپ;
    }
    u_strncpy(pTransform->src, newSrc, new_لمائی);
    pTransform->src_لمائی = u_terminateUChars(pTransform->src,
    		pTransform->src_ناپ, new_لمائی, p_Error_کوڈ);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
ساكن فراغ
doShape(UBiDiTransform *pTransform, uint32_t options, UError_کوڈ *p_Error_کوڈ)
{
    *pTransform->p_Dest_لمائی = u_shapeArabic(pTransform->src,
            pTransform->src_لمائی, pTransform->dest, pTransform->dest_ناپ,
            options, p_Error_کوڈ);
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
action_shapeArabic(UBiDiTransform *pTransform, UError_کوڈ *p_Error_کوڈ)
{
    إذا ((pTransform->letters | pTransform->digits) == 0) {
        أرجع خطأ;
    }
    إذا (pTransform->p_سرگرم_Scheme->lettersDir == pTransform->p_سرگرم_Scheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->p_سرگرم_Scheme->lettersDir,
                p_Error_کوڈ);
    } وإلا {
        doShape(pTransform, pTransform->digits | pTransform->p_سرگرم_Scheme->digitsDir, p_Error_کوڈ);
        إذا (U_SUCCESS(*p_Error_کوڈ)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_لمائی,
                    *pTransform->p_Dest_لمائی, p_Error_کوڈ);
            doShape(pTransform, pTransform->letters | pTransform->p_سرگرم_Scheme->lettersDir,
                    p_Error_کوڈ);
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
action_mirror(UBiDiTransform *pTransform, UError_کوڈ *p_Error_کوڈ)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    إذا (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        أرجع خطأ;
    }
    افعل {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_اگلا(pTransform->src, i, pTransform->src_لمائی, c); 
        إذا (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_لمائی(c);
        إذا (j + k <= pTransform->dest_ناپ) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } وإلا {
            j += k;
        }
    } طالما (i < pTransform->src_لمائی);
    
    *pTransform->p_Dest_لمائی = j;
    إذا (pTransform->dest_ناپ < j) {
        *p_Error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
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
            {action_shapeArabic, action_resolve, action_reorder, خالی_سوچک}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, خالی_سوچک}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_الٹ, خالی_سوچک}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_الٹ, خالی_سوچک}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_سیٹ_Inverse, action_resolve, action_reorder, خالی_سوچک}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_الٹ, action_shapeArabic, action_سیٹ_Inverse, action_resolve, action_reorder, خالی_سوچک}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_سیٹ_Inverse, action_resolve, action_reorder, action_shapeArabic, خالی_سوچک}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_الٹ, action_سیٹ_Inverse, action_resolve, action_reorder, action_shapeArabic, خالی_سوچک}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_سیٹ_Runs_Only, action_resolve, action_reorder, خالی_سوچک}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_سیٹ_Runs_Only, action_resolve, action_reorder, action_shapeArabic, خالی_سوچک}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_سیٹ_Inverse, action_resolve, action_mirror, action_الٹ, خالی_سوچک}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_الٹ, action_shapeArabic, action_سیٹ_Inverse, action_resolve, action_mirror, خالی_سوچک}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, خالی_سوچک}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, خالی_سوچک}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, خالی_سوچک}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_الٹ, action_resolve, action_mirror, action_shapeArabic, action_الٹ, خالی_سوچک}}
};

ساكن ثابت uint32_t nSchemes = حجم(Schemes) / حجم(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
ساكن فراغ
resolve_ادھار_Direction(ثابت char16_t *متن, uint32_t length,
        UBiDiLevel *p_وچ_Level, UBiDiLevel *p_باہرلا_Level)
{
    بدل (*p_وچ_Level) {
        حالة UBIDI_DEFAULT_LTR:
        حالة UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_get_ادھار_Direction(متن, length));
            *p_وچ_Level = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *p_وچ_Level == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            اكسر;
        }
        افتراضي:
            *p_وچ_Level &= 1;
            اكسر;
    }
    بدل (*p_باہرلا_Level) {
        حالة UBIDI_DEFAULT_LTR:
        حالة UBIDI_DEFAULT_RTL:
            *p_باہرلا_Level = *p_وچ_Level;
            اكسر;
        افتراضي:
            *p_باہرلا_Level &= 1;
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
لبھو_Matching_Scheme(UBiDiLevel وچ_Level, UBiDiLevel باہرلا_Level,
        UBi_Di_ترتیب وچ_ترتیب, UBi_Di_ترتیب باہرلا_ترتیب)
{
    uint32_t i;
    لكل (i = 0; i < nSchemes; i++) {
        ثابت ReorderingScheme *pScheme = Schemes + i;
        إذا (وچ_Level == pScheme->وچ_Level && باہرلا_Level == pScheme->باہرلا_Level
                && وچ_ترتیب == pScheme->وچ_ترتیب && باہرلا_ترتیب == pScheme->باہرلا_ترتیب) {
            أرجع pScheme;
        }
    }
    أرجع خالی_سوچک;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ثابت char16_t *src, int32_t src_لمائی,
            char16_t *dest, int32_t dest_ناپ,
            UBiDiLevel وچ_Para_Level, UBi_Di_ترتیب وچ_ترتیب,
            UBiDiLevel باہرلا_Para_Level, UBi_Di_ترتیب باہرلا_ترتیب,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UError_کوڈ *p_Error_کوڈ)
{
    uint32_t dest_لمائی = 0;
    UBool متن_بدلیا = خطأ;
    ثابت UBiDiTransform *pOrigTransform = pBiDiTransform;
    ثابت UBiDiAction *action = خالی_سوچک;

    إذا (U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا (src == خالی_سوچک || dest == خالی_سوچک) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    CHECK_LEN(src, src_لمائی, p_Error_کوڈ);
    CHECK_LEN(dest, dest_ناپ, p_Error_کوڈ);

    إذا (pBiDiTransform == خالی_سوچک) {
        pBiDiTransform = ubiditransform_open(p_Error_کوڈ);
        إذا (U_FAILURE(*p_Error_کوڈ)) {
            أرجع 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolve_ادھار_Direction(src, src_لمائی, &وچ_Para_Level, &باہرلا_Para_Level);

    pBiDiTransform->p_سرگرم_Scheme = لبھو_Matching_Scheme(وچ_Para_Level, باہرلا_Para_Level,
            وچ_ترتیب, باہرلا_ترتیب);
    إذا (pBiDiTransform->p_سرگرم_Scheme == خالی_سوچک) {
        اذهب cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_متن_DIRECTION_نقاب;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_نقاب;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_نقاب;

    updateSrc(pBiDiTransform, src, src_لمائی, dest_ناپ > src_لمائی ? dest_ناپ : src_لمائی, p_Error_کوڈ);
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        اذهب cleanup;
    }
    إذا (pBiDiTransform->pBidi == خالی_سوچک) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, p_Error_کوڈ);
        إذا (U_FAILURE(*p_Error_کوڈ)) {
            اذهب cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_ناپ = dest_ناپ;
    pBiDiTransform->p_Dest_لمائی = &dest_لمائی;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    لكل (action = pBiDiTransform->p_سرگرم_Scheme->actions; *action && U_SUCCESS(*p_Error_کوڈ); action++) {
        إذا ((*action)(pBiDiTransform, p_Error_کوڈ)) {
            إذا (action[1] != خالی_سوچک) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_لمائی,
                        *pBiDiTransform->p_Dest_لمائی, p_Error_کوڈ);
            }
            متن_بدلیا = سچ;
        }
    }
    ubidi_سیٹ_Inverse(pBiDiTransform->pBidi, خطأ);

    إذا (!متن_بدلیا && U_SUCCESS(*p_Error_کوڈ)) {
        /* Text was not changed - just copy src to dest */
        إذا (dest_ناپ < src_لمائی) {
            *p_Error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
        } وإلا {
            u_strncpy(dest, src, src_لمائی);
            dest_لمائی = src_لمائی;
        }
    }
cleanup:
    إذا (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } وإلا {
        pBiDiTransform->dest = خالی_سوچک;
        pBiDiTransform->p_Dest_لمائی = خالی_سوچک;
        pBiDiTransform->src_لمائی = 0;
        pBiDiTransform->dest_ناپ = 0;
    }
    أرجع (U_FAILURE(*p_Error_کوڈ) && *p_Error_کوڈ != U_بفر_حد_توں_ودھ_ERROR) ? 0 : dest_لمائی;
}
