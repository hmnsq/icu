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
#تعریف_کرو LTR                     UBIDI_LTR
#تعریف_کرو RTL                     UBIDI_RTL
#تعریف_کرو LOGICAL                 UBIDI_LOGICAL
#تعریف_کرو VISUAL                  UBIDI_VISUAL
#تعریف_کرو SHAPE_LOGICAL           U_SHAPE_متن_DIRECTION_LOGICAL
#تعریف_کرو SHAPE_VISUAL            U_SHAPE_متن_DIRECTION_VISUAL_LTR

#تعریف_کرو CHECK_LEN(STR, LEN, ERROR) UPRV_بلاک_MACRO_BEGIN { \
    اگر (LEN == 0) واپس 0; \
    اگر (LEN < -1) { *(ERROR) = U_ILLEGAL_دلیل_ERROR; واپس 0; } \
    اگر (LEN == -1) LEN = u_strlen(STR); \
} UPRV_بلاک_MACRO_انتہا

#تعریف_کرو MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
قسمتعریف UBool (*UBiDiAction)(UBiDiTransform *, UError_رمز *);

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
قسمتعریف ساخت {
    UBiDiLevel        اندر_Level;               /* input level */
    UBi_Di_ترتیب        اندر_ترتیب;               /* input order */
    UBiDiLevel        برآمد_Level;              /* output level */
    UBi_Di_ترتیب        برآمد_ترتیب;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        بنیاد_Level;             /* paragraph level to be used with setPara */
    ثابت UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

ساخت UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ثابت ReorderingScheme  *p_فعال_Scheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_لمبائی;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_سائز;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_سائز;           /* output text capacity */
    uint32_t                *p_Dest_لمبائی;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UError_رمز *p_Error_رمز)
{
    UBiDiTransform *pBiDiTransform = خالی_اشاریہ;
    اگر (U_SUCCESS(*p_Error_رمز)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, حجم(UBiDiTransform));
        اگر (pBiDiTransform == خالی_اشاریہ) {
            *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        }
    }
    واپس pBiDiTransform;
}

