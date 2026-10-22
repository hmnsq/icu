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
#تعریف_کړه LTR                     UBIDI_LTR
#تعریف_کړه RTL                     UBIDI_RTL
#تعریف_کړه LOGICAL                 UBIDI_LOGICAL
#تعریف_کړه VISUAL                  UBIDI_VISUAL
#تعریف_کړه SHAPE_LOGICAL           U_SHAPE_متن_DIRECTION_LOGICAL
#تعریف_کړه SHAPE_VISUAL            U_SHAPE_متن_DIRECTION_VISUAL_LTR

#تعریف_کړه CHECK_LEN(STR, LEN, ERROR) UPRV_بلاک_MACRO_BEGIN { \
    إذا (LEN == 0) أرجع 0; \
    إذا (LEN < -1) { *(ERROR) = U_ILLEGAL_دلیل_ERROR; أرجع 0; } \
    إذا (LEN == -1) LEN = u_strlen(STR); \
} UPRV_بلاک_MACRO_پای_ټکی

#تعریف_کړه MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
تعريفنوع UBool (*UBiDiAction)(UBiDiTransform *, UError_کوډ *);

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
    UBiDiLevel        دننه_Level;               /* input level */
    UBi_Di_ترتیب        دننه_ترتیب;               /* input order */
    UBiDiLevel        بهر_Level;              /* output level */
    UBi_Di_ترتیب        بهر_ترتیب;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        بنسټ_Level;             /* paragraph level to be used with setPara */
    ثابت UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

بنية UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ثابت ReorderingScheme  *p_فعال_Scheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_اوږدتیا;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_اندازه;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_اندازه;           /* output text capacity */
    uint32_t                *p_Dest_اوږدتیا;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UError_کوډ *p_Error_کوډ)
{
    UBiDiTransform *pBiDiTransform = تش_اشاره;
    إذا (U_SUCCESS(*p_Error_کوډ)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, حجم(UBiDiTransform));
        إذا (pBiDiTransform == تش_اشاره) {
            *p_Error_کوډ = U_حافظه_ویش_ERROR;
        }
    }
    أرجع pBiDiTransform;
}

