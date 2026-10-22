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

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ubidi.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ushape.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "unicode/ubiditransform.h"

/* Some convenience defines */
#परिभाषय LTR                     UBIDI_LTR
#परिभाषय RTL                     UBIDI_RTL
#परिभाषय LOGICAL                 UBIDI_LOGICAL
#परिभाषय VISUAL                  UBIDI_VISUAL
#परिभाषय SHAPE_LOGICAL           U_SHAPE_TEXT_DIRECTION_LOGICAL
#परिभाषय SHAPE_VISUAL            U_SHAPE_TEXT_DIRECTION_VISUAL_LTR

#परिभाषय CHECK_LEN(STR, LEN, ERROR) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि (LEN == 0) प्रत्यागम 0; \
    यदि (LEN < -1) { *(ERROR) = U_ILLEGAL_तर्कः_ERROR; प्रत्यागम 0; } \
    यदि (LEN == -1) LEN = u_strlen(STR); \
} UPRV_खण्डम्_MACRO_अन्तः

#परिभाषय MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
प्रकारनिर्देश UBool (*UBiDiAction)(UBiDiTransform *, UError_संहिता *);

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
प्रकारनिर्देश संरचना {
    UBiDiLevel        अन्तर्_Level;               /* input level */
    UBi_Di_क्रमः        अन्तर्_क्रमः;               /* input order */
    UBiDiLevel        बहिर्गमनम्_Level;              /* output level */
    UBi_Di_क्रमः        बहिर्गमनम्_क्रमः;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        आधारः_Level;             /* paragraph level to be used with setPara */
    ध्रुव UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

संरचना UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    ध्रुव ReorderingScheme  *p_सक्रिय_Scheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_दीर्घता;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_आकार;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_आकार;           /* output text capacity */
    uint32_t                *p_Dest_दीर्घता;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UError_संहिता *p_Error_संहिता)
{
    UBiDiTransform *pBiDiTransform = शून्यसूचकः;
    यदि (U_SUCCESS(*p_Error_संहिता)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, परिमाण(UBiDiTransform));
        यदि (pBiDiTransform == शून्यसूचकः) {
            *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        }
    }
    प्रत्यागम pBiDiTransform;
}