U_CAPI خالی U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    اگر (pBiDiTransform != خالی_اشاریہ) {
        اگر (pBiDiTransform->pBidi != خالی_اشاریہ) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        اگر (pBiDiTransform->src != خالی_اشاریہ) {
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
ساکن UBool
action_resolve(UBiDiTransform *pTransform, UError_رمز *p_Error_رمز)
{
    ubidi_سیٹ_Para(pTransform->pBidi, pTransform->src, pTransform->src_لمبائی,
            pTransform->p_فعال_Scheme->بنیاد_Level, خالی_اشاریہ, p_Error_رمز);
    واپس جھوٹ;
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
ساکن UBool
action_reorder(UBiDiTransform *pTransform, UError_رمز *p_Error_رمز)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_سائز,
            static_cast<uint16_t>(pTransform->reorderingOptions), p_Error_رمز);

    *pTransform->p_Dest_لمبائی = pTransform->src_لمبائی;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    واپس سچ;
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
ساکن UBool
action_سیٹ_Inverse(UBiDiTransform *pTransform, UError_رمز *p_Error_رمز)
{
    (خالی)p_Error_رمز;
    ubidi_سیٹ_Inverse(pTransform->pBidi, سچ);
    ubidi_سیٹ_Reordering_منوال(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_براہ_راست);
    واپس جھوٹ;
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
ساکن UBool
action_سیٹ_Runs_Only(UBiDiTransform *pTransform, UError_رمز *p_Error_رمز)
{
    (خالی)p_Error_رمز;
    ubidi_سیٹ_Reordering_منوال(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    واپس جھوٹ;
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
ساکن UBool
action_الٹ(UBiDiTransform *pTransform, UError_رمز *p_Error_رمز)
{
    ubidi_write_الٹ(pTransform->src, pTransform->src_لمبائی,
            pTransform->dest, pTransform->dest_سائز,
            UBIDI_REORDER_DEFAULT, p_Error_رمز);
    *pTransform->p_Dest_لمبائی = pTransform->src_لمبائی;
    واپس سچ;
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
ساکن خالی
updateSrc(UBiDiTransform *pTransform, ثابت char16_t *newSrc, uint32_t new_لمبائی,
        uint32_t new_سائز, UError_رمز *p_Error_رمز)
{
    اگر (new_سائز < new_لمبائی) {
        *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        واپس;
    }
    اگر (new_سائز > pTransform->src_سائز) {
        new_سائز += 50; // allocate slightly more than needed right now
        اگر (pTransform->src != خالی_اشاریہ) {
            uprv_free(pTransform->src);
            pTransform->src = خالی_اشاریہ;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_سائز * حجم(char16_t)));
        اگر (pTransform->src == خالی_اشاریہ) {
            *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            واپس;
        }
        pTransform->src_سائز = new_سائز;
    }
    u_strncpy(pTransform->src, newSrc, new_لمبائی);
    pTransform->src_لمبائی = u_terminateUChars(pTransform->src,
    		pTransform->src_سائز, new_لمبائی, p_Error_رمز);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
ساکن خالی
doShape(UBiDiTransform *pTransform, uint32_t options, UError_رمز *p_Error_رمز)
{
    *pTransform->p_Dest_لمبائی = u_shapeArabic(pTransform->src,
            pTransform->src_لمبائی, pTransform->dest, pTransform->dest_سائز,
            options, p_Error_رمز);
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
ساکن UBool
action_shapeArabic(UBiDiTransform *pTransform, UError_رمز *p_Error_رمز)
{
    اگر ((pTransform->letters | pTransform->digits) == 0) {
        واپس جھوٹ;
    }
    اگر (pTransform->p_فعال_Scheme->lettersDir == pTransform->p_فعال_Scheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->p_فعال_Scheme->lettersDir,
                p_Error_رمز);
    } ورنہ {
        doShape(pTransform, pTransform->digits | pTransform->p_فعال_Scheme->digitsDir, p_Error_رمز);
        اگر (U_SUCCESS(*p_Error_رمز)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_لمبائی,
                    *pTransform->p_Dest_لمبائی, p_Error_رمز);
            doShape(pTransform, pTransform->letters | pTransform->p_فعال_Scheme->lettersDir,
                    p_Error_رمز);
        }
    }
    واپس سچ;
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
ساکن UBool
action_mirror(UBiDiTransform *pTransform, UError_رمز *p_Error_رمز)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    اگر (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        واپس جھوٹ;
    }
    کرو {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_اگلا(pTransform->src, i, pTransform->src_لمبائی, c); 
        اگر (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_لمبائی(c);
        اگر (j + k <= pTransform->dest_سائز) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } ورنہ {
            j += k;
        }
    } جب (i < pTransform->src_لمبائی);
    
    *pTransform->p_Dest_لمبائی = j;
    اگر (pTransform->dest_سائز < j) {
        *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        واپس جھوٹ;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    واپس سچ;
}

/**
 * All possible reordering schemes.
 *
 */
ساکن ثابت ReorderingScheme Schemes[] =
{
    /* 0: Logical LTR => Visual LTR */
    {LTR, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, خالی_اشاریہ}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, خالی_اشاریہ}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_الٹ, خالی_اشاریہ}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_الٹ, خالی_اشاریہ}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_سیٹ_Inverse, action_resolve, action_reorder, خالی_اشاریہ}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_الٹ, action_shapeArabic, action_سیٹ_Inverse, action_resolve, action_reorder, خالی_اشاریہ}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_سیٹ_Inverse, action_resolve, action_reorder, action_shapeArabic, خالی_اشاریہ}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_الٹ, action_سیٹ_Inverse, action_resolve, action_reorder, action_shapeArabic, خالی_اشاریہ}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_سیٹ_Runs_Only, action_resolve, action_reorder, خالی_اشاریہ}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_سیٹ_Runs_Only, action_resolve, action_reorder, action_shapeArabic, خالی_اشاریہ}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_سیٹ_Inverse, action_resolve, action_mirror, action_الٹ, خالی_اشاریہ}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_الٹ, action_shapeArabic, action_سیٹ_Inverse, action_resolve, action_mirror, خالی_اشاریہ}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, خالی_اشاریہ}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, خالی_اشاریہ}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, خالی_اشاریہ}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_الٹ, action_resolve, action_mirror, action_shapeArabic, action_الٹ, خالی_اشاریہ}}
};