U_CAPI فراغ U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    إذا (pBiDiTransform != تش_اشاره) {
        إذا (pBiDiTransform->pBidi != تش_اشاره) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        إذا (pBiDiTransform->src != تش_اشاره) {
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
action_resolve(UBiDiTransform *pTransform, UError_کوډ *p_Error_کوډ)
{
    ubidi_سېټ_Para(pTransform->pBidi, pTransform->src, pTransform->src_اوږدتیا,
            pTransform->p_فعال_Scheme->بنسټ_Level, تش_اشاره, p_Error_کوډ);
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
action_reorder(UBiDiTransform *pTransform, UError_کوډ *p_Error_کوډ)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_اندازه,
            static_cast<uint16_t>(pTransform->reorderingOptions), p_Error_کوډ);

    *pTransform->p_Dest_اوږدتیا = pTransform->src_اوږدتیا;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    أرجع ریښتیا;
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
action_سېټ_Inverse(UBiDiTransform *pTransform, UError_کوډ *p_Error_کوډ)
{
    (فراغ)p_Error_کوډ;
    ubidi_سېټ_Inverse(pTransform->pBidi, ریښتیا);
    ubidi_سېټ_Reordering_ډېرپېښ(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_مستقیم);
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
action_سېټ_Runs_Only(UBiDiTransform *pTransform, UError_کوډ *p_Error_کوډ)
{
    (فراغ)p_Error_کوډ;
    ubidi_سېټ_Reordering_ډېرپېښ(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
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
action_سرچپه(UBiDiTransform *pTransform, UError_کوډ *p_Error_کوډ)
{
    ubidi_write_سرچپه(pTransform->src, pTransform->src_اوږدتیا,
            pTransform->dest, pTransform->dest_اندازه,
            UBIDI_REORDER_DEFAULT, p_Error_کوډ);
    *pTransform->p_Dest_اوږدتیا = pTransform->src_اوږدتیا;
    أرجع ریښتیا;
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
updateSrc(UBiDiTransform *pTransform, ثابت char16_t *newSrc, uint32_t new_اوږدتیا,
        uint32_t new_اندازه, UError_کوډ *p_Error_کوډ)
{
    إذا (new_اندازه < new_اوږدتیا) {
        *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
        أرجع;
    }
    إذا (new_اندازه > pTransform->src_اندازه) {
        new_اندازه += 50; // allocate slightly more than needed right now
        إذا (pTransform->src != تش_اشاره) {
            uprv_free(pTransform->src);
            pTransform->src = تش_اشاره;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_اندازه * حجم(char16_t)));
        إذا (pTransform->src == تش_اشاره) {
            *p_Error_کوډ = U_حافظه_ویش_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            أرجع;
        }
        pTransform->src_اندازه = new_اندازه;
    }
    u_strncpy(pTransform->src, newSrc, new_اوږدتیا);
    pTransform->src_اوږدتیا = u_terminateUChars(pTransform->src,
    		pTransform->src_اندازه, new_اوږدتیا, p_Error_کوډ);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
ساكن فراغ
doShape(UBiDiTransform *pTransform, uint32_t options, UError_کوډ *p_Error_کوډ)
{
    *pTransform->p_Dest_اوږدتیا = u_shapeArabic(pTransform->src,
            pTransform->src_اوږدتیا, pTransform->dest, pTransform->dest_اندازه,
            options, p_Error_کوډ);
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
action_shapeArabic(UBiDiTransform *pTransform, UError_کوډ *p_Error_کوډ)
{
    إذا ((pTransform->letters | pTransform->digits) == 0) {
        أرجع خطأ;
    }
    إذا (pTransform->p_فعال_Scheme->lettersDir == pTransform->p_فعال_Scheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->p_فعال_Scheme->lettersDir,
                p_Error_کوډ);
    } وإلا {
        doShape(pTransform, pTransform->digits | pTransform->p_فعال_Scheme->digitsDir, p_Error_کوډ);
        إذا (U_SUCCESS(*p_Error_کوډ)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_اوږدتیا,
                    *pTransform->p_Dest_اوږدتیا, p_Error_کوډ);
            doShape(pTransform, pTransform->letters | pTransform->p_فعال_Scheme->lettersDir,
                    p_Error_کوډ);
        }
    }
    أرجع ریښتیا;
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
action_mirror(UBiDiTransform *pTransform, UError_کوډ *p_Error_کوډ)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    إذا (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        أرجع خطأ;
    }
    افعل {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_بل(pTransform->src, i, pTransform->src_اوږدتیا, c); 
        إذا (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_اوږدتیا(c);
        إذا (j + k <= pTransform->dest_اندازه) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } وإلا {
            j += k;
        }
    } طالما (i < pTransform->src_اوږدتیا);
    
    *pTransform->p_Dest_اوږدتیا = j;
    إذا (pTransform->dest_اندازه < j) {
        *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
        أرجع خطأ;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    أرجع ریښتیا;
}

/**
 * All possible reordering schemes.
 *
 */
ساكن ثابت ReorderingScheme Schemes[] =
{
    /* 0: Logical LTR => Visual LTR */
    {LTR, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, تش_اشاره}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, تش_اشاره}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_سرچپه, تش_اشاره}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_سرچپه, تش_اشاره}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_سېټ_Inverse, action_resolve, action_reorder, تش_اشاره}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_سرچپه, action_shapeArabic, action_سېټ_Inverse, action_resolve, action_reorder, تش_اشاره}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_سېټ_Inverse, action_resolve, action_reorder, action_shapeArabic, تش_اشاره}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_سرچپه, action_سېټ_Inverse, action_resolve, action_reorder, action_shapeArabic, تش_اشاره}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_سېټ_Runs_Only, action_resolve, action_reorder, تش_اشاره}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_سېټ_Runs_Only, action_resolve, action_reorder, action_shapeArabic, تش_اشاره}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_سېټ_Inverse, action_resolve, action_mirror, action_سرچپه, تش_اشاره}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_سرچپه, action_shapeArabic, action_سېټ_Inverse, action_resolve, action_mirror, تش_اشاره}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, تش_اشاره}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, تش_اشاره}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, تش_اشاره}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_سرچپه, action_resolve, action_mirror, action_shapeArabic, action_سرچپه, تش_اشاره}}
};