U_CAPI शून्य U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    यदि (pBiDiTransform != शून्यसूचकः) {
        यदि (pBiDiTransform->pBidi != शून्यसूचकः) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        यदि (pBiDiTransform->src != शून्यसूचकः) {
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
स्थिर UBool
action_resolve(UBiDiTransform *pTransform, UError_संहिता *p_Error_संहिता)
{
    ubidi_समुच्चयः_Para(pTransform->pBidi, pTransform->src, pTransform->src_दीर्घता,
            pTransform->p_सक्रिय_Scheme->आधारः_Level, शून्यसूचकः, p_Error_संहिता);
    प्रत्यागम असत्य;
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
स्थिर UBool
action_reorder(UBiDiTransform *pTransform, UError_संहिता *p_Error_संहिता)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_आकार,
            static_cast<uint16_t>(pTransform->reorderingOptions), p_Error_संहिता);

    *pTransform->p_Dest_दीर्घता = pTransform->src_दीर्घता;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    प्रत्यागम सत्य;
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
स्थिर UBool
action_समुच्चयः_Inverse(UBiDiTransform *pTransform, UError_संहिता *p_Error_संहिता)
{
    (शून्य)p_Error_संहिता;
    ubidi_समुच्चयः_Inverse(pTransform->pBidi, सत्य);
    ubidi_समुच्चयः_Reordering_बहुलकम्(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_प्रत्यक्षम्);
    प्रत्यागम असत्य;
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
स्थिर UBool
action_समुच्चयः_Runs_Only(UBiDiTransform *pTransform, UError_संहिता *p_Error_संहिता)
{
    (शून्य)p_Error_संहिता;
    ubidi_समुच्चयः_Reordering_बहुलकम्(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    प्रत्यागम असत्य;
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
स्थिर UBool
action_reverse(UBiDiTransform *pTransform, UError_संहिता *p_Error_संहिता)
{
    ubidi_writeReverse(pTransform->src, pTransform->src_दीर्घता,
            pTransform->dest, pTransform->dest_आकार,
            UBIDI_REORDER_DEFAULT, p_Error_संहिता);
    *pTransform->p_Dest_दीर्घता = pTransform->src_दीर्घता;
    प्रत्यागम सत्य;
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
स्थिर शून्य
updateSrc(UBiDiTransform *pTransform, ध्रुव char16_t *newSrc, uint32_t new_दीर्घता,
        uint32_t new_आकार, UError_संहिता *p_Error_संहिता)
{
    यदि (new_आकार < new_दीर्घता) {
        *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम;
    }
    यदि (new_आकार > pTransform->src_आकार) {
        new_आकार += 50; // allocate slightly more than needed right now
        यदि (pTransform->src != शून्यसूचकः) {
            uprv_free(pTransform->src);
            pTransform->src = शून्यसूचकः;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_आकार * परिमाण(char16_t)));
        यदि (pTransform->src == शून्यसूचकः) {
            *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            प्रत्यागम;
        }
        pTransform->src_आकार = new_आकार;
    }
    u_strncpy(pTransform->src, newSrc, new_दीर्घता);
    pTransform->src_दीर्घता = u_terminateUChars(pTransform->src,
    		pTransform->src_आकार, new_दीर्घता, p_Error_संहिता);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
स्थिर शून्य
doShape(UBiDiTransform *pTransform, uint32_t options, UError_संहिता *p_Error_संहिता)
{
    *pTransform->p_Dest_दीर्घता = u_shapeArabic(pTransform->src,
            pTransform->src_दीर्घता, pTransform->dest, pTransform->dest_आकार,
            options, p_Error_संहिता);
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
स्थिर UBool
action_shapeArabic(UBiDiTransform *pTransform, UError_संहिता *p_Error_संहिता)
{
    यदि ((pTransform->letters | pTransform->digits) == 0) {
        प्रत्यागम असत्य;
    }
    यदि (pTransform->p_सक्रिय_Scheme->lettersDir == pTransform->p_सक्रिय_Scheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->p_सक्रिय_Scheme->lettersDir,
                p_Error_संहिता);
    } अन्यथा {
        doShape(pTransform, pTransform->digits | pTransform->p_सक्रिय_Scheme->digitsDir, p_Error_संहिता);
        यदि (U_SUCCESS(*p_Error_संहिता)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_दीर्घता,
                    *pTransform->p_Dest_दीर्घता, p_Error_संहिता);
            doShape(pTransform, pTransform->letters | pTransform->p_सक्रिय_Scheme->lettersDir,
                    p_Error_संहिता);
        }
    }
    प्रत्यागम सत्य;
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
स्थिर UBool
action_mirror(UBiDiTransform *pTransform, UError_संहिता *p_Error_संहिता)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    यदि (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        प्रत्यागम असत्य;
    }
    कुरु {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_अग्र(pTransform->src, i, pTransform->src_दीर्घता, c); 
        यदि (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_दीर्घता(c);
        यदि (j + k <= pTransform->dest_आकार) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } अन्यथा {
            j += k;
        }
    } यावत् (i < pTransform->src_दीर्घता);
    
    *pTransform->p_Dest_दीर्घता = j;
    यदि (pTransform->dest_आकार < j) {
        *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम असत्य;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    प्रत्यागम सत्य;
}

/**
 * All possible reordering schemes.
 *
 */
स्थिर ध्रुव ReorderingScheme Schemes[] =
{
    /* 0: Logical LTR => Visual LTR */
    {LTR, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, शून्यसूचकः}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, शून्यसूचकः}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_reverse, शून्यसूचकः}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_reverse, शून्यसूचकः}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_समुच्चयः_Inverse, action_resolve, action_reorder, शून्यसूचकः}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_reverse, action_shapeArabic, action_समुच्चयः_Inverse, action_resolve, action_reorder, शून्यसूचकः}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_समुच्चयः_Inverse, action_resolve, action_reorder, action_shapeArabic, शून्यसूचकः}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_reverse, action_समुच्चयः_Inverse, action_resolve, action_reorder, action_shapeArabic, शून्यसूचकः}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_समुच्चयः_Runs_Only, action_resolve, action_reorder, शून्यसूचकः}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_समुच्चयः_Runs_Only, action_resolve, action_reorder, action_shapeArabic, शून्यसूचकः}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_समुच्चयः_Inverse, action_resolve, action_mirror, action_reverse, शून्यसूचकः}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_reverse, action_shapeArabic, action_समुच्चयः_Inverse, action_resolve, action_mirror, शून्यसूचकः}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, शून्यसूचकः}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, शून्यसूचकः}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, शून्यसूचकः}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_reverse, action_resolve, action_mirror, action_shapeArabic, action_reverse, शून्यसूचकः}}
};