ساکن ثابت uint32_t nSchemes = حجم(Schemes) / حجم(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
ساکن خالی
resolve_بنیاد_Direction(ثابت char16_t *متن, uint32_t length,
        UBiDiLevel *p_اندر_Level, UBiDiLevel *p_برآمد_Level)
{
    انتخاب (*p_اندر_Level) {
        حالت UBIDI_DEFAULT_LTR:
        حالت UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_get_بنیاد_Direction(متن, length));
            *p_اندر_Level = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *p_اندر_Level == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            توڑو;
        }
        طےشدہ:
            *p_اندر_Level &= 1;
            توڑو;
    }
    انتخاب (*p_برآمد_Level) {
        حالت UBIDI_DEFAULT_LTR:
        حالت UBIDI_DEFAULT_RTL:
            *p_برآمد_Level = *p_اندر_Level;
            توڑو;
        طےشدہ:
            *p_برآمد_Level &= 1;
            توڑو;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
ساکن ثابت ReorderingScheme*
ڈھونڈو_Matching_Scheme(UBiDiLevel اندر_Level, UBiDiLevel برآمد_Level,
        UBi_Di_ترتیب اندر_ترتیب, UBi_Di_ترتیب برآمد_ترتیب)
{
    uint32_t i;
    برائے (i = 0; i < nSchemes; i++) {
        ثابت ReorderingScheme *pScheme = Schemes + i;
        اگر (اندر_Level == pScheme->اندر_Level && برآمد_Level == pScheme->برآمد_Level
                && اندر_ترتیب == pScheme->اندر_ترتیب && برآمد_ترتیب == pScheme->برآمد_ترتیب) {
            واپس pScheme;
        }
    }
    واپس خالی_اشاریہ;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ثابت char16_t *src, int32_t src_لمبائی,
            char16_t *dest, int32_t dest_سائز,
            UBiDiLevel اندر_Para_Level, UBi_Di_ترتیب اندر_ترتیب,
            UBiDiLevel برآمد_Para_Level, UBi_Di_ترتیب برآمد_ترتیب,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UError_رمز *p_Error_رمز)
{
    uint32_t dest_لمبائی = 0;
    UBool متن_بدلا = جھوٹ;
    ثابت UBiDiTransform *pOrigTransform = pBiDiTransform;
    ثابت UBiDiAction *action = خالی_اشاریہ;

    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر (src == خالی_اشاریہ || dest == خالی_اشاریہ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    CHECK_LEN(src, src_لمبائی, p_Error_رمز);
    CHECK_LEN(dest, dest_سائز, p_Error_رمز);

    اگر (pBiDiTransform == خالی_اشاریہ) {
        pBiDiTransform = ubiditransform_open(p_Error_رمز);
        اگر (U_FAILURE(*p_Error_رمز)) {
            واپس 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolve_بنیاد_Direction(src, src_لمبائی, &اندر_Para_Level, &برآمد_Para_Level);

    pBiDiTransform->p_فعال_Scheme = ڈھونڈو_Matching_Scheme(اندر_Para_Level, برآمد_Para_Level,
            اندر_ترتیب, برآمد_ترتیب);
    اگر (pBiDiTransform->p_فعال_Scheme == خالی_اشاریہ) {
        جاؤ cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_متن_DIRECTION_نقاب;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_نقاب;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_نقاب;

    updateSrc(pBiDiTransform, src, src_لمبائی, dest_سائز > src_لمبائی ? dest_سائز : src_لمبائی, p_Error_رمز);
    اگر (U_FAILURE(*p_Error_رمز)) {
        جاؤ cleanup;
    }
    اگر (pBiDiTransform->pBidi == خالی_اشاریہ) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, p_Error_رمز);
        اگر (U_FAILURE(*p_Error_رمز)) {
            جاؤ cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_سائز = dest_سائز;
    pBiDiTransform->p_Dest_لمبائی = &dest_لمبائی;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    برائے (action = pBiDiTransform->p_فعال_Scheme->actions; *action && U_SUCCESS(*p_Error_رمز); action++) {
        اگر ((*action)(pBiDiTransform, p_Error_رمز)) {
            اگر (action[1] != خالی_اشاریہ) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_لمبائی,
                        *pBiDiTransform->p_Dest_لمبائی, p_Error_رمز);
            }
            متن_بدلا = سچ;
        }
    }
    ubidi_سیٹ_Inverse(pBiDiTransform->pBidi, جھوٹ);

    اگر (!متن_بدلا && U_SUCCESS(*p_Error_رمز)) {
        /* Text was not changed - just copy src to dest */
        اگر (dest_سائز < src_لمبائی) {
            *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        } ورنہ {
            u_strncpy(dest, src, src_لمبائی);
            dest_لمبائی = src_لمبائی;
        }
    }
cleanup:
    اگر (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } ورنہ {
        pBiDiTransform->dest = خالی_اشاریہ;
        pBiDiTransform->p_Dest_لمبائی = خالی_اشاریہ;
        pBiDiTransform->src_لمبائی = 0;
        pBiDiTransform->dest_سائز = 0;
    }
    واپس (U_FAILURE(*p_Error_رمز) && *p_Error_رمز != U_بفر_حد_سے_تجاوز_ERROR) ? 0 : dest_لمبائی;
}