ساكن ثابت uint32_t nSchemes = حجم(Schemes) / حجم(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
ساكن فراغ
resolve_بنسټ_Direction(ثابت char16_t *متن, uint32_t length,
        UBiDiLevel *p_دننه_Level, UBiDiLevel *p_بهر_Level)
{
    بدل (*p_دننه_Level) {
        حالة UBIDI_DEFAULT_LTR:
        حالة UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_get_بنسټ_Direction(متن, length));
            *p_دننه_Level = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *p_دننه_Level == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            اكسر;
        }
        افتراضي:
            *p_دننه_Level &= 1;
            اكسر;
    }
    بدل (*p_بهر_Level) {
        حالة UBIDI_DEFAULT_LTR:
        حالة UBIDI_DEFAULT_RTL:
            *p_بهر_Level = *p_دننه_Level;
            اكسر;
        افتراضي:
            *p_بهر_Level &= 1;
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
پیدا_کړه_Matching_Scheme(UBiDiLevel دننه_Level, UBiDiLevel بهر_Level,
        UBi_Di_ترتیب دننه_ترتیب, UBi_Di_ترتیب بهر_ترتیب)
{
    uint32_t i;
    لكل (i = 0; i < nSchemes; i++) {
        ثابت ReorderingScheme *pScheme = Schemes + i;
        إذا (دننه_Level == pScheme->دننه_Level && بهر_Level == pScheme->بهر_Level
                && دننه_ترتیب == pScheme->دننه_ترتیب && بهر_ترتیب == pScheme->بهر_ترتیب) {
            أرجع pScheme;
        }
    }
    أرجع تش_اشاره;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ثابت char16_t *src, int32_t src_اوږدتیا,
            char16_t *dest, int32_t dest_اندازه,
            UBiDiLevel دننه_Para_Level, UBi_Di_ترتیب دننه_ترتیب,
            UBiDiLevel بهر_Para_Level, UBi_Di_ترتیب بهر_ترتیب,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UError_کوډ *p_Error_کوډ)
{
    uint32_t dest_اوږدتیا = 0;
    UBool متن_بدل_شوی = خطأ;
    ثابت UBiDiTransform *pOrigTransform = pBiDiTransform;
    ثابت UBiDiAction *action = تش_اشاره;

    إذا (U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا (src == تش_اشاره || dest == تش_اشاره) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    CHECK_LEN(src, src_اوږدتیا, p_Error_کوډ);
    CHECK_LEN(dest, dest_اندازه, p_Error_کوډ);

    إذا (pBiDiTransform == تش_اشاره) {
        pBiDiTransform = ubiditransform_open(p_Error_کوډ);
        إذا (U_FAILURE(*p_Error_کوډ)) {
            أرجع 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolve_بنسټ_Direction(src, src_اوږدتیا, &دننه_Para_Level, &بهر_Para_Level);

    pBiDiTransform->p_فعال_Scheme = پیدا_کړه_Matching_Scheme(دننه_Para_Level, بهر_Para_Level,
            دننه_ترتیب, بهر_ترتیب);
    إذا (pBiDiTransform->p_فعال_Scheme == تش_اشاره) {
        اذهب cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_متن_DIRECTION_ماسک;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_ماسک;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_ماسک;

    updateSrc(pBiDiTransform, src, src_اوږدتیا, dest_اندازه > src_اوږدتیا ? dest_اندازه : src_اوږدتیا, p_Error_کوډ);
    إذا (U_FAILURE(*p_Error_کوډ)) {
        اذهب cleanup;
    }
    إذا (pBiDiTransform->pBidi == تش_اشاره) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, p_Error_کوډ);
        إذا (U_FAILURE(*p_Error_کوډ)) {
            اذهب cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_اندازه = dest_اندازه;
    pBiDiTransform->p_Dest_اوږدتیا = &dest_اوږدتیا;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    لكل (action = pBiDiTransform->p_فعال_Scheme->actions; *action && U_SUCCESS(*p_Error_کوډ); action++) {
        إذا ((*action)(pBiDiTransform, p_Error_کوډ)) {
            إذا (action[1] != تش_اشاره) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_اوږدتیا,
                        *pBiDiTransform->p_Dest_اوږدتیا, p_Error_کوډ);
            }
            متن_بدل_شوی = ریښتیا;
        }
    }
    ubidi_سېټ_Inverse(pBiDiTransform->pBidi, خطأ);

    إذا (!متن_بدل_شوی && U_SUCCESS(*p_Error_کوډ)) {
        /* Text was not changed - just copy src to dest */
        إذا (dest_اندازه < src_اوږدتیا) {
            *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
        } وإلا {
            u_strncpy(dest, src, src_اوږدتیا);
            dest_اوږدتیا = src_اوږدتیا;
        }
    }
cleanup:
    إذا (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } وإلا {
        pBiDiTransform->dest = تش_اشاره;
        pBiDiTransform->p_Dest_اوږدتیا = تش_اشاره;
        pBiDiTransform->src_اوږدتیا = 0;
        pBiDiTransform->dest_اندازه = 0;
    }
    أرجع (U_FAILURE(*p_Error_کوډ) && *p_Error_کوډ != U_بفر_ډکېدنه_ERROR) ? 0 : dest_اوږدتیا;
}