स्थिर ध्रुव uint32_t nSchemes = परिमाण(Schemes) / परिमाण(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
स्थिर शून्य
resolve_आधारः_Direction(ध्रुव char16_t *text, uint32_t length,
        UBiDiLevel *p_अन्तर्_Level, UBiDiLevel *p_बहिर्गमनम्_Level)
{
    विकल्प (*p_अन्तर्_Level) {
        पक्ष UBIDI_DEFAULT_LTR:
        पक्ष UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_get_आधारः_Direction(text, length));
            *p_अन्तर्_Level = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *p_अन्तर्_Level == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            भङ्ग;
        }
        स्वभाव:
            *p_अन्तर्_Level &= 1;
            भङ्ग;
    }
    विकल्प (*p_बहिर्गमनम्_Level) {
        पक्ष UBIDI_DEFAULT_LTR:
        पक्ष UBIDI_DEFAULT_RTL:
            *p_बहिर्गमनम्_Level = *p_अन्तर्_Level;
            भङ्ग;
        स्वभाव:
            *p_बहिर्गमनम्_Level &= 1;
            भङ्ग;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
स्थिर ध्रुव ReorderingScheme*
findMatchingScheme(UBiDiLevel अन्तर्_Level, UBiDiLevel बहिर्गमनम्_Level,
        UBi_Di_क्रमः अन्तर्_क्रमः, UBi_Di_क्रमः बहिर्गमनम्_क्रमः)
{
    uint32_t i;
    प्रति (i = 0; i < nSchemes; i++) {
        ध्रुव ReorderingScheme *pScheme = Schemes + i;
        यदि (अन्तर्_Level == pScheme->अन्तर्_Level && बहिर्गमनम्_Level == pScheme->बहिर्गमनम्_Level
                && अन्तर्_क्रमः == pScheme->अन्तर्_क्रमः && बहिर्गमनम्_क्रमः == pScheme->बहिर्गमनम्_क्रमः) {
            प्रत्यागम pScheme;
        }
    }
    प्रत्यागम शून्यसूचकः;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            ध्रुव char16_t *src, int32_t src_दीर्घता,
            char16_t *dest, int32_t dest_आकार,
            UBiDiLevel अन्तर्_Para_Level, UBi_Di_क्रमः अन्तर्_क्रमः,
            UBiDiLevel बहिर्गमनम्_Para_Level, UBi_Di_क्रमः बहिर्गमनम्_क्रमः,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UError_संहिता *p_Error_संहिता)
{
    uint32_t dest_दीर्घता = 0;
    UBool text_परिवर्तितम् = असत्य;
    ध्रुव UBiDiTransform *pOrigTransform = pBiDiTransform;
    ध्रुव UBiDiAction *action = शून्यसूचकः;

    यदि (U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि (src == शून्यसूचकः || dest == शून्यसूचकः) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    CHECK_LEN(src, src_दीर्घता, p_Error_संहिता);
    CHECK_LEN(dest, dest_आकार, p_Error_संहिता);

    यदि (pBiDiTransform == शून्यसूचकः) {
        pBiDiTransform = ubiditransform_open(p_Error_संहिता);
        यदि (U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolve_आधारः_Direction(src, src_दीर्घता, &अन्तर्_Para_Level, &बहिर्गमनम्_Para_Level);

    pBiDiTransform->p_सक्रिय_Scheme = findMatchingScheme(अन्तर्_Para_Level, बहिर्गमनम्_Para_Level,
            अन्तर्_क्रमः, बहिर्गमनम्_क्रमः);
    यदि (pBiDiTransform->p_सक्रिय_Scheme == शून्यसूचकः) {
        गच्छ cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_TEXT_DIRECTION_आवरणम्;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_आवरणम्;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_आवरणम्;

    updateSrc(pBiDiTransform, src, src_दीर्घता, dest_आकार > src_दीर्घता ? dest_आकार : src_दीर्घता, p_Error_संहिता);
    यदि (U_FAILURE(*p_Error_संहिता)) {
        गच्छ cleanup;
    }
    यदि (pBiDiTransform->pBidi == शून्यसूचकः) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, p_Error_संहिता);
        यदि (U_FAILURE(*p_Error_संहिता)) {
            गच्छ cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_आकार = dest_आकार;
    pBiDiTransform->p_Dest_दीर्घता = &dest_दीर्घता;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    प्रति (action = pBiDiTransform->p_सक्रिय_Scheme->actions; *action && U_SUCCESS(*p_Error_संहिता); action++) {
        यदि ((*action)(pBiDiTransform, p_Error_संहिता)) {
            यदि (action[1] != शून्यसूचकः) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_दीर्घता,
                        *pBiDiTransform->p_Dest_दीर्घता, p_Error_संहिता);
            }
            text_परिवर्तितम् = सत्य;
        }
    }
    ubidi_समुच्चयः_Inverse(pBiDiTransform->pBidi, असत्य);

    यदि (!text_परिवर्तितम् && U_SUCCESS(*p_Error_संहिता)) {
        /* Text was not changed - just copy src to dest */
        यदि (dest_आकार < src_दीर्घता) {
            *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        } अन्यथा {
            u_strncpy(dest, src, src_दीर्घता);
            dest_दीर्घता = src_दीर्घता;
        }
    }
cleanup:
    यदि (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } अन्यथा {
        pBiDiTransform->dest = शून्यसूचकः;
        pBiDiTransform->p_Dest_दीर्घता = शून्यसूचकः;
        pBiDiTransform->src_दीर्घता = 0;
        pBiDiTransform->dest_आकार = 0;
    }
    प्रत्यागम (U_FAILURE(*p_Error_संहिता) && *p_Error_संहिता != U_बफर_अतिप्रवाहः_ERROR) ? 0 : dest_दीर्घता;
}
